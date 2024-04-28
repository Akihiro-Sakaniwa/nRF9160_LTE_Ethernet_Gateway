/*
 * Copyright (c) 2019 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */
#include <zephyr/logging/log.h>
#include <zephyr/logging/log_ctrl.h>
#include <nrf_errno.h>
#include <zephyr/types.h>
#include <zephyr/sys/util.h>
#include <zephyr/kernel.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <modem/at_params.h>
#include <modem/at_cmd_parser.h>

#include "nrf91_lte.h"

LOG_MODULE_REGISTER(nrf91_at_commands, CONFIG_NRF91_LOG_LEVEL);

#define OK_STR		"OK"
#define ERROR_STR	"ERROR"
#define FATAL_STR	"FATAL ERROR"

static uint8_t parse_buf[128+1];
struct at_response_list {
	int     param_type; // 0=None, 1=int, 2=str
    int     int_value;
    uint8_t str_data[128+1];
    size_t  str_len;
};
static int at_res_list_count = 0;
static struct at_response_list at_res_list[AT_MAX_PARAMS];




//uint8_t*  nrf91_at(void)
//{
//	/* Send to modem */
//	int err = nrf_modem_at_cmd(at_buf, sizeof(at_buf), "%s", at_buf);
//	if (err < 0) {
//		LOG_ERR("AT command failed: %d", err);
//		rsp_send(ERROR_STR, sizeof(ERROR_STR) - 1);
//		return NULL;
//	} else if (err > 0) {
//		LOG_ERR("AT command error, type: %d", nrf_modem_at_err_type(err));
//		return NULL;
//	}
//	if (strlen(at_buf) > 0) {
//		rsp_send("\r\n", 2);
//		rsp_send(at_buf, strlen(at_buf));
//	}
//	return at_buf;
//}


static void at_res_clear(void)
{
    at_res_list_count = 0;
    for (int i = 0; i < AT_MAX_PARAMS; i++)
    {
        //printk("%d\r\n",i);
        at_res_list[i].int_value  = 0;
        at_res_list[i].param_type = 0;
        at_res_list[i].str_len    = 0;
        memset(at_res_list[i].str_data,0,sizeof(at_res_list[i].str_data));
    }
}
static int __at_res_parse(const int parse_buf_idx, const int cur_res_idx,const int state, const uint8_t *at_response)
{
    int new_pb_idx = parse_buf_idx;
    int new_res_idx = cur_res_idx;
    int new_state = state;  // 0=ignore,1=in_int,1=in_str,3=rdy,-1=ERROR
    uint8_t c = at_response[0];
    //printk("[%c]",c);
    switch (state)
    {
    case 0: // ignore
        switch (c)
        {
        case '\0':
            at_res_list_count = new_res_idx;
            return 0;
            break;
        case ' ':
            new_state = 3;
            break;
        default:
            // ignore
            break;
        }
        break;
    case 3:  // rdy
        switch (c)
        {
        case '\0':
        case '\r':
        case '\n':
            at_res_list_count = new_res_idx;
            return 0;
            break;
        case ' ':
            return -5;
            break;
        case ',':
            //printk("\r\n");
            new_res_idx++;
            break;
        case '+':
        case '-':
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            parse_buf[new_pb_idx] = c;
            new_pb_idx++;
            new_state = 1;
            break;
        case '"':
            new_state = 2;
            break;
        default:
            return -3001;
            break;
        }
        break;
    case 4:  // after str
        switch (c)
        {
        case '\0':
        case '\r':
        case '\n':
            at_res_list_count = new_res_idx;
            return 0;
            break;
        case ',':
            new_state = 3;
            break;
        default:
            return -4001;
            break;
        }
        break;
    case 2: // in_str
        switch (c)
        {
        case '"':
            at_res_list[new_res_idx].param_type = 2;
            at_res_list[new_res_idx].str_len = new_pb_idx;
            memcpy(at_res_list[new_res_idx].str_data,parse_buf,new_pb_idx + 1);
            memset(parse_buf,0,sizeof(parse_buf));
            new_pb_idx = 0;
            new_res_idx++;
            //printk("\r\n");
            new_state = 4;
            break;
        default:
            if (c >= 0x20 && c <= 0x7E) 
            {
                parse_buf[new_pb_idx] = c;
                new_pb_idx++;
            } else {
                return -2001;
            }
            break;
        }
        break;
    case 1: // in_int
        switch (c)
        {
        case ',':
            if (strlen(parse_buf) <= 1 && (parse_buf[0] == '+' || parse_buf[0] == '-')) return -1001;
            at_res_list[new_res_idx].param_type = 1;
            at_res_list[new_res_idx].int_value =  atoi(parse_buf);
            //printk("#%d#",at_res_list[new_res_idx].int_value);
            memset(parse_buf,0,sizeof(parse_buf));
            new_pb_idx = 0;
            new_res_idx++;
            //printk("\r\n");
            new_state = 3;
            break;
        case '\r':
        case '\n':
        case '\0':
            if (strlen(parse_buf) <= 1 && (parse_buf[0] == '+' || parse_buf[0] == '-')) return -1002;
            at_res_list[new_res_idx].param_type = 1;
            at_res_list[new_res_idx].int_value =  atoi(parse_buf);
            memset(parse_buf,0,sizeof(parse_buf));
            //printk("#%d#",at_res_list[new_res_idx].int_value);
            new_pb_idx = 0;
            new_res_idx++;
            //printk("\r\n");
            at_res_list_count = new_res_idx;
            return 0;
            break;
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            parse_buf[new_pb_idx] = c;
            new_pb_idx++;
            break;
        default:
            return -1004;
            break;
        }
        break;
    default:
        return -9001;
    }
    if (new_res_idx > sizeof(at_res_list)) {
        printk("__at_response_parse: error list index overflow\r\n");
        return -9002;
    }
    if (new_pb_idx > sizeof(parse_buf)) {
        printk("__at_response_parse: error parse_buf overflow\r\n");
        return -9002;
    }
    return __at_res_parse(new_pb_idx, new_res_idx, new_state, &(at_response[1]));
}
static int at_res_parse(const uint8_t *at_response)
{
    at_res_clear();
    int ret = __at_res_parse(0, 0, 0, at_response);
    if (ret) {
        LOG_ERR("Failed to parse AT command %d", ret);
        return -EINVAL;
    }
    return ret;
}
int at_cfun_read1(int* status)
{
    static int at_status1;
    static int is_done = false;
	int rc;

    void __callback(const char *at_response)
    {
        //printk("cfun_read1: <%s>", at_response);
        const char atc[] = "+CFUN: ";
        if (0 == memcmp(at_response,atc,strlen(atc)))
        {
            at_status1 = at_response[strlen(atc)] - '0';
            is_done = true;
            //printk("cfun_read1: done.\r\n");
            return;
        }
    };

	rc = nrf_modem_at_cmd_async(__callback, "AT+CFUN?");
    if (rc == 0)
    {
        for (int i =0;i < (5000 / 10);i++)
        {
            k_sleep(K_MSEC(10));
            if (is_done) break;
        }
        if (!is_done)
        {
            rc = -NRF_EFAULT;
        } else {
            *status = at_status1;
            rc = 1;
        }
    }

	return rc;
}
int at_coneval(void)
{
    static int is_done = false;
	int rc;

    void __callback(const char *at_response)
    {
        printk("at_coneval: <%s>", at_response);
        is_done = true;
    };

	rc = nrf_modem_at_cmd_async(__callback, "AT%%CONEVAL");
    if (rc == 0)
    {
        for (int i =0;i < (5000 / 10);i++)
        {
            k_sleep(K_MSEC(10));
            if (is_done) break;
        }
        if (!is_done)
        {
            rc = -NRF_EFAULT;
        } else {
            rc = 1;
        }
    }

	return rc;
}
int at_coneval_2(int* rsrp, int* rsrq)
{
	int rc;
	int ret;

    static int is_done = false;
    static uint8_t buf[AT_MAX_CMD_LEN];

    void __callback(const char *atr)
    {
        //printk("coneval_2: <%s>", atr);
        memcpy(buf,atr,strlen(atr));

        if (0 == memcmp(atr,"%CONEVAL: ",strlen("%CONEVAL: ")))
        {
            is_done = true;
            return;
        }
    };

	rc = nrf_modem_at_cmd_async(__callback, "AT%%CONEVAL");
    if (rc == 0)
    {
        for (int i =0;i < (5000 / 10);i++)
        {
            k_sleep(K_MSEC(10));
            if (is_done) break;
        }
        if (!is_done)
        {
            rc = -NRF_EFAULT;
        } else {
            rc = 0;
            ret = at_res_parse(buf);
            //printk("\r\n\r\nparse result = [%d](%d)\r\n",ret,at_res_list_count);
            if (ret == 0) 
            {
                if (at_res_list_count >= 6)
                {
                    if (at_res_list[4].param_type == 1)
                    {
                        *rsrq = at_res_list[4].int_value;
            //printk("\r\nrsrq %d\r\n",*rsrq);
                    }
                    if (at_res_list[5].param_type == 1)
                    {
                        *rsrp = at_res_list[5].int_value;
            //printk("\r\nrsrp %d\r\n",*rsrp);
                        rc = 2;
                    }
                }   
            }
        }
    }

	return rc;
}
int at_xvbat(int* vbat_mv)
{
	int rc;
	int ret;

    static int is_done = false;
    static uint8_t buf[AT_MAX_CMD_LEN];

    void __callback(const char *atr)
    {
        memcpy(buf,atr,strlen(atr));
        if (0 == memcmp(atr,"%XVBAT: ",strlen("%XVBAT: ")))
        {
            is_done = true;
            return;
        }
    };
	rc = nrf_modem_at_cmd_async(__callback, "AT%%XVBAT");
    if (rc == 0)
    {
        for (int i =0;i < (5000 / 10);i++)
        {
            k_sleep(K_MSEC(10));
            if (is_done) break;
        }
        if (!is_done)
        {
            rc = -NRF_EFAULT;
        } else {
            rc = 0;
            ret = at_res_parse(buf);
            if (ret == 0) 
            {
                if (at_res_list_count >= 1)
                {
                    if (at_res_list[0].param_type == 1)
                    {
                        *vbat_mv = at_res_list[0].int_value;
                        rc = 1;
                    }
                }   
            }
        }
    }
	return rc;
}
int at_periodicsearchconf(void)
{
    static int is_done = false;
	int rc;

    void __callback(const char *at_response)
    {
        printk("at_periodicsearchconf: <%s>", at_response);
        is_done = true;
    };

	nrf_modem_at_printf("AT%%PERIODICSEARCHCONF=2");
	nrf_modem_at_printf("AT%%XDATAPRFL=0");
	nrf_modem_at_printf("AT%%PERIODICSEARCHCONF=0,0,0,1,\"0,10,40,,5\",\"1,300,600,1800,1800,3600\"");
	rc = nrf_modem_at_cmd_async(__callback, "AT%%PERIODICSEARCHCONF=1");
    if (rc == 0)
    {
        for (int i =0;i < (5000 / 10);i++)
        {
            k_sleep(K_MSEC(10));
            if (is_done) break;
        }
        if (!is_done)
        {
            rc = -NRF_EFAULT;
        } else {
            rc = 1;
        }
    }

	return rc;
}
int at_rel14feat_read(void)
{
    static int is_done = false;
	int rc;

    void __callback(const char *at_response)
    {
        printk("at_rel14feat: <%s>", at_response);
        is_done = true;
    };

	rc = nrf_modem_at_cmd_async(__callback, "AT%%REL14FEAT?");
    if (rc == 0)
    {
        for (int i =0;i < (5000 / 10);i++)
        {
            k_sleep(K_MSEC(10));
            if (is_done) break;
        }
        if (!is_done)
        {
            rc = -NRF_EFAULT;
        } else {
            rc = 1;
        }
    }

	return rc;
}
int at_xmon(void)
{
    static int is_done = false;
	int rc;

    void __callback(const char *at_response)
    {
        printk("xmon_read1: <%s>", at_response);
        is_done = true;
    };

	rc = nrf_modem_at_cmd_async(__callback, "AT%%XMONITOR");
    if (rc == 0)
    {
        for (int i =0;i < (5000 / 10);i++)
        {
            k_sleep(K_MSEC(10));
            if (is_done) break;
        }
        if (!is_done)
        {
            rc = -NRF_EFAULT;
        } else {
            rc = 1;
        }
    }

	return rc;
}
int at_xmon_read1(int* status)
{
    static int at_status1;
    static int is_done = false;
	int rc;

    void __callback(const char *at_response)
    {
        printk("xmon_read1: <%s>", at_response);
        const char atc[] = "%XMONITOR: ";
        if (0 == memcmp(at_response,atc,strlen(atc)))
        {
            at_status1 = at_response[strlen(atc)] - '0';
            is_done = true;
            //printk("xmon_read1: done.\r\n");
            return;
        }
    };

	rc = nrf_modem_at_cmd_async(__callback, "AT%%XMONITOR");
    if (rc == 0)
    {
        for (int i =0;i < (5000 / 10);i++)
        {
            k_sleep(K_MSEC(10));
            if (is_done) break;
        }
        if (!is_done)
        {
            rc = -NRF_EFAULT;
        } else {
            *status = at_status1;
            rc = 1;
        }
    }

	return rc;
}
int at_xmon_read2(int* status, int* carrier)
{
	int rc;
	int ret;

    static int is_done = false;
    static uint8_t buf[AT_MAX_CMD_LEN];

    void __callback(const char *atr)
    {
        //printk("xmon_read2: <%s>", atr);
        memcpy(buf,atr,strlen(atr));

        if (0 == memcmp(atr,"%XMONITOR: 5",strlen("%XMONITOR: 5")))
        {
            is_done = true;
            return;
        }
    };

	rc = nrf_modem_at_cmd_async(__callback, "AT%%XMONITOR");
    if (rc == 0)
    {
        for (int i =0;i < (5000 / 10);i++)
        {
            k_sleep(K_MSEC(10));
            if (is_done) break;
        }
        if (!is_done)
        {
            rc = -NRF_EFAULT;
        } else {
            ret = at_res_parse(buf);
            if (ret == 0) 
            {
                if (at_res_list_count >= 4)
                {
                    if (at_res_list[3].str_len == 5)
                    {
                        if (at_res_list[3].param_type == 2)
                        {
                            *status = 5;
                            *carrier = atoi(at_res_list[3].str_data);
                            rc = 2;
                        }
                    }
                }   
            }
        }
    }

	return rc;
}
int at_cesq_read2(int* rsrp, int* rsrq)
{
	int rc;
	int ret;

    static int is_done = false;
    static uint8_t buf[AT_MAX_CMD_LEN];

    void __callback(const char *atr)
    {
        //printk("cesq_read2: <%s>", atr);
        memcpy(buf,atr,strlen(atr));

        if (0 == memcmp(atr,"+CESQ: ",strlen("+CESQ: ")))
        {
            is_done = true;
            return;
        }
    };

	rc = nrf_modem_at_cmd_async(__callback, "AT+CESQ");
    if (rc == 0)
    {
        for (int i =0;i < (5000 / 10);i++)
        {
            k_sleep(K_MSEC(10));
            if (is_done) break;
        }
        if (!is_done)
        {
            rc = -NRF_EFAULT;
        } else {
            rc = 0;
            ret = at_res_parse(buf);
            //printk("\r\n\r\nparse result = [%d](%d)\r\n",ret,at_res_list_count);
            if (ret == 0) 
            {
                if (at_res_list_count >= 6)
                {
                    if (at_res_list[4].param_type == 1)
                    {
                        *rsrq = at_res_list[4].int_value;
                    }
                    if (at_res_list[5].param_type == 1)
                    {
                        *rsrp = at_res_list[5].int_value;
                        rc = 2;
                    }
                }   
            }
        }
    }

	return rc;
}


int at_cgpaddr_read2(uint8_t* v4, uint8_t* v6)
{
	int rc;
	int ret;

    static int is_done = false;
    static uint8_t buf[AT_MAX_CMD_LEN];

    void __callback(const char *atr)
    {
        printk("cgpaddr_read2: <%s>", atr);
        memcpy(buf,atr,strlen(atr));

        if (0 == memcmp(atr,"+CGPADDR: 0",strlen("+CGPADDR: 0")))
        {
            is_done = true;
            return;
        }
    };

	rc = nrf_modem_at_cmd_async(__callback, "AT+CGPADDR=%d",0);
    if (rc == 0)
    {
        for (int i =0;i < (5000 / 10);i++)
        {
            k_sleep(K_MSEC(10));
            if (is_done) break;
        }
        if (!is_done)
        {
            rc = -NRF_EFAULT;
        } else {
            ret = at_res_parse(buf);
            if (ret == 0) 
            {
                if (at_res_list_count == 2)
                {
                    //printk("cgpaddr_read2: %d,%d,%s", at_res_list[0].param_type,at_res_list[0].str_len,at_res_list[0].str_data);
                    if (at_res_list[1].param_type == 2)
                    {
                        if (v4 != NULL)
                        {
                            strncpy(v4,at_res_list[1].str_data,at_res_list[1].str_len);
                            v4[at_res_list[1].str_len] = '\0';
                            rc++;
                        } else if (v6 != NULL) {
                            strncpy(v6,at_res_list[1].str_data,at_res_list[1].str_len);
                            v6[at_res_list[1].str_len] = '\0';
                            rc++;
                        }
                    }
                } else if (at_res_list_count == 3) {
                    if (at_res_list[1].param_type == 2)
                    {
                        if (v4 != NULL)
                        {
                            strncpy(v4,at_res_list[1].str_data,at_res_list[1].str_len);
                            v4[at_res_list[1].str_len] = '\0';
                            rc++;
                        }
                    }
                    if (at_res_list[2].param_type == 2)
                    {
                        if (v6 != NULL) {
                            strncpy(v6,at_res_list[2].str_data,at_res_list[2].str_len);
                            v6[at_res_list[2].str_len] = '\0';
                            rc++;
                        }
                    }
                }
            }
        }
    }

	return rc;
}
int at_cgsn_read1(uint8_t* imei)
{
	int rc;
	int ret;

    static int is_done = false;
    static uint8_t buf[AT_MAX_CMD_LEN];

    void __callback(const char *atr)
    {
        printk("cgsn_read1: <%s>", atr);
        memcpy(buf,atr,strlen(atr));

        if (0 == memcmp(atr,"+CGSN: ",strlen("+CGSN: ")))
        {
            is_done = true;
            return;
        }
    };

	rc = nrf_modem_at_cmd_async(__callback, "AT+CGSN=%d",1);
    if (rc == 0)
    {
        for (int i =0;i < (5000 / 10);i++)
        {
            k_sleep(K_MSEC(10));
            if (is_done) break;
        }
        if (!is_done)
        {
            rc = -NRF_EFAULT;
        } else {
            ret = at_res_parse(buf);
            if (ret == 0) 
            {
                if (at_res_list_count == 1)
                {
                    //printk("cgsn_read1: %d,%d,%s", at_res_list[0].param_type,at_res_list[0].str_len,at_res_list[0].str_data);
                    if (at_res_list[0].param_type == 2)
                    {
                        strncpy(imei,at_res_list[0].str_data,at_res_list[0].str_len);
                        imei[at_res_list[0].str_len] = '\0';
                        rc++;
                    }
                }
            }
        }
    }
	return rc;
}
// EOF