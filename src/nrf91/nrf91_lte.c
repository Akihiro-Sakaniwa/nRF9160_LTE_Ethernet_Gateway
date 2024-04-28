/*
 * Copyright (c) 2019 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */
#include <zephyr/logging/log.h>
#include <zephyr/logging/log_ctrl.h>

#include <zephyr/types.h>
#include <zephyr/sys/util.h>
#include <zephyr/kernel.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include <zephyr/drivers/gpio.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>

#include <nrf_modem.h>
#include <nrf_errno.h>
#include <hal/nrf_gpio.h>
#include <hal/nrf_power.h>
#include <hal/nrf_regulators.h>
#include <modem/nrf_modem_lib.h>

#include <modem/lte_lc.h>

#include <zephyr/sys/reboot.h>
#include <zephyr/init.h>
#include <zephyr/sys/reboot.h>
#include "ncs_version.h"

#include <modem/modem_info.h>

#include <nrf_socket.h>
#include <zephyr/net/net_ip.h>

#include "nrf91_lte.h"

#include <modem/at_monitor.h>
#include <stdlib.h>

LOG_MODULE_REGISTER(nrf91_lte, CONFIG_NRF91_LOG_LEVEL);

extern void status_led_on(void);
extern void status_led_off(void);

static uint8_t cyc[] = "|-\\|/-\\";
extern struct in_addr my_ip_addr;

volatile bool is_on_terminate;
void handle_reset(void)
{
	/*
	 * The LTE modem also needs to be stopped by issuing AT command
	 * through the modem API, before entering System OFF mode.
	 * Once the command is issued, one should wait for the modem
	 * to respond that it actually has stopped as there may be a
	 * delay until modem is disconnected from the network.
	 * Refer to https://infocenter.nordicsemi.com/topic/ps_nrf9160/
	 * pmu.html?cp=2_0_0_4_0_0_1#system_off_mode
	 */
	(void)nrf_modem_at_printf("AT+CFUN=0");
	k_sleep(K_SECONDS(1));
	sys_reboot(SYS_REBOOT_COLD);
}

void nrf_modem_recoverable_error_handler(uint32_t err)
{
	LOG_ERR("Modem library recoverable error: %u", err);
}

bool is_cgev_act_on = false;
bool is_cgev_detach_on = false;
static uint8_t at_evt_res[AT_MAX_CMD_LEN];
//AT_MONITOR(at_notify, "+CGEV", notification_handler);
//AT_MONITOR_ISR(at_notify, "+CGEV", notification_handler);
static void notification_handler(const char *response)
{
	memset(at_evt_res,0,sizeof(at_evt_res));
	strncpy(at_evt_res,response,sizeof(at_evt_res));
	/* Forward the data over UART */
	printk("\r\n+++++++>>>> ");
	printk("%s\r\n",at_evt_res);
	if ( (strlen(at_evt_res) >= strlen(CGEV_ACT)) && (0 == memcmp(at_evt_res,CGEV_ACT,strlen(CGEV_ACT))) ) 
	{
		is_cgev_act_on = true;
		status_led_on();
	}
	if ( (strlen(at_evt_res) >= strlen(CGEV_DETACH)) && (0 == memcmp(at_evt_res,CGEV_DETACH,strlen(CGEV_DETACH))) )
	{
		is_cgev_detach_on = true;
		status_led_off();
	}
}
void set_at_notification(void)
{
	nrf_modem_at_notif_handler_set(notification_handler);
}
void reset_at_notification(void)
{
	nrf_modem_at_notif_handler_set(NULL);
}
static int nrf91_lte_disconnect(int dc_type)
{
	printk("\r\n=1=======\r\n");
	if (0 != nrf_modem_at_printf("AT+CGEREP=1"))
	{
		printk("err set cfun=%d\r\n",dc_type);
		return -1;
	}
	is_cgev_detach_on = false;
	if (0 == nrf_modem_at_printf("AT+CFUN=%d",dc_type))
	{
		for (int i=0;i < (10000/50);i++)
		{
			k_sleep(K_MSEC(50));
			if (is_cgev_detach_on) break;
		}
		if (!is_cgev_detach_on) return -2;
		is_cgev_detach_on = false;
	} else {
		printk("err set cfun=%d\r\n",dc_type);
		return -3;
	}
	printk("\r\n=3=======\r\n");

	int v1;
	int rc;
	for (int i = 0; i < (10000/250);i++)
	{
		k_sleep(K_MSEC(250));
		rc = at_xmon_read1(&v1);
		if (rc == 1 && v1 != 5 && v1 != 2){
			printk("\r\n=2=======OK\r\n");
			return 0;
		}
	}
	printk("\r\n=4=======ERR-999\r\n");
	return -999;

}
static int nrf91_lte_connect(bool is_init)
{
	k_sleep(K_MSEC(1000));

	int v1;
	int rc;

	rc = at_cfun_read1(&v1);
	if (rc != 1) 
	{
		return -1;
	}
	if (v1 != 0 && v1 != 4) 
	{
		nrf91_lte_disconnect(is_init ? 0 : 4);
	}
	//if (0 != nrf_modem_at_printf("AT%%XSYSTEMMODE=1,0,0,1"))
	if (0 != nrf_modem_at_printf("AT%%XSYSTEMMODE=1,0,0,0"))
	{
		LOG_ERR("%s","AT%%XSYSTEMMODE Failed.");
	}
	if (psm_mode == 1)
	{
		if (0 != nrf_modem_at_printf("AT+CPSMS=1,,,\"%s\",\"%s\"",psm_tau,psm_act))
		{
			LOG_ERR("AT+CPSMS=%d Failed(1).",psm_mode);
		}
	} else {
		if (0 != nrf_modem_at_printf("AT+CPSMS=0"))
		{
			LOG_ERR("AT+CPSMS=0 Failed(1).");
		}
	}
	if (0 != nrf_modem_at_printf("AT+CEPPI=1"))
	{
		LOG_ERR("AT+CEPPI=1 Failed(1).");
	}
	if (0 != nrf_modem_at_printf("AT%%XRAI=4"))
	{
		LOG_ERR("AT%%XRAI Failed(1).");
	}
	//if (0 != nrf_modem_at_printf("AT%%XCOEX2=3"))
	//{
	//	LOG_ERR("AT%XCOEX2 Failed(1).");
	//}
	//if (0 != nrf_modem_at_printf("AT%%XEMPR=1,0,%d",2))
	if (0 != nrf_modem_at_printf("AT%%XEMPR=1,0,%d",0))
	{
		LOG_ERR("AT%%XEMPR Failed(1).");
	}
	//if (0 != nrf_modem_at_printf("AT+CEDRXS=%d,4,\"%04d\"",edrx_mode,edrx_value))
	//{
	//	LOG_ERR("AT+CEDRXS=%d,\"%04d\" Failed(1).",edrx_mode,edrx_value);
	//	cs->error_count++;
	//}
	if (edrx_mode == 1)
	{
		if (0 != nrf_modem_at_printf("AT+CEDRXS=%d,4,\"%04d\"",edrx_mode,edrx_value))
		{
			LOG_ERR("AT+CEDRXS=%d,\"%04d\" Failed(1).",edrx_mode,edrx_value);
		}
		if (0 != nrf_modem_at_printf("AT%%XPTW=4,\"%04d\"",edrx_ptw))
		{
			LOG_ERR("AT+CEDRXS=%d,\"%04d\" Failed(1).",edrx_mode,edrx_value);
		}
		if (0 != nrf_modem_at_printf("AT+SSRDA=1,1,0"))
		{
			LOG_ERR("AT+SSRDA Failed(1).");
		}
	} else {
		if (0 != nrf_modem_at_printf("AT+CEDRXS=%d",0))
		{
			LOG_ERR("AT+CEDRXS=%d Failed(1).",0);
		}
	}
	//if (0 != nrf_modem_at_printf("AT%%XMODEMSLEEP=1,2000,10240"))
	//{
	//	LOG_ERR("%s","AT%XMODEMSLEEP=1 Failed.");
	//	return -1;
	//}
	if (0 != nrf_modem_at_printf("AT%%XNETTIME=0")) // Disable NITS support
	{
		LOG_ERR("%s","AT%XNETTIME=0 Failed.");
	}
	if (0 != nrf_modem_at_printf("AT+CGEREP=1"))
	{
		LOG_ERR("%s","AT+CGREP=1 Failed.");
	}
	if (0 != nrf_modem_at_printf("AT+COPS=1,2,\"%d\",7",CARRIER_1))
	{
		;;
	}
#ifdef CONFIG_SIM_SORACOM_PLAN_D
	if (0 != nrf_modem_at_printf("AT%%XBANDLOCK=1,\"1100000000010000101\""))
	{
		;;
	}
	if (0 != nrf_modem_at_printf("AT+CGDCONT=0,\"IP\",\"soracom.io\",\"sora\",\"sora\""))
	{
		;;
	}
#endif

	is_cgev_act_on = false;
	if (0 == nrf_modem_at_printf("AT+CFUN=1"))
	{
		//for (int i=0;i < (360000/50);i++)
		//for (int i=0;i < (180000/50);i++)
		for (int i=0;i < (30000/1000);i++)
		{
			printk("\b%c",cyc[i%6]);
			k_sleep(K_MSEC(1000));
			if (is_cgev_act_on) break;
		}
		if (!is_cgev_act_on)
		{
			LOG_ERR("AT+CFUN=1 failed");
			return -5;
		}
	} else {
		printk("err set cfun=1\r\n");
		return -6;
	}
	return 0;
}
static void nrf91_main_loop_lte(int (*cbf)(void))
{
	int v1;
	int rc;
	set_at_notification();

	k_sleep(K_MSEC(500));
	is_cgev_act_on = false;
	status_led_off();
	is_cgev_detach_on = false;
	rc = at_cfun_read1(&v1);
	if (rc != 1) return;
	if (v1 == 0 || v1 == 4)
	{
		while (!is_cgev_act_on)
		{
			int ret = nrf91_lte_connect(true);
			if (ret == 0) {
				k_yield();
                k_sleep(K_MSEC(200));
				;;
			} else if (ret >= -4) {
				printk("error nrf91_lte_connect = %d --> exit\r\n",ret);
				goto exit_lte_loop;
			} else {
				printk("error nrf91_lte_connect = %d --> retry\r\n",ret);
			}
		}
	}
	k_sleep(K_MSEC(500));

//printk("start_lte_loop\r\n");
	while (true)
	{
//printk("lte_loop begin\r\n");
		rc = at_cfun_read1(&v1);
		if (rc != 1) break;
		if (v1 == 0 || v1 == 4)
		{
			is_cgev_act_on = false;
			status_led_off();
			while (!is_cgev_act_on)
			{
				int ret = nrf91_lte_connect(true);
				if (ret == 0) {
					;;
				} else if (ret >= -4) {
					printk("error nrf91_lte_connect = %d --> exit\r\n",ret);
					goto exit_lte_loop;
				} else {
					printk("error nrf91_lte_connect = %d --> retry\r\n",ret);
				}
			}
		}
		if (0 != nrf_modem_at_printf("AT+CGEREP=1")) { goto exit_lte_loop; }

		if (psm_mode ==1)
		{
			if (0 != nrf_modem_at_printf("AT+CPSMS=0"))
			{
				LOG_ERR("AT+CPSMS=0 Failed().");
			}
		}
		if (0 != nrf_modem_at_printf("AT+CEPPI=0")) // Normal Power Configuration
		{
			LOG_ERR("AT+CEPPI=0 Failed(1).");
		}
		if (0 != nrf_modem_at_printf("AT%%XDATAPRFL=4")) // High Performance
		{
			LOG_ERR("AT%%XDATAPRFL=4 Failed(1).");
		}		
		static uint8_t strbuf[] = "xxx.xxx.xxx.xxx";
        memset(strbuf,0,sizeof(strbuf));
        int err = modem_info_string_get(MODEM_INFO_IP_ADDRESS, strbuf, sizeof(strbuf));
        if (err <= 0)
        {
            LOG_INF("can not get MODEM_INFO_IP_ADDRESS err = %d", err);
            k_sleep(K_MSEC(1000));
            continue;
        }
        err = net_addr_pton(AF_INET,strbuf,&my_ip_addr);
        if (err < 0)
        {
            printk("can not convert IP_ADDRESS %s err = %d\r\n", strbuf,err);
            k_sleep(K_MSEC(1000));
            continue;
        }
		while (is_cgev_act_on && !is_cgev_detach_on)
		{
//at_xmon();
			cbf();
		}
		k_sleep(K_MSEC(500));
//printk("lte_loop end\r\n");
	}
exit_lte_loop:
//printk("exit_lte_loop\r\n");
	rc = at_cfun_read1(&v1);
	if (rc != 1) return;
	nrf91_lte_disconnect(0);
}

void nrf91_lte_main(int (*cbf)(void))
{
	__COMPILER_BARRIER();
	is_on_terminate = false;
	__COMPILER_BARRIER();
	nrf91_main_loop_lte(cbf);
	status_led_off();
	printk("Rebooting...\r\n");
	handle_reset();
}
