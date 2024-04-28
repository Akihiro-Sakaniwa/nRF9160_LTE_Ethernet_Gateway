# nRF9160_LTE_Ethernet_Gateway
LTE Ethernet Gateway sample program for nRF9160 (W5500,enc28j60,enc424j600,enc624j600)


# Overview
This source program is LTE-Ethernet-Gateway for nRF9160.
You will be able to communicate with the server via LTE from the communication device connected to the Ethernet side.
Since port mapping is performed, multiple communication devices can be connected to the Ethernet side.

I hope this is of some use to you.

FOTA related functions have been omitted because the code would be complicated.

When using,
prj.conf
overlay.dts
Don't forget to change.

Set the MTU of the LAN side device to 1280.

The GW IP address is written in prj.conf.

There are no DHCP related functions.

Since the port mapping function is implemented, multiple units can be connected on the LAN side.

The MAC address can be specified in overlay.dts.

Specifying LAN-CHIP requires changing both prj.conf and overlay.dts.


# Target SDK

NCS v2.5.2


# Target board

スイッチサイエンス nRF9160搭載 LTE-M/NB-IoT/GNSS対応 IoT開発ボード

https://ssci.to/8999

Actinius Icarus SoM DK

https://www.actinius.com/icarus-som-dk


# Target SPI LAN CHIP

WIZNET W5500

MICROCHIP ENC28J60

MICROCHIP ENC424J600 (ENC624J600)


# Target SIM

SORACOM PLAN-D

https://soracom.jp/store/13380/


# Board Pin Assign

see  xxxxxxxx_overlay.dts file


# Disclaimer
This source program is an implementation example and does not provide any guarantees.
Please use at your own risk.

# License
LicenseRef-Nordic-5-Clause

MIT License
