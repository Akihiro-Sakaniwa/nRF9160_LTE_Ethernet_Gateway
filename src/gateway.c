/*
 * Copyright (c) 2024 Akihiro Sakaniwa
 *
 * SPDX-License-Identifier: MIT License
 */
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <modem/modem_info.h>
#include <zephyr/posix/poll.h>

#include <zephyr/net/net_pkt.h>
#include <zephyr/net/net_if.h>
#include <zephyr/net/net_l2.h>
#include <zephyr/net/net_ip.h>
#include <zephyr/net/net_core.h>
#include <zephyr/net/net_context.h>
#include <zephyr/net/socket.h>
#include <zephyr/posix/poll.h>
#include <zephyr/posix/sys/socket.h>

#include "nrf91/nrf91_util.h"
#include "arp_cache.h"
#include "pmap.h"
#include "main.h"

LOG_MODULE_REGISTER(gateway, CONFIG_NRF91_LOG_LEVEL);

extern volatile bool on_req_eth_thread_terminate;
extern volatile bool on_eth_rx_thread_terminate;
extern volatile bool on_eth_tx_thread_terminate;

extern bool is_cgev_act_on;
extern bool is_cgev_detach_on;

struct net_if *eth_iface;
static volatile bool eth_lock = false;

static int lte_sock;
static volatile bool lock = false;

#define RX_THREAD_STACK_SIZE	KB(16)
#define TX_THREAD_STACK_SIZE	KB(16)
#define THREAD_PRIORITY		K_LOWEST_APPLICATION_THREAD_PRIO
static struct k_thread eth_rx_thread;
static K_THREAD_STACK_DEFINE(eth_rx_thread_stack, RX_THREAD_STACK_SIZE);
static k_tid_t eth_rx_thread_id;
static struct k_thread eth_tx_thread;
static K_THREAD_STACK_DEFINE(eth_tx_thread_stack, TX_THREAD_STACK_SIZE);
static k_tid_t eth_tx_thread_id;

//static uint8_t client_ip[] = {192,168,10,100};
//static uint8_t client_mac[] = {0xa8,0x20,0x66,0x54,0xe6,0xd9};
uint8_t my_eth_mac_addr[6] = {0,0,0,0,0,0};
uint8_t my_eth_ip_addr[4] = {0,0,0,0};
extern uint8_t GenerateNewTxSerialNumber( void );
static struct net_context* ctx_rx;
struct net_context* ctx_tx;
struct in_addr my_ip_addr = {.s4_addr32 = 0}; 


/*****************************************************************************/
static const uint16_t port_seq_base = 32768;
static const uint16_t port_seq_max = MAX_PMAP_ROWS * 4;
static uint16_t port_seq = 0; 
static uint16_t lte_port_genarator(void)
{
    uint16_t port = 0;
    while (port == 0)
    {
        port_seq = (port_seq + 1 ) % (port_seq_max + 1);
        uint16_t tmp = port_seq + port_seq_base;
        bool is_dup = false;
        for (int i =0;i < MAX_PMAP_ROWS;i++)
        {
            if (pmap.row[i].ip_protocol == 0) continue;
            if (pmap.row[i].ip_protocol == IPPROTO_ICMP) continue;
            if (pmap.row[i].lte_port == tmp)
            {
                is_dup = true;
                break;
            }
        }
        if (!is_dup)
        {
            port = tmp;
            break;
        }
    }
    return port;
}
static uint16_t lte_port_genarator_icmp_echo(void)
{
    uint16_t port = 0;
    while (port == 0)
    {
        port_seq = (port_seq + 1 ) % (port_seq_max + 1);
        uint16_t tmp = port_seq % 256;
        bool is_dup = false;
        for (int i =0;i < MAX_PMAP_ROWS;i++)
        {
            if (pmap.row[i].ip_protocol != IPPROTO_ICMP) continue;
            if (pmap.row[i].lte_port == tmp)
            {
                is_dup = true;
                break;
            }
        }
        if (!is_dup)
        {
            port = tmp;
            break;
        }
    }
    return port;
}

/*****************************************************************************/

static uint16_t ip_htons( uint16_t hostshort)
{
#if 1
  //#ifdef LITTLE_ENDIAN
    uint16_t netshort=0;
    netshort = (hostshort & 0xFF) << 8;
    netshort |= ((hostshort >> 8)& 0xFF);
    return netshort;
#else
    return hostshort;
#endif
}
////////////////////////////////////////////////////////////////////////////////
static void eth_send_cb(struct net_context *context, int status, void *user_data)
{
    //printk("send_cb: %d\r\n",status);
}
////////////////////////////////////////////////////////////////////////////////
typedef struct  L2_PAYLOAD
{
    uint16_t size;
    uint8_t  data[NRF9160_MTU];
} L2_PAYLOAD_t;
static L2_PAYLOAD_t lte_tx_ip_pkt;
void eth_recv_cb(struct net_context *context, struct net_pkt *pkt, union net_ip_header *ip_hdr, union net_proto_header *proto_hdr, int status, void *user_data)
{


    //printk("recv_cb: %d\r\n",status);
    if (status != 0 && pkt)
    {
        net_buf_unref(pkt->buffer);
        net_pkt_unref(pkt);
        return;
    }
    else if (status == 0 && pkt)
    {
        if  (!is_cgev_act_on || is_cgev_detach_on)
        {
            net_buf_unref(pkt->buffer);
            net_pkt_unref(pkt);
            return;
        }
        struct net_pkt * np = net_pkt_clone(pkt,K_FOREVER);
        net_buf_unref(pkt->buffer);
        net_pkt_unref(pkt);

        uint16_t len = np->buffer->len;
        uint8_t* data = np->buffer->data;

        if ( (data[12] == 0x08) && (data[13] == 0x00) )
        {
		    volatile bool is_last_frag = false;
            uint16_t dsz = 0;
            uint16_t tsz = (((uint16_t)data[14+2]) << 8) + (((uint16_t)data[14+3]) << 0);
            if (sizeof(lte_tx_ip_pkt.data) < tsz)
            {
                net_buf_unref(np->buffer);
                net_pkt_unref(np);	
                return;
            }
            struct net_buf *frag = np->buffer;
			while (frag && !is_last_frag)
			{
//printk("frag %04x len %u\r\n",(uint32_t)frag->data,frag->len);
				if (frag->len == 0) break;
				int rsz = MIN(sizeof(lte_tx_ip_pkt.data) - dsz, frag->len) - ((np->buffer == frag) ? 14 : 0);
				if (dsz + rsz >= tsz)
				{
					rsz = tsz - dsz; 
					is_last_frag = true;
				}
				if (rsz > 0)
				{
					memcpy(lte_tx_ip_pkt.data + dsz, &frag->data[(np->buffer == frag) ? 14 : 0], rsz);
					dsz += rsz;
				}
				frag = frag->frags;	
			}
// for(int i = 0; i < MIN(tsz,sizeof(lte_tx_ip_pkt.data));i++)
// {
//     if (i % 16 == 0) printk("\r\n");
//     printk("%02x ",lte_tx_ip_pkt.data[i]);
// }
// printk("\r\n CMP %d \r\n", memcmp(lte_tx_ip_pkt.data,&data[14],len - 14));

		}

        uint32_t null_ip = 0;
        if (0 != memcmp(my_ip_addr.s4_addr,(uint8_t*)&null_ip,4)
        && (len >= (42))
        && (data[12] == 0x08) && (data[13] == 0x06) 
        && (0 == memcmp(my_eth_mac_addr,&data[0],6))
        )
        {
            operate_packet_arp(data,len);
            net_buf_unref(np->buffer);
            net_pkt_unref(np);
            return;
        }
        else if (0 != memcmp(my_ip_addr.s4_addr,(uint8_t*)&null_ip,4)
        && (len >= (6+6+2+20))
        && (data[12] == 0x08) && (data[13] == 0x00) 
        && (0 == memcmp(my_eth_mac_addr,&data[0],6))
        && ((data[14] >> 4) == 0x04) 
        && (data[23] == IPPROTO_UDP || data[23] == IPPROTO_TCP || data[23] == IPPROTO_ICMP) 
        )
        {

            uint16_t ip_pkt_len   = (uint16_t)(data[16]) << 8 | (uint16_t)(data[17]);
            if (ip_pkt_len > sizeof(lte_tx_ip_pkt.data))
            {
                LOG_ERR("eth eth-rx error ip_pkt_len = %u (> %u)\r\n",ip_pkt_len, sizeof(lte_tx_ip_pkt.data));
                net_buf_unref(np->buffer);
                net_pkt_unref(np);
                return;
            }
            //if (0 != memcmp(server_ip,&data[30],4))
            //{
            //    //LOG_INF("ignore target ip  = %u.%u.%u.%u\r\n",data[30],data[31],data[32],data[33]);
            //    net_buf_unref(pkt->buffer);
            //    net_pkt_unref(pkt);
            //    return;
            //}

            uint16_t ip_hdr_len   = (uint16_t)(data[14] & 0x0F) * 4;
            if (ip_pkt_len <= ip_hdr_len)
            {
                net_buf_unref(np->buffer);
                net_pkt_unref(np);
                return;
            }
            uint16_t ip_dat_len   = ip_pkt_len - ip_hdr_len;
            uint8_t  ip_protocol  = data[23];
            if (0 != check_ics(&data[14],ip_hdr_len))
            {
                LOG_INF("ignore ip_checksum_hdr\r\n");
                net_buf_unref(np->buffer);
                net_pkt_unref(np);
                return;
            }

            lte_tx_ip_pkt.size = ip_pkt_len;
            pmap_row_t* row = NULL;
            switch (lte_tx_ip_pkt.data[9])
            {
            case IPPROTO_ICMP:
                {
                    if (data[14+ip_hdr_len+0] == 0 || data[14+ip_hdr_len+0] == 8) // ECHO ,REPLY
                    {
                        // pass
                    }
                    else
                    {
                        net_buf_unref(np->buffer);
                        net_pkt_unref(np);
                        return;
                    }
                    uint32_t c_ip = *(uint32_t*)&lte_tx_ip_pkt.data[12];
                    uint16_t e_port = (((uint16_t)lte_tx_ip_pkt.data[ip_hdr_len+4]) << 8) + (uint16_t)lte_tx_ip_pkt.data[ip_hdr_len+5];
                    uint16_t l_port = 0;
                    row = search_pmap_by_client_info(IPPROTO_ICMP,c_ip,e_port);
                    if (row == NULL)
                    {
                        uint32_t s_ip = *(uint32_t*)&lte_tx_ip_pkt.data[16];
                        l_port = lte_port_genarator_icmp_echo();
                        row = add_pmap(IPPROTO_ICMP,c_ip,e_port,s_ip,l_port); 
                    }
                    if (row != NULL)
                    {
                        if (l_port == 0) l_port = row->lte_port;
                        lte_tx_ip_pkt.data[ip_hdr_len+4] = (uint8_t)((l_port >> 8) & 0x00FFu);
                        lte_tx_ip_pkt.data[ip_hdr_len+5] = (uint8_t)(l_port & 0x00FFu);
                    }
                }
                break;
            case IPPROTO_UDP:
                {
                    uint32_t c_ip   = *(uint32_t*)&lte_tx_ip_pkt.data[12];
                    uint16_t e_port = (((uint16_t)lte_tx_ip_pkt.data[ip_hdr_len+0]) << 8) + (uint16_t)lte_tx_ip_pkt.data[ip_hdr_len+1];
                    uint16_t l_port = 0;
                    row = search_pmap_by_client_info(IPPROTO_UDP,c_ip,e_port);
                    if (row == NULL)
                    {
                        uint32_t s_ip = *(uint32_t*)&lte_tx_ip_pkt.data[16];
                        l_port = lte_port_genarator();
                        row = add_pmap(IPPROTO_UDP,c_ip,e_port,s_ip,l_port); 
                    }
                    if (row != NULL)
                    {
                        if (l_port == 0) l_port = row->lte_port;
                        lte_tx_ip_pkt.data[ip_hdr_len+0] = (uint8_t)((l_port >> 8) & 0x00FFu);
                        lte_tx_ip_pkt.data[ip_hdr_len+1] = (uint8_t)(l_port & 0x00FFu);
                    }
                }
                break;
            case IPPROTO_TCP:
                {
                    uint32_t c_ip   = *(uint32_t*)&lte_tx_ip_pkt.data[12];
                    uint16_t e_port = (((uint16_t)lte_tx_ip_pkt.data[ip_hdr_len+0]) << 8) + (uint16_t)lte_tx_ip_pkt.data[ip_hdr_len+1];
                    uint16_t l_port = 0;
                    row = search_pmap_by_client_info(IPPROTO_TCP,c_ip,e_port);
                    if (row == NULL)
                    {
                        uint32_t s_ip = *(uint32_t*)&lte_tx_ip_pkt.data[16];
                        l_port = lte_port_genarator();
                        row = add_pmap(IPPROTO_TCP,c_ip,e_port,s_ip,l_port); 
                    }
                    if (row != NULL)
                    {
                        if (l_port == 0) l_port = row->lte_port;
                        lte_tx_ip_pkt.data[ip_hdr_len+0] = (uint8_t)((l_port >> 8) & 0x00FFu);
                        lte_tx_ip_pkt.data[ip_hdr_len+1] = (uint8_t)(l_port & 0x00FFu);
                    }
                }
                break;
            }
            if (row == NULL)
            {
                LOG_ERR("eth rx error portmap overflow\r\n");
                net_buf_unref(np->buffer);
                net_pkt_unref(np);
                return;                
            }
//printk("!!!!!! port %u <-> %u\r\n", row->eth_port,row->lte_port);
            update_arp_cache_by_ip(row->client_ip, &data[6]);
            
            if (data[23] == IPPROTO_UDP)
            {
                // static pseudo_ip_hdr_t ip_hdr;
                // memset(&ip_hdr,0,sizeof(ip_hdr));
                // memcpy(ip_hdr.ip_src,&lte_tx_ip_pkt.data[12],4);
                // memcpy(ip_hdr.ip_dst,&lte_tx_ip_pkt.data[16],4);
                // ip_hdr.ip_p   = ip_protocol;
                // ip_hdr.ip_len = ip_htons(ip_dat_len);
                // uint8_t * p_data = &lte_tx_ip_pkt.data[ip_hdr_len];
                // if (0 != ip_checksum2((uint8_t *)&ip_hdr, sizeof(pseudo_ip_hdr_t), p_data, ip_dat_len))
                // {
                //     LOG_INF("ignore UDP invalid checksum\r\n");
                //     net_buf_unref(np->buffer);
                //     net_pkt_unref(np);
                //     return;
                // }
            }
            else if (data[23] == IPPROTO_TCP)
            {
                // static pseudo_ip_hdr_t ip_hdr;
                // memset(&ip_hdr,0,sizeof(ip_hdr));
                // memcpy(ip_hdr.ip_src,&lte_tx_ip_pkt.data[12],4);
                // memcpy(ip_hdr.ip_dst,&lte_tx_ip_pkt.data[16],4);
                // ip_hdr.ip_p   = ip_protocol;
                // ip_hdr.ip_len = ip_htons(ip_dat_len);
                // uint8_t * p_data = &lte_tx_ip_pkt.data[ip_hdr_len];
                // if (0 != ip_checksum2((uint8_t *)&ip_hdr, sizeof(pseudo_ip_hdr_t), p_data, ip_dat_len))
                // {
                //     LOG_INF("ignore TCP invalid checksum\r\n");
                //     net_buf_unref(np->buffer);
                //     net_pkt_unref(np);
                //     return;
                // }
            }
            else // ICMP
            {
                // if (0 != check_ics(&lte_tx_ip_pkt.data[ip_hdr_len], ip_dat_len))
                // {
                //     LOG_INF("ignore ICMP invalid checksum, len = %u,%u\r\n", ip_hdr_len, ip_dat_len);
                //     net_buf_unref(np->buffer);
                //     net_pkt_unref(np);
                //     return;
                // }
            }
            memcpy(&lte_tx_ip_pkt.data[12],&my_ip_addr,sizeof(my_ip_addr));
            lte_tx_ip_pkt.data[10] = 0; lte_tx_ip_pkt.data[11] =0;
            calc_ics(lte_tx_ip_pkt.data,ip_hdr_len,10);
            bool is_udp = lte_tx_ip_pkt.data[9] == IPPROTO_UDP;
            bool is_tcp = lte_tx_ip_pkt.data[9] == IPPROTO_TCP;
            if (is_tcp || is_udp)
            {
                lte_tx_ip_pkt.data[ip_hdr_len + (is_udp ? 6 : 16)] = 0;
                lte_tx_ip_pkt.data[ip_hdr_len + (is_udp ? 7 : 17)] = 0;
                static pseudo_ip_hdr_t ip_hdr;
                memset(&ip_hdr,0,sizeof(ip_hdr));
                memcpy(ip_hdr.ip_src,&lte_tx_ip_pkt.data[12],4);
                memcpy(ip_hdr.ip_dst,&lte_tx_ip_pkt.data[16],4);
                ip_hdr.ip_p   = ip_protocol;
                ip_hdr.ip_len = ip_htons(ip_dat_len);
                uint8_t * p_data = &lte_tx_ip_pkt.data[ip_hdr_len];
                uint16_t cs = ip_checksum2((uint8_t *)&ip_hdr, sizeof(pseudo_ip_hdr_t), p_data, ip_dat_len);
                lte_tx_ip_pkt.data[ip_hdr_len + (is_udp ? 7 : 17)] = (uint8_t)((cs >> 8) & 0xFF);
                lte_tx_ip_pkt.data[ip_hdr_len + (is_udp ? 6 : 16)] = (uint8_t)(cs & 0xFF);
            }
            else
            {
                calc_ics(&lte_tx_ip_pkt.data[ip_hdr_len],ip_pkt_len - ip_hdr_len,2);
            }

            errno = 0;
            while (lock) {k_sleep(K_USEC(1));};
            lock = true;
            ssize_t ssz = send(lte_sock, lte_tx_ip_pkt.data, lte_tx_ip_pkt.size, 0);
            if (ssz <= 0) {
                LOG_ERR("send failed: (%d)", -errno);
            }
            lock = false;
        }
        net_buf_unref(np->buffer);
        net_pkt_unref(np);
    }
    
}

////////////////////////////////////////////////////////////////////////////////
static void eth_rx_thread_func(void *p1, void *p2, void *p3)
{
	ARG_UNUSED(p1);
	ARG_UNUSED(p2);
	ARG_UNUSED(p3);

    while (!on_req_eth_thread_terminate)
    {
        if  (!is_cgev_act_on || is_cgev_detach_on)
        {
            k_sleep(K_MSEC(3000));
            continue;
        }
        net_context_recv(ctx_rx,eth_recv_cb,K_MSEC(100),NULL);
        k_sleep(K_MSEC(1));
    }
    on_eth_rx_thread_terminate = true;
    LOG_INF("ETH RX thread terminated");
}
////////////////////////////////////////////////////////////////////////////////
static uint8_t lte_rx_ip_raw_packet[NRF9160_MTU];
typedef struct  L1_PAYLOAD
{
    uint16_t size;
    uint8_t  data[NRF9160_MTU+14];
} L1_PAYLOAD_t;
static L1_PAYLOAD_t eth_tx_raw_pkt;
static void eth_tx_thread_func(void *p1, void *p2, void *p3) // LTE_RX
{
	ARG_UNUSED(p1);
	ARG_UNUSED(p2);
	ARG_UNUSED(p3);

    static char    sbuf[256];
    static struct in_addr my_addr; 
    static struct pollfd fds[1];
    int err = 0;
    while (!on_req_eth_thread_terminate)
    {
        if  (!is_cgev_act_on || is_cgev_detach_on)
        {
            k_sleep(K_MSEC(3000));
            continue;
        }
        err = modem_info_string_get(MODEM_INFO_IP_ADDRESS, sbuf, sizeof(sbuf));
        if (err <= 0)
        {
            LOG_INF("can not get MODEM_INFO_IP_ADDRESS err = %d", err);
            k_sleep(K_MSEC(3000));
            continue;
        }
        LOG_DBG("IP address of the device: %s", sbuf);
        err = net_addr_pton(AF_INET,sbuf,&my_addr);
        if (err)
        {
            LOG_ERR("can not transform ip address err = %d",err);
            k_sleep(K_MSEC(3000));
            continue;
        }

        lte_sock = socket(AF_PACKET, SOCK_RAW, 0);
        if (lte_sock < 0)
        {
            LOG_ERR("socket(AF_PACKET,SOCK_RAW,0) failed: (%d)", -errno);
            continue;
        }
        while (!on_req_eth_thread_terminate)
        {
            if  (!is_cgev_act_on || is_cgev_detach_on)
            {
                break;
            }
            err = modem_info_string_get(MODEM_INFO_IP_ADDRESS, sbuf, sizeof(sbuf));
            if (err <= 0)
            {
                LOG_DBG("can not get MODEM_INFO_IP_ADDRESS err = %d", err);
                break;;
            }
            errno = 0;
            fds[0].fd = lte_sock;
            fds[0].events = POLLIN;
            while (lock) {k_sleep(K_USEC(1));};
            lock = true;
            int ret = poll(fds, 1, 100);
            if (ret == 0)
            {
                //close(fd);
                lock = false;
                //k_sleep(K_MSEC(1));
                continue;
            }
            else if (ret < 0)
            {
                //close(fd);
                lock = false;
                LOG_INF("poll() failed: (%d) (%d)", -errno, ret);
                //k_sleep(K_MSEC(1));
                continue;
            }
            if (ret > sizeof(lte_rx_ip_raw_packet))
            {
                LOG_INF("poll() : recv size %d > %u", ret, sizeof(lte_rx_ip_raw_packet));
                //k_sleep(K_MSEC(1));
                continue;
            }
            ssize_t raw_len = recv(lte_sock, lte_rx_ip_raw_packet, sizeof(lte_rx_ip_raw_packet), 0);
            if (raw_len <= 0)
            {
                if (errno != EAGAIN && errno != EWOULDBLOCK)
                {
                    lock = false;
                    //close(fd);
                    LOG_ERR( "recv() failed with errno (%d) and return value (%d)", -errno, raw_len);
                }
                k_sleep(K_MSEC(1));
                continue;
            }
            lock = false;
//printk("lte recv -------\r\n");
//for (int i = 0; i < raw_len; i++)
//{
//    if (0 == i % 16) printk("\r\n");
//    printk("%02x",lte_rx_ip_raw_packet[i]);
//}
//printk("\r\n");
            if (raw_len < 20) {
                LOG_WRN("recv() wrong data (%d)", raw_len);
                k_sleep(K_MSEC(10));
                continue;
            }

            if (((lte_rx_ip_raw_packet[0] >> 4) == 0x04) 
            &&  (lte_rx_ip_raw_packet[9] == IPPROTO_UDP || lte_rx_ip_raw_packet[9] == IPPROTO_TCP || lte_rx_ip_raw_packet[9] == IPPROTO_ICMP) 
            )
            {
                uint16_t ip_pkt_len   = (uint16_t)(lte_rx_ip_raw_packet[2]) << 8 | (uint16_t)(lte_rx_ip_raw_packet[3]);
                if (ip_pkt_len > NRF9160_MTU)
                {
                    LOG_ERR("error lte-recv ip_pkt_len = %u (> %u)\r\n",ip_pkt_len, NRF9160_MTU);
                    continue;
                }
                if (0 != memcmp(&my_addr,&lte_rx_ip_raw_packet[16],4))
                {
                    continue;
                }

                uint16_t ip_hdr_len   = (uint16_t)(lte_rx_ip_raw_packet[0] & 0x0F) * 4;
                if (ip_pkt_len <= ip_hdr_len)
                {
                    continue;
                }
                uint16_t ip_dat_len   = ip_pkt_len - ip_hdr_len;
                uint8_t  ip_protocol  = lte_rx_ip_raw_packet[9];
                if (0 != check_ics(&lte_rx_ip_raw_packet[0],ip_hdr_len))
                {
                    LOG_ERR("error ip_checksum_hdr\r\n");
                    continue;
                }
                if (ip_dat_len > 0 && ip_dat_len <= NRF9160_MTU)
                {
                    if (lte_rx_ip_raw_packet[9] == IPPROTO_UDP)
                    {
                        static pseudo_ip_hdr_t ip_hdr;
                        memset(&ip_hdr,0,sizeof(ip_hdr));
                        memcpy(ip_hdr.ip_src,&lte_rx_ip_raw_packet[12],4);
                        memcpy(ip_hdr.ip_dst,&lte_rx_ip_raw_packet[16],4);
                        ip_hdr.ip_p   = ip_protocol;
                        ip_hdr.ip_len = ip_htons(ip_dat_len);
                        uint8_t * p_data = &lte_rx_ip_raw_packet[ip_hdr_len];
                        if (0 != ip_checksum2((uint8_t *)&ip_hdr, sizeof(pseudo_ip_hdr_t), p_data, ip_dat_len))
                        {
                            LOG_ERR("error UDP invalid checksum\r\n");
                            continue; 
                        }
                    }
                    else if (lte_rx_ip_raw_packet[9] == IPPROTO_TCP)
                    {
                        //static pseudo_ip_hdr_t ip_hdr;
                        //memset(&ip_hdr,0,sizeof(ip_hdr));
                        //memcpy(ip_hdr.ip_src,&lte_rx_ip_raw_packet[12],4);
                        //memcpy(ip_hdr.ip_dst,&lte_rx_ip_raw_packet[16],4);
                        //ip_hdr.ip_p   = ip_protocol;
                        //ip_hdr.ip_len = ip_htons(ip_dat_len);
                        //uint8_t * p_data = &lte_rx_ip_raw_packet[ip_hdr_len];
                        //if (0 != ip_checksum2((uint8_t *)&ip_hdr, sizeof(pseudo_ip_hdr_t), p_data, ip_dat_len))
                        //{
                        //    LOG_ERR("error TCP invalid checksum\r\n");
                        //    continue; 
                        //}
                    }
                    else // ICMP
                    {
//printk("**** %d\r\n",lte_rx_ip_raw_packet[ip_hdr_len+0]);
                        if (lte_rx_ip_raw_packet[ip_hdr_len+0] == 0 || lte_rx_ip_raw_packet[ip_hdr_len+0] == 8) // ECHO,REPLY
                        {
                            // pass
                        }
                        else
                        {
                            continue;
                        }
                        uint8_t * p_data = &lte_rx_ip_raw_packet[ip_hdr_len];
                        if (0 != check_ics(p_data, ip_dat_len))
                        {
                            LOG_ERR("error ICMP invalid checksum\r\n");
                            continue; 
                        }
                    }
                }
                eth_tx_raw_pkt.size = ip_pkt_len + 14;
                memcpy(&eth_tx_raw_pkt.data[14],&(lte_rx_ip_raw_packet[0]),ip_pkt_len);
                pmap_row_t* prow = NULL;
                static pmap_row_t row;
                switch (eth_tx_raw_pkt.data[14+9])
                {
                case IPPROTO_ICMP:
                    {
                        uint32_t ip   = *(uint32_t*)&eth_tx_raw_pkt.data[14+12];
                        uint16_t port = (((uint16_t)eth_tx_raw_pkt.data[14+ip_hdr_len+4]) << 8) + (uint16_t)eth_tx_raw_pkt.data[14+ip_hdr_len+5];
                        prow = search_pmap_by_server_info(IPPROTO_ICMP,ip,port);
                        if (prow == NULL)
                        {
printk("ICMP invalid port %u\r\n", port);
                            continue;
                        }
                        memcpy(&row,prow,sizeof(row));
                        eth_tx_raw_pkt.data[14+ip_hdr_len+4] = (uint8_t)(row.eth_port >> 8);
                        eth_tx_raw_pkt.data[14+ip_hdr_len+5] = (uint8_t)(row.eth_port & 0x00FFu);
                        memcpy(&eth_tx_raw_pkt.data[14+16],(uint8_t*)&row.client_ip,4);
                    }
                    break;
                case IPPROTO_UDP:
                    {
                        uint32_t ip   = *(uint32_t*)&eth_tx_raw_pkt.data[14+12];
                        uint16_t port = (((uint16_t)eth_tx_raw_pkt.data[14+ip_hdr_len+2]) << 8) + (uint16_t)eth_tx_raw_pkt.data[14+ip_hdr_len+3];
                        prow = search_pmap_by_server_info(IPPROTO_UDP,ip,port);
                        if (prow == NULL)
                        {
printk("UDP invalid port %u\r\n",port);
                            continue;
                        }
                        memcpy(&row,prow,sizeof(row));
                        eth_tx_raw_pkt.data[14+ip_hdr_len+2] = (uint8_t)(row.eth_port >> 8);
                        eth_tx_raw_pkt.data[14+ip_hdr_len+3] = (uint8_t)(row.eth_port & 0x00FFu);
                        memcpy(&eth_tx_raw_pkt.data[14+16],(uint8_t*)&row.client_ip,4);
                    }
                    break;
                case IPPROTO_TCP:
                    {
                        uint32_t ip   = *(uint32_t*)&eth_tx_raw_pkt.data[14+12];
                        uint16_t port = (((uint16_t)eth_tx_raw_pkt.data[14+ip_hdr_len+2]) << 8) + (uint16_t)eth_tx_raw_pkt.data[14+ip_hdr_len+3];
                        prow = search_pmap_by_server_info(IPPROTO_TCP,ip,port);
                        if (prow == NULL)
                        {
printk("TCP invalid port %u\r\n",port);
                            continue;
                        }
                        memcpy(&row,prow,sizeof(row));
                        eth_tx_raw_pkt.data[14+ip_hdr_len+2] = (uint8_t)(row.eth_port >> 8);
                        eth_tx_raw_pkt.data[14+ip_hdr_len+3] = (uint8_t)(row.eth_port & 0x00FFu);
                        memcpy(&eth_tx_raw_pkt.data[14+16],(uint8_t*)&row.client_ip,4);
                    }
                    break;
                }
                eth_tx_raw_pkt.data[14+10] = 0; eth_tx_raw_pkt.data[14+11] =0;
                calc_ics(&eth_tx_raw_pkt.data[14],ip_hdr_len,10);
                bool is_udp = lte_rx_ip_raw_packet[9] == IPPROTO_UDP;
                bool is_tcp = lte_rx_ip_raw_packet[9] == IPPROTO_TCP;
                if (is_tcp || is_udp)
                {
                    eth_tx_raw_pkt.data[14 + ip_hdr_len + (is_udp ? 6 : 16)] = 0;
                    eth_tx_raw_pkt.data[14 + ip_hdr_len + (is_udp ? 7 : 17)] = 0;
                    static pseudo_ip_hdr_t ip_hdr;
                    memset(&ip_hdr,0,sizeof(ip_hdr));
                    memcpy(ip_hdr.ip_src,&eth_tx_raw_pkt.data[14 + 12],4);
                    memcpy(ip_hdr.ip_dst,&eth_tx_raw_pkt.data[14 + 16],4);
                    ip_hdr.ip_p   = ip_protocol;
                    ip_hdr.ip_len = ip_htons(ip_dat_len);
                    uint8_t * p_data = &eth_tx_raw_pkt.data[14 + ip_hdr_len];
                    uint16_t cs = ip_checksum2((uint8_t *)&ip_hdr, sizeof(pseudo_ip_hdr_t), p_data, ip_dat_len);
                    eth_tx_raw_pkt.data[14 + ip_hdr_len + (is_udp ? 7 : 17)] = (uint8_t)((cs >> 8) & 0xFF);
                    eth_tx_raw_pkt.data[14 + ip_hdr_len + (is_udp ? 6 : 16)] = (uint8_t)(cs & 0xFF);
                    // if (0 != ip_checksum2((uint8_t *)&ip_hdr, sizeof(pseudo_ip_hdr_t), p_data, ip_dat_len))
                    // {
                    //     LOG_ERR("error %s packet-data checksum %d %d\r\n",(is_udp ? "UDP" : "TCP"),ip_opt_len,ip_dat_len);
                    //     continue; 
                    // }
                }
                else
                {
                    calc_ics(&eth_tx_raw_pkt.data[14 + ip_hdr_len],ip_pkt_len - ip_hdr_len,2);
                }
                // if (0 != check_ics(&pkt.data[14],20 + ip_opt_len))
                // {
                //     LOG_ERR("error ip_checksum_hdr (new)\r\n");
                //     continue;
                // }

                if (!on_eth_rx_thread_terminate)
                {
                    uint8_t mac[6];
                    if (!resolve_arp(row.client_ip,mac,(1000),true))
                    {
                        LOG_WRN("ETH_TX ARP resolve error");
                    }
                    else
                    {
                        memcpy(&eth_tx_raw_pkt.data[0],mac,6);
                        memcpy(&eth_tx_raw_pkt.data[6],my_eth_mac_addr,6);
                        eth_tx_raw_pkt.data[12] = 0x08;
                        eth_tx_raw_pkt.data[13] = 0x00;
                        struct sockaddr_ll tgt_addr;
                        memset(&tgt_addr,0,sizeof(tgt_addr));
                        memcpy(tgt_addr.sll_addr,mac,6);
                        tgt_addr.sll_family = AF_PACKET;
                        tgt_addr.sll_protocol = 0x0800;
                        tgt_addr.sll_hatype = 1;
                        tgt_addr.sll_ifindex = 0;
                        tgt_addr.sll_halen = 6;
                        tgt_addr.sll_pkttype = PACKET_HOST;
                        int r = net_context_sendto(ctx_tx, &eth_tx_raw_pkt.data[0], eth_tx_raw_pkt.size,(struct sockaddr *)&tgt_addr,sizeof(tgt_addr),eth_send_cb,K_MSEC(1000),NULL);
                        if (r <= 0)
                        {
                            LOG_ERR("ETH_TX error send (%d)",r);
                        }
                    }
//printk("eth sendto -------\r\n");
//for (int i = 0; i < pkt.size - 14; i++)
//{
//    if (0 == i % 16) printk("\r\n");
//    printk("%02x",pkt.data[14 + i]);
//}
//printk("\r\n");
                }
            }
            //k_sleep(K_MSEC(100));
        }
        close(lte_sock);
        k_sleep(K_MSEC(100));
    }
    on_eth_tx_thread_terminate = true;
    LOG_INF("ETH TX thread terminated");
}

int eth_start(void)
{
    int err = 0;
    on_req_eth_thread_terminate = false;
    init_pmap();

    struct net_if *iface = net_if_get_default();
    eth_iface = net_if_get_first_by_type(&NET_L2_GET_NAME(ETHERNET));
    if (iface != eth_iface)
    {
        printk("%04x %04x\r\n",(uint32_t)iface,(uint32_t)eth_iface);
        net_if_set_default(eth_iface);

    }
    if (eth_iface->if_dev->link_addr.len != 6)
    {
        printk("error eth mac length =  %d\r\n",eth_iface->if_dev->link_addr.len);
        return -1;
    }
    memcpy(my_eth_mac_addr,eth_iface->if_dev->link_addr.addr,eth_iface->if_dev->link_addr.len);
    memcpy(my_eth_ip_addr,eth_iface->config.ip.ipv4->unicast->address.in_addr.s4_addr,4);
    printk("ETH IP: %u.%u.%u.%u, MAC: %02x:%02x:%02x:%02x:%02x:%02x\r\n",
        my_eth_ip_addr[0],
        my_eth_ip_addr[1],
        my_eth_ip_addr[2],
        my_eth_ip_addr[3],
        my_eth_mac_addr[0],
        my_eth_mac_addr[1],
        my_eth_mac_addr[2],
        my_eth_mac_addr[3],
        my_eth_mac_addr[4],
        my_eth_mac_addr[5]
        );
    net_if_set_mtu(eth_iface,NRF9160_MTU);
    printk("eth iface = %04x\r\n",(uint32_t)eth_iface);
    err = net_context_get(AF_PACKET,SOCK_RAW,IPPROTO_RAW,&ctx_tx);
    if (0 > err)
    {
        printk("error net_context_get (tx)%d\r\n",err);
        return err;
    }

    err = net_context_get(AF_PACKET,SOCK_RAW,IPPROTO_IP,&ctx_rx);
    if (0 > err)
    {
        net_context_put(ctx_tx);
        printk("error net_context_get (rx)%d\r\n",err);
        return err;

    }
    net_context_set_iface(ctx_tx,eth_iface);
    net_context_set_iface(ctx_rx,eth_iface);
    // while (true)
    // {
    //     net_context_recv(ctx,eth_recv_cb,K_MSEC(100),NULL);
    //     k_sleep(K_MSEC(1));
    // }
	eth_rx_thread_id = k_thread_create(&eth_rx_thread, eth_rx_thread_stack,
			K_THREAD_STACK_SIZEOF(eth_rx_thread_stack),
			eth_rx_thread_func, NULL, NULL, NULL,
			THREAD_PRIORITY, K_USER, K_NO_WAIT);
	eth_tx_thread_id = k_thread_create(&eth_tx_thread, eth_tx_thread_stack,
			K_THREAD_STACK_SIZEOF(eth_tx_thread_stack),
			eth_tx_thread_func, NULL, NULL, NULL,
			THREAD_PRIORITY, K_USER, K_NO_WAIT);
    return 0;
}