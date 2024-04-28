/*
 * Copyright (c) 2024 Akihiro Sakaniwa
 *
 * SPDX-License-Identifier: MIT License
 */
#include "arp_cache.h"
#include <zephyr/logging/log.h>
#include <zephyr/logging/log_ctrl.h>

LOG_MODULE_REGISTER(arp_cache, CONFIG_NRF91_LOG_LEVEL);
extern struct net_context* ctx_tx;
extern struct net_if *eth_iface;
arp_cache_tbl_t arp_cache;

extern uint8_t my_eth_ip_addr[6];
extern uint8_t my_eth_mac_addr[6];

static const uint8_t bc_mac[]={0xff,0xff,0xff,0xff,0xff,0xff};

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
static void eth_send_cb(struct net_context *context, int status, void *user_data)
{
    //printk("send_cb: %d\r\n",status);
}


static void __gc_arp_cache(uint32_t timeout_ms)
{
    for (int i = 0; i < MAX_ARP_CACHE_ROWS; i++)
    {
        if (arp_cache.row[i].ip == 0) continue;
        if (is_timeout(timeout_ms,arp_cache.row[i].ts,k_uptime_get_32()))
        {
//Serial.printf("GC\r\n");
            arp_cache.row[i].ip = 0;
        }
    }
}
arp_cache_row_t* search_arp_cache_empty(void)
{
    {
        __gc_arp_cache(ARP_CACHE_TIMEOUT_MS);
        for (int i = 0; i < MAX_ARP_CACHE_ROWS; i++)
        {
            if (arp_cache.row[i].ip == 0) return &arp_cache.row[i];
        }
    }
    return NULL;
}
arp_cache_row_t* search_arp_cache_by_ip(uint32_t tgt_ip)
{
    {
        __gc_arp_cache(ARP_CACHE_TIMEOUT_MS);
        for (int i = 0; i < MAX_ARP_CACHE_ROWS; i++)
        {
            uint32_t ip = arp_cache.row[i].ip;
            if (ip == 0) continue;
            if (ip == tgt_ip) 
            {
//Serial.printf("HIT\r\n");
                return &arp_cache.row[i];
            }
        }
    }
    return NULL;
}
arp_cache_row_t* update_arp_cache_by_ip(uint32_t tgt_ip, uint8_t* mac)
{
    {
        __gc_arp_cache(ARP_CACHE_TIMEOUT_MS);
        arp_cache_row_t* empty = NULL;
        for (int i = 0; i < MAX_ARP_CACHE_ROWS; i++)
        {
            uint32_t ip = arp_cache.row[i].ip;
            if (ip == 0)
            {
                if (!empty) empty = &arp_cache.row[i];
                continue;
            }
            if (ip == tgt_ip)
            {
//Serial.printf("update %02x:%02x:%02x:%02x:%02x:%02x\r\n",mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
                memcpy(arp_cache.row[i].mac,mac,6);
                arp_cache.row[i].ts = k_uptime_get_32();
                return &arp_cache.row[i];
            }
        }
        if (empty)
        {
//Serial.printf("add %02x:%02x:%02x:%02x:%02x:%02x\r\n",mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
            memcpy(empty->mac,mac,6);
            empty->ts = k_uptime_get_32();
            empty->ip = tgt_ip;
            return empty;
        }
    }
    return NULL;
}
//static volatile bool lock_resolve_arp = false;
bool resolve_arp(uint32_t tgt_ip, uint8_t *mac, uint32_t timeout_ms, bool use_arp_cache)
{
    uint32_t ts = k_uptime_get_32();
    static pkt_arp_t pkt_arp_req;

    if (!eth_iface)
    {
        return false;
    }

    if (use_arp_cache)
    {
        arp_cache_row_t* r = search_arp_cache_by_ip(tgt_ip);
        if (r)
        {
//Serial.printf("cache\r\n");
            memcpy(mac,r->mac,6);
            return true;
        }
    }
    //while (lock_resolve_arp)
    //{
    //    if(is_timeout(timeout_ms,ts,k_uptime_get_32()))
    //    {
    //        return false;
    //    }
    //    k_sleep(K_USEC(1));
    //};
    //lock_resolve_arp = true;
    {
        memset(&pkt_arp_req,0,sizeof(pkt_arp_req));
        memset(pkt_arp_req.eth_dst_mac,0xFF,6);
        memcpy(pkt_arp_req.eth_src_mac,my_eth_mac_addr,6);
        pkt_arp_req.frame_type[0]   = 0x08;
        pkt_arp_req.frame_type[1]   = 0x06;
        pkt_arp_req.htype[0]        = 0x00;
        pkt_arp_req.htype[1]        = 0x01;
        pkt_arp_req.ptype[0]        = 0x08;
        pkt_arp_req.ptype[1]        = 0x00;
        pkt_arp_req.hsz[0]          = 0x06;
        pkt_arp_req.psz[0]          = 0x04;
        pkt_arp_req.operation[0]    = 0x00;
        pkt_arp_req.operation[1]    = 0x01;
        memcpy(pkt_arp_req.src_mac,my_eth_mac_addr,6);
        memcpy(pkt_arp_req.src_ip,my_eth_ip_addr,4);
        memset(pkt_arp_req.dst_mac,0,6);
        memcpy(pkt_arp_req.dst_ip,(uint8_t*)&tgt_ip,4);
//for(int i = 0; i < sizeof(pkt_arp_req);i++)
//{
//    if (i % 16 == 0) Serial.printf("\r\n");
//    Serial.printf("%02x ",((uint8_t*)&pkt_arp_req)[i]);
//}
//Serial.printf("\r\n");
        struct sockaddr_ll tgt_addr;
        memset(&tgt_addr,0,sizeof(tgt_addr));
        memset(tgt_addr.sll_addr,0xFF,6);
        tgt_addr.sll_family = AF_PACKET;
        tgt_addr.sll_protocol = 0x0806;
        tgt_addr.sll_hatype = 1;
        tgt_addr.sll_ifindex = 0;
        tgt_addr.sll_halen = 6;
        tgt_addr.sll_pkttype = PACKET_BROADCAST;
        int r = net_context_sendto(ctx_tx, &pkt_arp_req, sizeof(pkt_arp_req),(struct sockaddr *)&tgt_addr,sizeof(tgt_addr),eth_send_cb,K_MSEC(1000),NULL);
        if (r <= 0)
        {
            LOG_ERR("ARP ETH_TX error send (%d)",r);
        }
        else
        {
//Serial.printf("arp send\r\n");
            while (!is_timeout(timeout_ms,ts,k_uptime_get_32()))
            {
                arp_cache_row_t* r = search_arp_cache_by_ip(tgt_ip);
                if (r)
                {
//Serial.printf("resolve %02x:%02x:%02x:%02x:%02x:%02x\r\n",r->mac[0],r->mac[1],r->mac[2],r->mac[3],r->mac[4],r->mac[5]);
                    memcpy(mac,r->mac,6);
                    return true;
                }
                k_sleep(K_USEC(1));
            }
        }
    }
    //lock_resolve_arp = false;
    return false;
 
}
void operate_packet_arp(uint8_t *buffer, uint32_t length)
{
//printk("__ARP__\r\n");
    //static pkt_arp_t pkt_arp_ack;
    if (length < 42) return;
//printk("__ARP__1\r\n");
    if (0 != memcmp(&buffer[14+24],my_eth_ip_addr,4)) return;
//printk("__ARP__2\r\n");
    uint16_t htype = (((uint16_t)buffer[14+0]) << 8) + ((uint16_t)buffer[14+1]);
    if (htype != 0x0001) return;
//printk("__ARP__3\r\n");
    uint16_t ptype = (((uint16_t)buffer[14+2]) << 8) + ((uint16_t)buffer[14+3]);
    if (ptype != 0x0800) return;
//printk("__ARP__4\r\n");
    if (buffer[14+4] != 0x06) return; // HARD SIZE
//printk("__ARP__5\r\n");
    if (buffer[14+5] != 0x04) return; // PROTOCOL SIZE
    uint16_t operation = (((uint16_t)buffer[14+6]) << 8) + ((uint16_t)buffer[14+7]);
//printk("__ARP__6\r\n");

    if ( (operation == 0x0001) && (0 == memcmp(&buffer[0],bc_mac,6)) )
    {
        update_arp_cache_by_ip(*(uint32_t*)(&buffer[14+14]),&buffer[14+8]);
    }
    else if ( (operation == 0x0002) && (0 == memcmp(&buffer[0],my_eth_mac_addr,6)) )
    {
    // get  ARP RESPONCE, update ARP CACHE
        pkt_arp_t* arp = (pkt_arp_t*)buffer;
//Serial.printf("resolve %02x:%02x:%02x:%02x:%02x:%02x\r\n",arp->src_mac[0],arp->src_mac[1],arp->src_mac[2],arp->src_mac[3],arp->src_mac[4],arp->src_mac[5]);
        update_arp_cache_by_ip(*(uint32_t*)(arp->src_ip),arp->src_mac);        
    }
}