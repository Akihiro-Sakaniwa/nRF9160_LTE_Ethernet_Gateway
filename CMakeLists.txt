# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

set(PM_STATIC_YML_FILE ${CMAKE_CURRENT_SOURCE_DIR}/pm_static.yml)
#set(mcuboot_key_file ${CMAKE_CURRENT_SOURCE_DIR}/mcuboot.pem)
#set(mcuboot_CONFIG_BOOT_SIGNATURE_KEY_FILE "\"${mcuboot_key_file}\"")

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(nRF9160_LTE_Ethernet_Gateway)

target_sources(app PRIVATE src/main.c)

target_sources(app PRIVATE src/nrf91/nrf91_lte.c)
target_sources(app PRIVATE src/nrf91/nrf91_at_commands.c)
target_sources(app PRIVATE src/nrf91/nrf91_util.c)

target_sources(app PRIVATE src/gateway.c)
target_sources(app PRIVATE src/arp_cache.c)
target_sources(app PRIVATE src/pmap.c)
