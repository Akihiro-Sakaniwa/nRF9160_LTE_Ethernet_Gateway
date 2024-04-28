/*
 * Copyright (c) 2019 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#ifndef NRF91_LTE_
#define NRF91_LTE_
#include <zephyr/types.h>
#include <ctype.h>
#include <nrf_modem_at.h>
#include <modem/at_monitor.h>


#define AT_MAX_CMD_LEN			2048
#define AT_MAX_PARAMS			42


//#ifdef CONFIG_SIM_SAKURA_IOT
//#define CARRIER_1 				44020
//#define CARRIER_2 				44051
//#define CARRIER_3 				44010
//#endif
#ifdef CONFIG_SIM_SORACOM_PLAN_D
#define CARRIER_1 				44010 
#endif

static const uint8_t psm_mode = 0;
static const uint8_t edrx_mode = 0;

static const int edrx_value =  0; // 5.12s
//static const int edrx_value =  1; // 10.24s
//static const int edrx_value =  10; // 20.48s
//static const int edrx_value =  11; // 40.96s
//static const int edrx_value = 100; // 61.44s
//static const int edrx_value = 101; // 81.92s
//static const int edrx_value = 110; // 102.4s
//static const int edrx_value = 111; // 122.88s
//static const int edrx_value = 1000; // 143.36s
//static const int edrx_value = 1001; // 163.84s
//static const int edrx_value = 1010; // 327.68s

static const int edrx_ptw = 0; // 1.28s
//static const int edrx_ptw = 1; // 2.56s
//static const int edrx_ptw = 10; // 3.84
//static const int edrx_ptw = 11; // 5.12s
//static const int edrx_ptw = 100; // 6.4s
//static const int edrx_ptw = 101; // 7.68s
//static const int edrx_ptw = 110; // 8.96s
//static const int edrx_ptw = 111; // 10.24s
//static const int edrx_ptw = 1000; // 11.52s
//static const int edrx_ptw = 1001; // 12.8s
//static const int edrx_ptw = 1010; // 14.08s
//static const int edrx_ptw = 1011; // 15.36s
//static const int edrx_ptw = 1100; // 16.64s
//static const int edrx_ptw = 1101; // 17.92s
//static const int edrx_ptw = 1110; // 19.20s
//static const int edrx_ptw = 1111; // 20.48s

//static const uint8_t psm_tau[] = "10101010";
//static const uint8_t psm_act[] = "00100001";
//static const uint8_t psm_tau[] = "00100110";
////static const uint8_t psm_act[] = "00000010";
//static const uint8_t psm_tau[] = "00100010";
////static const uint8_t psm_act[] = "00000000";
static const uint8_t psm_tau[] = "11100000";
static const uint8_t psm_act[] = "11100000";


int at_xvbat(int* vbat_mv);
int at_cfun_read1(int* status);
int at_cgsn_read1(uint8_t* imei);
int at_xmon(void);
int at_xmon_read1(int* status);
int at_xmon_read2(int* status, int* carrier);
int at_cesq_read2(int* rsrp, int* rsrq);
int at_coneval_2(int* rsrp, int* rsrq);
int at_cgpaddr_read2(uint8_t* v4, uint8_t* v6);
#define CGEV_ACT	    "+CGEV: ME PDN ACT"
#define CGEV_DETACH	    "+CGEV: ME DETACH"
#define CGEV_XMSLP 		"%XMODEMSLEEP: 1"
#define CGEV_XMSLP_WP	"%XMODEMSLEEP: 1,0"
#define CGEV_XMSLP_DRX 	"%XMODEMSLEEP: 2"
#define CGEV_CSCON_0	"+CSCON: 0"
#define CGEV_CSCON_1	"+CSCON: 1"



void handle_reset(void);


#endif /* NRF91_LTE_ */
