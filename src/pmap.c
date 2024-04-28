/*
 * Copyright (c) 2024 Akihiro Sakaniwa
 *
 * SPDX-License-Identifier: MIT License
 */
#include "pmap.h"
#include <zephyr/logging/log.h>
#include <zephyr/logging/log_ctrl.h>

LOG_MODULE_REGISTER(pmap, CONFIG_NRF91_LOG_LEVEL);
extern struct net_context* ctx_tx;
extern struct net_if *eth_iface;
pmap_tbl_t pmap;

static bool is_timeout(uint32_t timeout_ms,uint32_t start_ms,uint32_t current_ms)
{
    if (start_ms <= current_ms)
    {
        return ((current_ms - start_ms) >= timeout_ms);
    }
    else
    {
        return ((0xFFFFFFFFUL - start_ms + current_ms + 1) >= timeout_ms);
    }
}
static void __gc_pmap(uint32_t timeout_ms)
{
    for (int i = 0; i < MAX_PMAP_ROWS; i++)
    {
        if (pmap.row[i].ip_protocol == 0) continue;
        if (is_timeout(timeout_ms,pmap.row[i].ts,k_uptime_get_32()))
        {
            pmap.row[i].ip_protocol = 0;
            pmap.row[i].server_ip   = 0;
            pmap.row[i].client_ip   = 0;
            pmap.row[i].lte_port    = 0;
            pmap.row[i].eth_port    = 0;
        }
    }
}
pmap_row_t* search_pmap_empty(void)
{
    __gc_pmap(PMAP_TIMEOUT_MS);
    for (int i = 0; i < MAX_PMAP_ROWS; i++)
    {
        if (pmap.row[i].ip_protocol == 0) return &pmap.row[i];
    }
    return NULL;
}
pmap_row_t* search_pmap_by_server_info(uint8_t ip_protocol,uint32_t server_ip,uint16_t lte_port)
{
    __gc_pmap(PMAP_TIMEOUT_MS);
    for (int i = 0; i < MAX_PMAP_ROWS; i++)
    {
        uint8_t protocol = pmap.row[i].ip_protocol;
        uint16_t port = pmap.row[i].lte_port;
        uint32_t ip = pmap.row[i].server_ip;
        if (protocol == 0) continue;
        if (protocol == ip_protocol && ip == server_ip && port == lte_port) 
        {
//printk("HIT search_pmap_by_server_info\r\n");
            pmap.row[i].ts = k_uptime_get_32();
            return &pmap.row[i];
        }
    }
    return NULL;
}
pmap_row_t* search_pmap_by_client_info(uint8_t ip_protocol,uint32_t client_ip,uint16_t eth_port)
{
    __gc_pmap(PMAP_TIMEOUT_MS);
    for (int i = 0; i < MAX_PMAP_ROWS; i++)
    {
        uint8_t protocol = pmap.row[i].ip_protocol;
        uint16_t port = pmap.row[i].eth_port;
        uint32_t ip = pmap.row[i].client_ip;
        if (protocol == 0) continue;
        if (protocol == ip_protocol && ip == client_ip && port == eth_port) 
        {
//printk("HIT search_pmap_by_client_info\r\n");
            pmap.row[i].ts = k_uptime_get_32();
            return &pmap.row[i];
        }
    }
    return NULL;
}
pmap_row_t* update_pmap_by_serevr_info(uint8_t ip_protocol,uint32_t server_ip, uint16_t lte_port)
{
    __gc_pmap(PMAP_TIMEOUT_MS);
    for (int i = 0; i < MAX_PMAP_ROWS; i++)
    {
        uint8_t protocol = pmap.row[i].ip_protocol;
        uint16_t port = pmap.row[i].lte_port;
        uint32_t ip = pmap.row[i].server_ip;
        if (protocol == 0)
        {
            continue;
        }
        if (protocol == ip_protocol && ip == server_ip && port == lte_port)
        {
//printk("HIT update_pmap_by_serevr_info\r\n");
            pmap.row[i].ts = k_uptime_get_32();
            return &pmap.row[i];
        }
    }
    return NULL;
}
pmap_row_t* update_pmap_by_client_info(uint8_t ip_protocol,uint32_t client_ip, uint16_t eth_port)
{
    __gc_pmap(PMAP_TIMEOUT_MS);
    for (int i = 0; i < MAX_PMAP_ROWS; i++)
    {
        uint8_t protocol = pmap.row[i].ip_protocol;
        uint16_t port = pmap.row[i].eth_port;
        uint32_t ip = pmap.row[i].client_ip;
        if (protocol == 0)
        {
            continue;
        }
        if (protocol == ip_protocol && ip == client_ip && port == eth_port)
        {
//printk("HIT update_pmap_by_client_info\r\n");
            pmap.row[i].ts = k_uptime_get_32();
            return &pmap.row[i];
        }
    }
    return NULL;
}
void remove_pmap_by_serevr_info(uint8_t ip_protocol,uint32_t server_ip, uint16_t lte_port)
{
    __gc_pmap(PMAP_TIMEOUT_MS);
    for (int i = 0; i < MAX_PMAP_ROWS; i++)
    {
        uint8_t protocol = pmap.row[i].ip_protocol;
        uint16_t port = pmap.row[i].lte_port;
        uint32_t ip = pmap.row[i].server_ip;
        if (protocol == 0)
        {
            continue;
        }
        if (protocol == ip_protocol && ip == server_ip && port == lte_port)
        {
            pmap.row[i].ip_protocol = 0;
            pmap.row[i].server_ip   = 0;
            pmap.row[i].client_ip   = 0;
            pmap.row[i].lte_port    = 0;
            pmap.row[i].eth_port    = 0;
            pmap.row[i].ts          = 0;
            return;
        }
    }
    return;
}
void remove_pmap_by_client_info(uint8_t ip_protocol,uint32_t client_ip, uint16_t eth_port)
{
    __gc_pmap(PMAP_TIMEOUT_MS);
    for (int i = 0; i < MAX_PMAP_ROWS; i++)
    {
        uint8_t protocol = pmap.row[i].ip_protocol;
        uint16_t port = pmap.row[i].eth_port;
        uint32_t ip = pmap.row[i].client_ip;
        if (protocol == 0)
        {
            continue;
        }
        if (protocol == ip_protocol && ip == client_ip && port == eth_port)
        {
            pmap.row[i].ip_protocol = 0;
            pmap.row[i].server_ip   = 0;
            pmap.row[i].client_ip   = 0;
            pmap.row[i].lte_port    = 0;
            pmap.row[i].eth_port    = 0;
            pmap.row[i].ts          = 0;
            return;
        }
    }
    return;
}
void init_pmap(void)
{
    memset(&pmap,0,sizeof(pmap));
}
pmap_row_t* add_pmap(uint8_t ip_protocol, uint32_t client_ip,uint16_t eth_port,uint32_t server_ip,uint16_t lte_port)
{
    __gc_pmap(PMAP_TIMEOUT_MS);
    pmap_row_t* row = search_pmap_empty();
//printk("empty row : %04x\r\n",(uint32_t)row);
    if (row == NULL) return NULL;
    row->ip_protocol = ip_protocol;
    row->client_ip   = client_ip;
    row->eth_port    = eth_port;
    row->server_ip   = server_ip;
    row->lte_port    = lte_port;
    row->ts          = k_uptime_get_32();
    return row;
}

uint8_t pmap_status(void)
{
    uint32_t cnt = 0;
    for (int i = 0; i < MAX_PMAP_ROWS; i++)
    {
        if (pmap.row[i].ip_protocol == 0)
        {
            continue;
        }
        cnt++;
    }
    return (uint8_t)(cnt * 100 / MAX_PMAP_ROWS);
}