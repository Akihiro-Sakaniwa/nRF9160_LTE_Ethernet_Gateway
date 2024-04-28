/*
 * Copyright (c) 2019 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#ifndef NRF91_UTIL_
#define NRF91_UTIL_

/**@file nrf91_util.h
 *
 * @brief Utility functions for NRF91
 * @{
 */

#include <zephyr/types.h>
#include <ctype.h>
#include <stdbool.h>
#include <zephyr/net/socket.h>
#include <modem/at_cmd_parser.h>


#define __BARRIER__   ;;__DMB();__DSB();__ISB();;

// クロックカウンタの差分値計算用
static inline uint32_t time_diff(uint32_t t0,uint32_t t1)
{
	if (t0 > t1)
	{
		return (0xFFFFFFFFU -  t0) + t1 + 1;

	} else {
		return t1 - t0;
	}
}

/**
 * @brief Compare string ignoring case
 *
 * @param str1 First string
 * @param str2 Second string
 *
 * @return true If two commands match, false if not.
 */
bool nrf91_util_casecmp(const char *str1, const char *str2);

/**
 * @brief Compare name of AT command ignoring case
 *
 * @param cmd Command string received from UART
 * @param nrf91_cmd Propreiatry command supported by SLM
 *
 * @return true If two commands match, false if not.
 */
bool nrf91_util_cmd_casecmp(const char *cmd, const char *nrf91_cmd);

/**
 * @brief Detect hexdecimal string data type
 *
 * @param[in] data Hexdecimal string arrary to be checked
 * @param[in] data_len Length of array
 *
 * @return true if the input is hexdecimal string array, otherwise false
 */
bool nrf91_util_hexstr_check(const uint8_t *data, uint16_t data_len);

/**
 * @brief Encode hex array to hexdecimal string (ASCII text)
 *
 * @param[in]  hex Hex arrary to be encoded
 * @param[in]  hex_len Length of hex array
 * @param[out] ascii encoded hexdecimal string
 * @param[in]  ascii_len reserved buffer size
 *
 * @return actual size of ascii string if the operation was successful.
 *           Otherwise, a (negative) error code is returned.
 */
int nrf91_util_htoa(const uint8_t *hex, uint16_t hex_len, char *ascii, uint16_t ascii_len);

/**
 * @brief Decode hexdecimal string (ASCII text) to hex array
 *
 * @param[in]  ascii encoded hexdecimal string
 * @param[in]  ascii_len size of hexdecimal string
 * @param[out] hex decoded hex arrary
 * @param[in]  hex_len reserved size of hex array
 *
 * @return actual size of hex array if the operation was successful.
 *           Otherwise, a (negative) error code is returned.
 */
int nrf91_util_atoh(const char *ascii, uint16_t ascii_len, uint8_t *hex, uint16_t hex_len);

/**
 * @brief Get string value from AT command with length check.
 *
 * @p len must be bigger than the string length, or an error is returned.
 * The copied string is null-terminated.
 *
 * @param[in]     list    Parameter list.
 * @param[in]     index   Parameter index in the list.
 * @param[out]    value   Pointer to the buffer where to copy the value.
 * @param[in,out] len     Available space in @p value, returns actual length
 *                        copied into string buffer in bytes, excluding the
 *                        terminating null character.
 *
 * @retval 0 If the operation was successful.
 *           Otherwise, a (negative) error code is returned.
 */
int util_string_get(const struct at_param_list *list, size_t index, char *value, size_t *len);

/**
 * @brief use AT command to get IPv4 and IPv6 addresses for specified PDN
 *
 * @param[in] cid PDP Context ID as defined in "+CGDCONT" command (0~10).
 * @param[out] addr4 Buffer to hold the IPv4 address, size NET_IPV4_ADDR_LEN.
 * @param[out] addr6 Buffer to hold the IPv6 address, size NET_IPV6_ADDR_LEN.
 */
//void util_get_ip_addr(int cid, char *addr4, char *addr6);

/**
 * @brief convert string to integer
 *
 * @param[in] str A string containing the representation of an integral number.
 * @param[in] base The base, which must be between 2 and 36 inclusive or the special value 0.
 * @param[out] output The converted integral number as a long int value.
 *
 * @retval 0 If the operation was successful.
 *           Otherwise, a (negative) error code is returned.
 */
int util_str_to_int(const char *str, int base, int *output);
/** @} */

int util_resolve_host(int cid, const char *host, uint16_t port, int family, struct sockaddr *sa);


typedef struct __pseuso_ip_hdr_t {
    uint8_t    ip_src[4];
    uint8_t    ip_dst[4];
    uint8_t     dummy;
    uint8_t     ip_p;
    uint16_t    ip_len;
} pseudo_ip_hdr_t;
uint16_t check_ics(const uint8_t *buffer, int len);
void calc_ics(uint8_t *buffer, int len, int hcs_pos);
uint16_t ip_checksum2(uint8_t *data1, int len1, uint8_t *data2, int len2);
#endif /* NRF91_UTIL_ */
