/*
 * Copyright (c) 2024 Akihiro Sakaniwa
 *
 * SPDX-License-Identifier: MIT License
 */
#ifndef __ARP_H__
#define __ARP_H__

#include <zephyr/kernel.h>
#include <zephyr/net/net_pkt.h>
#include <zephyr/net/net_if.h>
#include <zephyr/net/net_l2.h>
#include <zephyr/net/net_ip.h>
#include <zephyr/net/net_core.h>
#include <zephyr/net/net_context.h>

#define ARP_CACHE_TIMEOUT_MS (20*60*1000)
#define MAX_ARP_CACHE_ROWS (96)

typedef struct __arp_cache_row_t
{
    uint32_t ip;
    uint8_t  mac[6];
    uint32_t ts;
} arp_cache_row_t;

typedef struct __arp_cache_tbl_t
{
    arp_cache_row_t  row[MAX_ARP_CACHE_ROWS];
} arp_cache_tbl_t;

typedef struct __pkt_arp_t
{
    uint8_t     eth_dst_mac[6];
    uint8_t     eth_src_mac[6];
    uint8_t     frame_type[2];

    uint8_t     htype[2];
    uint8_t     ptype[2];
    uint8_t     hsz[1];
    uint8_t     psz[1];
    uint8_t     operation[2];
    uint8_t     src_mac[6];
    uint8_t     src_ip[4];
    uint8_t     dst_mac[6];
    uint8_t     dst_ip[4];
    uint8_t     padding[18];
} pkt_arp_t;

extern arp_cache_tbl_t arp_cache;

void operate_packet_arp(uint8_t *buffer, uint32_t length);

void gc_arp_cache(uint32_t timeout_ms);
arp_cache_row_t* search_arp_cache_empty(void);
arp_cache_row_t* search_arp_cache_by_ip(uint32_t tgt_ip);
arp_cache_row_t* update_arp_cache_by_ip(uint32_t tgt_ip, uint8_t* mac);
bool resolve_arp(uint32_t tgt_ip, uint8_t *mac, uint32_t timeout_ms, bool use_arp_cache);


#endif  // __ARP_H__