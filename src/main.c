/*
 * Copyright (c) 2024 Akihiro Sakaniwa
 *
 * SPDX-License-Identifier: MIT License
 */
#include <zephyr/logging/log.h>
#include <zephyr/logging/log_ctrl.h>
#include <zephyr/net/net_pkt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/drivers/gpio.h>

#include <modem/at_monitor.h>
#include <nrf_modem.h>
#include <nrf_errno.h>
#include <hal/nrf_gpio.h>
#include <hal/nrf_power.h>
#include <hal/nrf_regulators.h>
#include <modem/nrf_modem_lib.h>
#include <zephyr/sys/reboot.h>
#include <zephyr/init.h>
#include <zephyr/sys/reboot.h>
#include "ncs_version.h"

#include <nrf_socket.h>
#include <zephyr/net/socket.h>

#include "nrf91/nrf91_lte.h"

#include <modem/at_monitor.h>
#include <modem/modem_info.h>

#include "nrf91/nrf91_util.h"

#include "pmap.h"
#include "main.h"

LOG_MODULE_REGISTER(main, CONFIG_NRF91_LOG_LEVEL);

volatile bool on_req_eth_thread_terminate = false;
volatile bool on_eth_rx_thread_terminate = false;
volatile bool on_eth_tx_thread_terminate = false;
extern int eth_start(void);

extern bool is_cgev_act_on;
extern bool is_cgev_detach_on;


#ifdef CONFIG_BOARD_ACTINIUS_ICARUS_SOM_DK_NS
//const uint8_t led_pin = 3;
//const uint8_t btn_pin = 23;
#endif //CONFIG_BOARD_ACTINIUS_ICARUS_SOM_DK_NS
#ifdef CONFIG_BOARD_SPARKFUN_THING_PLUS_NRF9160_NS
const uint8_t status_led_pin = 3;
const uint8_t btn_pin = 12;
#endif //CONFIG_BOARD_SPARKFUN_THING_PLUS_NRF9160_NS
#ifdef CONFIG_BOARD_CIRCUITDOJO_FEATHER_NRF9160_NS
const uint8_t status_led_pin = 3;
const uint8_t btn_pin = 12;
#endif
#ifndef CONFIG_BOARD_ACTINIUS_ICARUS_SOM_DK_NS
#ifndef CONFIG_BOARD_SPARKFUN_THING_PLUS_NRF9160_NS
#ifndef CONFIG_BOARD_CIRCUITDOJO_FEATHER_NRF9160_NS
const uint8_t status_led_pin = 20; // SSCI IoT Board
const uint8_t btn_pin = 6; // SSICC IoT Board
#endif
#endif
#endif


const struct device *gpio_dev;
//const struct device *flash_dev = DEVICE_DT_GET_ONE(jedec_spi_nor);

extern void nrf91_lte_main(int (*cbf)(void));
extern bool nrf91_lte_thread_start(void);

volatile bool is_on_buttun_up = false;
volatile bool is_on_buttun_up_mask = false;

//////////////////////////////////////////////////////////////////////////////////////////////////////
void button_callback_handler(const struct device *port, struct gpio_callback *cb, gpio_port_pins_t pins)
{
  printk("\r\n(%08x)!\r\n",pins);
  if ((BIT(btn_pin) & pins) > 0) 
  {
      if (!is_on_buttun_up_mask) is_on_buttun_up = true;
  }
}
static struct gpio_callback button_callback;
////////////////////////////////////////////////////////////////////////////////////////////////////
void error_handler(void)
{
    LOG_WRN("\r\n### POWER-OFF ###\r\n");
    while (1)
    {
        k_sleep(K_MSEC(100));
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////
void status_led_on(void)
{
    gpio_pin_set(gpio_dev,status_led_pin,1);
}
void status_led_off(void)
{
    gpio_pin_set(gpio_dev,status_led_pin,0);
}
int read_btn(void)
{
  return gpio_pin_get(gpio_dev, btn_pin);
}
////////////////////////////////////////////////////////////////////////////////////////////////////
static int cbf_main(void)
{
    LOG_INF("PORT-MAP = %3u[%%]",pmap_status());
    k_sleep(K_MSEC(10*1000));
    return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
int main(void)
{
    printk("%s\r\n", CONFIG_BOARD);

#ifdef CONFIG_SERIAL
    NRF_UARTE0_NS->TASKS_STOPRX = 1;
#endif

    gpio_dev = DEVICE_DT_GET(DT_NODELABEL(gpio0)); 
    #ifdef CONFIG_BOARD_SPARKFUN_THING_PLUS_NRF9160_NS
    gpio_pin_configure(gpio_dev, status_led_pin , GPIO_OUTPUT | GPIO_PUSH_PULL | GPIO_ACTIVE_LOW);
    gpio_pin_configure(gpio_dev, btn_pin , GPIO_INPUT  | GPIO_PULL_UP   | GPIO_ACTIVE_HIGH);
    //gpio_pin_configure(gpio_dev, buz_pin , GPIO_OUTPUT | GPIO_PUSH_PULL | GPIO_ACTIVE_HIGH);
    #else
    #ifdef CONFIG_BOARD_CIRCUITDOJO_FEATHER_NRF9160_NS
    gpio_pin_configure(gpio_dev, status_led_pin , GPIO_OUTPUT | GPIO_PUSH_PULL | GPIO_ACTIVE_LOW);
    gpio_pin_configure(gpio_dev, btn_pin , GPIO_INPUT  | GPIO_PULL_UP   | GPIO_ACTIVE_HIGH);
    #else
    gpio_pin_configure(gpio_dev, status_led_pin , GPIO_OUTPUT | GPIO_PUSH_PULL | GPIO_ACTIVE_HIGH);
    gpio_pin_configure(gpio_dev, btn_pin , GPIO_INPUT  | GPIO_PULL_UP   | GPIO_ACTIVE_HIGH);
    #endif
    #endif
    status_led_off();

    gpio_init_callback(&button_callback, button_callback_handler, BIT(btn_pin));
    gpio_add_callback(gpio_dev, &button_callback);
    gpio_pin_interrupt_configure(gpio_dev, btn_pin, GPIO_INT_EDGE_RISING);

    int ret = nrf_modem_lib_init();
    if (ret)
    {
        LOG_ERR("Modem library init failed, err: %d", ret);
        if (ret != -EAGAIN && ret != -EIO)
        {
            return ret;
        }
        else if (ret == -EIO)
        {
            LOG_ERR("Please program full modem firmware with the bootloader or external tools");
        }
    }
    ret = modem_info_init();
    if (ret)
    {
        LOG_ERR("Failed to init modem_info_init: %d", ret);
        return(0);
    }

    if (0 != eth_start()) return 0;
    nrf91_lte_main(cbf_main);

    return 0;
}
// EOF //