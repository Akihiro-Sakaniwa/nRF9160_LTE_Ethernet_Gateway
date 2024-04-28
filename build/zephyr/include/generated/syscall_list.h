/* auto-generated by gen_syscalls.py, don't edit */

#ifndef ZEPHYR_SYSCALL_LIST_H
#define ZEPHYR_SYSCALL_LIST_H

#define K_SYSCALL_CLOCK_GETTIME 0
#define K_SYSCALL_DEVICE_GET_BINDING 1
#define K_SYSCALL_DEVICE_IS_READY 2
#define K_SYSCALL_ENTROPY_GET_ENTROPY 3
#define K_SYSCALL_GPIO_GET_PENDING_INT 4
#define K_SYSCALL_GPIO_PIN_CONFIGURE 5
#define K_SYSCALL_GPIO_PIN_GET_CONFIG 6
#define K_SYSCALL_GPIO_PIN_INTERRUPT_CONFIGURE 7
#define K_SYSCALL_GPIO_PORT_CLEAR_BITS_RAW 8
#define K_SYSCALL_GPIO_PORT_GET_DIRECTION 9
#define K_SYSCALL_GPIO_PORT_GET_RAW 10
#define K_SYSCALL_GPIO_PORT_SET_BITS_RAW 11
#define K_SYSCALL_GPIO_PORT_SET_MASKED_RAW 12
#define K_SYSCALL_GPIO_PORT_TOGGLE_BITS 13
#define K_SYSCALL_K_BUSY_WAIT 14
#define K_SYSCALL_K_CONDVAR_BROADCAST 15
#define K_SYSCALL_K_CONDVAR_INIT 16
#define K_SYSCALL_K_CONDVAR_SIGNAL 17
#define K_SYSCALL_K_CONDVAR_WAIT 18
#define K_SYSCALL_K_EVENT_CLEAR 19
#define K_SYSCALL_K_EVENT_INIT 20
#define K_SYSCALL_K_EVENT_POST 21
#define K_SYSCALL_K_EVENT_SET 22
#define K_SYSCALL_K_EVENT_SET_MASKED 23
#define K_SYSCALL_K_EVENT_WAIT 24
#define K_SYSCALL_K_EVENT_WAIT_ALL 25
#define K_SYSCALL_K_FLOAT_DISABLE 26
#define K_SYSCALL_K_FLOAT_ENABLE 27
#define K_SYSCALL_K_FUTEX_WAIT 28
#define K_SYSCALL_K_FUTEX_WAKE 29
#define K_SYSCALL_K_IS_PREEMPT_THREAD 30
#define K_SYSCALL_K_MSGQ_ALLOC_INIT 31
#define K_SYSCALL_K_MSGQ_GET 32
#define K_SYSCALL_K_MSGQ_GET_ATTRS 33
#define K_SYSCALL_K_MSGQ_NUM_FREE_GET 34
#define K_SYSCALL_K_MSGQ_NUM_USED_GET 35
#define K_SYSCALL_K_MSGQ_PEEK 36
#define K_SYSCALL_K_MSGQ_PEEK_AT 37
#define K_SYSCALL_K_MSGQ_PURGE 38
#define K_SYSCALL_K_MSGQ_PUT 39
#define K_SYSCALL_K_MUTEX_INIT 40
#define K_SYSCALL_K_MUTEX_LOCK 41
#define K_SYSCALL_K_MUTEX_UNLOCK 42
#define K_SYSCALL_K_OBJECT_ACCESS_GRANT 43
#define K_SYSCALL_K_OBJECT_ALLOC 44
#define K_SYSCALL_K_OBJECT_ALLOC_SIZE 45
#define K_SYSCALL_K_OBJECT_RELEASE 46
#define K_SYSCALL_K_PIPE_ALLOC_INIT 47
#define K_SYSCALL_K_PIPE_BUFFER_FLUSH 48
#define K_SYSCALL_K_PIPE_FLUSH 49
#define K_SYSCALL_K_PIPE_GET 50
#define K_SYSCALL_K_PIPE_PUT 51
#define K_SYSCALL_K_PIPE_READ_AVAIL 52
#define K_SYSCALL_K_PIPE_WRITE_AVAIL 53
#define K_SYSCALL_K_POLL 54
#define K_SYSCALL_K_POLL_SIGNAL_CHECK 55
#define K_SYSCALL_K_POLL_SIGNAL_INIT 56
#define K_SYSCALL_K_POLL_SIGNAL_RAISE 57
#define K_SYSCALL_K_POLL_SIGNAL_RESET 58
#define K_SYSCALL_K_QUEUE_ALLOC_APPEND 59
#define K_SYSCALL_K_QUEUE_ALLOC_PREPEND 60
#define K_SYSCALL_K_QUEUE_CANCEL_WAIT 61
#define K_SYSCALL_K_QUEUE_GET 62
#define K_SYSCALL_K_QUEUE_INIT 63
#define K_SYSCALL_K_QUEUE_IS_EMPTY 64
#define K_SYSCALL_K_QUEUE_PEEK_HEAD 65
#define K_SYSCALL_K_QUEUE_PEEK_TAIL 66
#define K_SYSCALL_K_SEM_COUNT_GET 67
#define K_SYSCALL_K_SEM_GIVE 68
#define K_SYSCALL_K_SEM_INIT 69
#define K_SYSCALL_K_SEM_RESET 70
#define K_SYSCALL_K_SEM_TAKE 71
#define K_SYSCALL_K_SLEEP 72
#define K_SYSCALL_K_STACK_ALLOC_INIT 73
#define K_SYSCALL_K_STACK_POP 74
#define K_SYSCALL_K_STACK_PUSH 75
#define K_SYSCALL_K_STR_OUT 76
#define K_SYSCALL_K_THREAD_ABORT 77
#define K_SYSCALL_K_THREAD_CREATE 78
#define K_SYSCALL_K_THREAD_CUSTOM_DATA_GET 79
#define K_SYSCALL_K_THREAD_CUSTOM_DATA_SET 80
#define K_SYSCALL_K_THREAD_DEADLINE_SET 81
#define K_SYSCALL_K_THREAD_JOIN 82
#define K_SYSCALL_K_THREAD_NAME_COPY 83
#define K_SYSCALL_K_THREAD_NAME_SET 84
#define K_SYSCALL_K_THREAD_PRIORITY_GET 85
#define K_SYSCALL_K_THREAD_PRIORITY_SET 86
#define K_SYSCALL_K_THREAD_RESUME 87
#define K_SYSCALL_K_THREAD_STACK_ALLOC 88
#define K_SYSCALL_K_THREAD_STACK_FREE 89
#define K_SYSCALL_K_THREAD_STACK_SPACE_GET 90
#define K_SYSCALL_K_THREAD_START 91
#define K_SYSCALL_K_THREAD_SUSPEND 92
#define K_SYSCALL_K_THREAD_TIMEOUT_EXPIRES_TICKS 93
#define K_SYSCALL_K_THREAD_TIMEOUT_REMAINING_TICKS 94
#define K_SYSCALL_K_TIMER_EXPIRES_TICKS 95
#define K_SYSCALL_K_TIMER_REMAINING_TICKS 96
#define K_SYSCALL_K_TIMER_START 97
#define K_SYSCALL_K_TIMER_STATUS_GET 98
#define K_SYSCALL_K_TIMER_STATUS_SYNC 99
#define K_SYSCALL_K_TIMER_STOP 100
#define K_SYSCALL_K_TIMER_USER_DATA_GET 101
#define K_SYSCALL_K_TIMER_USER_DATA_SET 102
#define K_SYSCALL_K_UPTIME_TICKS 103
#define K_SYSCALL_K_USLEEP 104
#define K_SYSCALL_K_WAKEUP 105
#define K_SYSCALL_K_YIELD 106
#define K_SYSCALL_LOG_BUFFERED_CNT 107
#define K_SYSCALL_LOG_FILTER_SET 108
#define K_SYSCALL_LOG_PANIC 109
#define K_SYSCALL_LOG_PROCESS 110
#define K_SYSCALL_NET_ADDR_NTOP 111
#define K_SYSCALL_NET_ADDR_PTON 112
#define K_SYSCALL_NET_ETH_GET_PTP_CLOCK_BY_INDEX 113
#define K_SYSCALL_NET_IF_GET_BY_INDEX 114
#define K_SYSCALL_NET_IF_IPV4_ADDR_ADD_BY_INDEX 115
#define K_SYSCALL_NET_IF_IPV4_ADDR_LOOKUP_BY_INDEX 116
#define K_SYSCALL_NET_IF_IPV4_ADDR_RM_BY_INDEX 117
#define K_SYSCALL_NET_IF_IPV4_SET_GW_BY_INDEX 118
#define K_SYSCALL_NET_IF_IPV4_SET_NETMASK_BY_INDEX 119
#define K_SYSCALL_NET_IF_IPV6_ADDR_ADD_BY_INDEX 120
#define K_SYSCALL_NET_IF_IPV6_ADDR_LOOKUP_BY_INDEX 121
#define K_SYSCALL_NET_IF_IPV6_ADDR_RM_BY_INDEX 122
#define K_SYSCALL_SPI_RELEASE 123
#define K_SYSCALL_SPI_TRANSCEIVE 124
#define K_SYSCALL_SYS_CLOCK_HW_CYCLES_PER_SEC_RUNTIME_GET 125
#define K_SYSCALL_SYS_CSRAND_GET 126
#define K_SYSCALL_SYS_RAND32_GET 127
#define K_SYSCALL_SYS_RAND_GET 128
#define K_SYSCALL_UART_CONFIGURE 129
#define K_SYSCALL_UART_CONFIG_GET 130
#define K_SYSCALL_UART_DRV_CMD 131
#define K_SYSCALL_UART_ERR_CHECK 132
#define K_SYSCALL_UART_IRQ_ERR_DISABLE 133
#define K_SYSCALL_UART_IRQ_ERR_ENABLE 134
#define K_SYSCALL_UART_IRQ_IS_PENDING 135
#define K_SYSCALL_UART_IRQ_RX_DISABLE 136
#define K_SYSCALL_UART_IRQ_RX_ENABLE 137
#define K_SYSCALL_UART_IRQ_TX_DISABLE 138
#define K_SYSCALL_UART_IRQ_TX_ENABLE 139
#define K_SYSCALL_UART_IRQ_UPDATE 140
#define K_SYSCALL_UART_LINE_CTRL_GET 141
#define K_SYSCALL_UART_LINE_CTRL_SET 142
#define K_SYSCALL_UART_POLL_IN 143
#define K_SYSCALL_UART_POLL_IN_U16 144
#define K_SYSCALL_UART_POLL_OUT 145
#define K_SYSCALL_UART_POLL_OUT_U16 146
#define K_SYSCALL_UART_RX_DISABLE 147
#define K_SYSCALL_UART_RX_ENABLE 148
#define K_SYSCALL_UART_RX_ENABLE_U16 149
#define K_SYSCALL_UART_TX 150
#define K_SYSCALL_UART_TX_ABORT 151
#define K_SYSCALL_UART_TX_U16 152
#define K_SYSCALL_ZEPHYR_FPUTC 153
#define K_SYSCALL_ZEPHYR_FWRITE 154
#define K_SYSCALL_ZEPHYR_READ_STDIN 155
#define K_SYSCALL_ZEPHYR_WRITE_STDOUT 156
#define K_SYSCALL_ZSOCK_ACCEPT 157
#define K_SYSCALL_ZSOCK_BIND 158
#define K_SYSCALL_ZSOCK_CLOSE 159
#define K_SYSCALL_ZSOCK_CONNECT 160
#define K_SYSCALL_ZSOCK_FCNTL 161
#define K_SYSCALL_ZSOCK_GETHOSTNAME 162
#define K_SYSCALL_ZSOCK_GETPEERNAME 163
#define K_SYSCALL_ZSOCK_GETSOCKNAME 164
#define K_SYSCALL_ZSOCK_GETSOCKOPT 165
#define K_SYSCALL_ZSOCK_GET_CONTEXT_OBJECT 166
#define K_SYSCALL_ZSOCK_INET_PTON 167
#define K_SYSCALL_ZSOCK_IOCTL 168
#define K_SYSCALL_ZSOCK_LISTEN 169
#define K_SYSCALL_ZSOCK_POLL 170
#define K_SYSCALL_ZSOCK_RECVFROM 171
#define K_SYSCALL_ZSOCK_SELECT 172
#define K_SYSCALL_ZSOCK_SENDMSG 173
#define K_SYSCALL_ZSOCK_SENDTO 174
#define K_SYSCALL_ZSOCK_SETSOCKOPT 175
#define K_SYSCALL_ZSOCK_SHUTDOWN 176
#define K_SYSCALL_ZSOCK_SOCKET 177
#define K_SYSCALL_ZSOCK_SOCKETPAIR 178
#define K_SYSCALL_Z_CURRENT_GET 179
#define K_SYSCALL_Z_ERRNO 180
#define K_SYSCALL_Z_LOG_MSG_RUNTIME_VCREATE 181
#define K_SYSCALL_Z_LOG_MSG_STATIC_CREATE 182
#define K_SYSCALL_Z_SYS_MUTEX_KERNEL_LOCK 183
#define K_SYSCALL_Z_SYS_MUTEX_KERNEL_UNLOCK 184
#define K_SYSCALL_Z_ZSOCK_GETADDRINFO_INTERNAL 185
#define K_SYSCALL_BAD 186
#define K_SYSCALL_LIMIT 187


/* Following syscalls are not used in image */
#define K_SYSCALL_ADC_CHANNEL_SETUP 188
#define K_SYSCALL_ADC_READ 189
#define K_SYSCALL_ADC_READ_ASYNC 190
#define K_SYSCALL_ATOMIC_ADD 191
#define K_SYSCALL_ATOMIC_AND 192
#define K_SYSCALL_ATOMIC_CAS 193
#define K_SYSCALL_ATOMIC_NAND 194
#define K_SYSCALL_ATOMIC_OR 195
#define K_SYSCALL_ATOMIC_PTR_CAS 196
#define K_SYSCALL_ATOMIC_PTR_SET 197
#define K_SYSCALL_ATOMIC_SET 198
#define K_SYSCALL_ATOMIC_SUB 199
#define K_SYSCALL_ATOMIC_XOR 200
#define K_SYSCALL_AUXDISPLAY_BACKLIGHT_GET 201
#define K_SYSCALL_AUXDISPLAY_BACKLIGHT_SET 202
#define K_SYSCALL_AUXDISPLAY_BRIGHTNESS_GET 203
#define K_SYSCALL_AUXDISPLAY_BRIGHTNESS_SET 204
#define K_SYSCALL_AUXDISPLAY_CAPABILITIES_GET 205
#define K_SYSCALL_AUXDISPLAY_CLEAR 206
#define K_SYSCALL_AUXDISPLAY_CURSOR_POSITION_GET 207
#define K_SYSCALL_AUXDISPLAY_CURSOR_POSITION_SET 208
#define K_SYSCALL_AUXDISPLAY_CURSOR_SET_ENABLED 209
#define K_SYSCALL_AUXDISPLAY_CURSOR_SHIFT_SET 210
#define K_SYSCALL_AUXDISPLAY_CUSTOM_CHARACTER_SET 211
#define K_SYSCALL_AUXDISPLAY_CUSTOM_COMMAND 212
#define K_SYSCALL_AUXDISPLAY_DISPLAY_OFF 213
#define K_SYSCALL_AUXDISPLAY_DISPLAY_ON 214
#define K_SYSCALL_AUXDISPLAY_DISPLAY_POSITION_GET 215
#define K_SYSCALL_AUXDISPLAY_DISPLAY_POSITION_SET 216
#define K_SYSCALL_AUXDISPLAY_IS_BUSY 217
#define K_SYSCALL_AUXDISPLAY_POSITION_BLINKING_SET_ENABLED 218
#define K_SYSCALL_AUXDISPLAY_WRITE 219
#define K_SYSCALL_BBRAM_CHECK_INVALID 220
#define K_SYSCALL_BBRAM_CHECK_POWER 221
#define K_SYSCALL_BBRAM_CHECK_STANDBY_POWER 222
#define K_SYSCALL_BBRAM_GET_SIZE 223
#define K_SYSCALL_BBRAM_READ 224
#define K_SYSCALL_BBRAM_WRITE 225
#define K_SYSCALL_BC12_SET_RESULT_CB 226
#define K_SYSCALL_BC12_SET_ROLE 227
#define K_SYSCALL_CAN_ADD_RX_FILTER_MSGQ 228
#define K_SYSCALL_CAN_CALC_TIMING 229
#define K_SYSCALL_CAN_CALC_TIMING_DATA 230
#define K_SYSCALL_CAN_GET_CAPABILITIES 231
#define K_SYSCALL_CAN_GET_CORE_CLOCK 232
#define K_SYSCALL_CAN_GET_MAX_BITRATE 233
#define K_SYSCALL_CAN_GET_MAX_FILTERS 234
#define K_SYSCALL_CAN_GET_STATE 235
#define K_SYSCALL_CAN_GET_TIMING_DATA_MAX 236
#define K_SYSCALL_CAN_GET_TIMING_DATA_MIN 237
#define K_SYSCALL_CAN_GET_TIMING_MAX 238
#define K_SYSCALL_CAN_GET_TIMING_MIN 239
#define K_SYSCALL_CAN_RECOVER 240
#define K_SYSCALL_CAN_REMOVE_RX_FILTER 241
#define K_SYSCALL_CAN_SEND 242
#define K_SYSCALL_CAN_SET_BITRATE 243
#define K_SYSCALL_CAN_SET_BITRATE_DATA 244
#define K_SYSCALL_CAN_SET_MODE 245
#define K_SYSCALL_CAN_SET_TIMING 246
#define K_SYSCALL_CAN_SET_TIMING_DATA 247
#define K_SYSCALL_CAN_START 248
#define K_SYSCALL_CAN_STOP 249
#define K_SYSCALL_COUNTER_CANCEL_CHANNEL_ALARM 250
#define K_SYSCALL_COUNTER_GET_FREQUENCY 251
#define K_SYSCALL_COUNTER_GET_GUARD_PERIOD 252
#define K_SYSCALL_COUNTER_GET_MAX_TOP_VALUE 253
#define K_SYSCALL_COUNTER_GET_NUM_OF_CHANNELS 254
#define K_SYSCALL_COUNTER_GET_PENDING_INT 255
#define K_SYSCALL_COUNTER_GET_TOP_VALUE 256
#define K_SYSCALL_COUNTER_GET_VALUE 257
#define K_SYSCALL_COUNTER_GET_VALUE_64 258
#define K_SYSCALL_COUNTER_IS_COUNTING_UP 259
#define K_SYSCALL_COUNTER_SET_CHANNEL_ALARM 260
#define K_SYSCALL_COUNTER_SET_GUARD_PERIOD 261
#define K_SYSCALL_COUNTER_SET_TOP_VALUE 262
#define K_SYSCALL_COUNTER_START 263
#define K_SYSCALL_COUNTER_STOP 264
#define K_SYSCALL_COUNTER_TICKS_TO_US 265
#define K_SYSCALL_COUNTER_US_TO_TICKS 266
#define K_SYSCALL_DAC_CHANNEL_SETUP 267
#define K_SYSCALL_DAC_WRITE_VALUE 268
#define K_SYSCALL_DMA_CHAN_FILTER 269
#define K_SYSCALL_DMA_RELEASE_CHANNEL 270
#define K_SYSCALL_DMA_REQUEST_CHANNEL 271
#define K_SYSCALL_DMA_RESUME 272
#define K_SYSCALL_DMA_START 273
#define K_SYSCALL_DMA_STOP 274
#define K_SYSCALL_DMA_SUSPEND 275
#define K_SYSCALL_EEPROM_GET_SIZE 276
#define K_SYSCALL_EEPROM_READ 277
#define K_SYSCALL_EEPROM_WRITE 278
#define K_SYSCALL_ESPI_CONFIG 279
#define K_SYSCALL_ESPI_FLASH_ERASE 280
#define K_SYSCALL_ESPI_GET_CHANNEL_STATUS 281
#define K_SYSCALL_ESPI_READ_FLASH 282
#define K_SYSCALL_ESPI_READ_LPC_REQUEST 283
#define K_SYSCALL_ESPI_READ_REQUEST 284
#define K_SYSCALL_ESPI_RECEIVE_OOB 285
#define K_SYSCALL_ESPI_RECEIVE_VWIRE 286
#define K_SYSCALL_ESPI_SAF_ACTIVATE 287
#define K_SYSCALL_ESPI_SAF_CONFIG 288
#define K_SYSCALL_ESPI_SAF_FLASH_ERASE 289
#define K_SYSCALL_ESPI_SAF_FLASH_READ 290
#define K_SYSCALL_ESPI_SAF_FLASH_WRITE 291
#define K_SYSCALL_ESPI_SAF_GET_CHANNEL_STATUS 292
#define K_SYSCALL_ESPI_SAF_SET_PROTECTION_REGIONS 293
#define K_SYSCALL_ESPI_SEND_OOB 294
#define K_SYSCALL_ESPI_SEND_VWIRE 295
#define K_SYSCALL_ESPI_WRITE_FLASH 296
#define K_SYSCALL_ESPI_WRITE_LPC_REQUEST 297
#define K_SYSCALL_ESPI_WRITE_REQUEST 298
#define K_SYSCALL_FLASH_ERASE 299
#define K_SYSCALL_FLASH_EX_OP 300
#define K_SYSCALL_FLASH_GET_PAGE_COUNT 301
#define K_SYSCALL_FLASH_GET_PAGE_INFO_BY_IDX 302
#define K_SYSCALL_FLASH_GET_PAGE_INFO_BY_OFFS 303
#define K_SYSCALL_FLASH_GET_PARAMETERS 304
#define K_SYSCALL_FLASH_GET_WRITE_BLOCK_SIZE 305
#define K_SYSCALL_FLASH_READ 306
#define K_SYSCALL_FLASH_READ_JEDEC_ID 307
#define K_SYSCALL_FLASH_SFDP_READ 308
#define K_SYSCALL_FLASH_SIMULATOR_GET_MEMORY 309
#define K_SYSCALL_FLASH_WRITE 310
#define K_SYSCALL_FUEL_GAUGE_BATTERY_CUTOFF 311
#define K_SYSCALL_FUEL_GAUGE_GET_BUFFER_PROP 312
#define K_SYSCALL_FUEL_GAUGE_GET_PROP 313
#define K_SYSCALL_FUEL_GAUGE_SET_PROP 314
#define K_SYSCALL_HWINFO_CLEAR_RESET_CAUSE 315
#define K_SYSCALL_HWINFO_GET_DEVICE_ID 316
#define K_SYSCALL_HWINFO_GET_RESET_CAUSE 317
#define K_SYSCALL_HWINFO_GET_SUPPORTED_RESET_CAUSE 318
#define K_SYSCALL_HWSPINLOCK_GET_MAX_ID 319
#define K_SYSCALL_HWSPINLOCK_LOCK 320
#define K_SYSCALL_HWSPINLOCK_TRYLOCK 321
#define K_SYSCALL_HWSPINLOCK_UNLOCK 322
#define K_SYSCALL_I2C_CONFIGURE 323
#define K_SYSCALL_I2C_GET_CONFIG 324
#define K_SYSCALL_I2C_RECOVER_BUS 325
#define K_SYSCALL_I2C_TARGET_DRIVER_REGISTER 326
#define K_SYSCALL_I2C_TARGET_DRIVER_UNREGISTER 327
#define K_SYSCALL_I2C_TRANSFER 328
#define K_SYSCALL_I2S_BUF_READ 329
#define K_SYSCALL_I2S_BUF_WRITE 330
#define K_SYSCALL_I2S_CONFIGURE 331
#define K_SYSCALL_I2S_TRIGGER 332
#define K_SYSCALL_I3C_DO_CCC 333
#define K_SYSCALL_I3C_TRANSFER 334
#define K_SYSCALL_IPM_COMPLETE 335
#define K_SYSCALL_IPM_MAX_DATA_SIZE_GET 336
#define K_SYSCALL_IPM_MAX_ID_VAL_GET 337
#define K_SYSCALL_IPM_SEND 338
#define K_SYSCALL_IPM_SET_ENABLED 339
#define K_SYSCALL_IVSHMEM_ENABLE_INTERRUPTS 340
#define K_SYSCALL_IVSHMEM_GET_ID 341
#define K_SYSCALL_IVSHMEM_GET_MAX_PEERS 342
#define K_SYSCALL_IVSHMEM_GET_MEM 343
#define K_SYSCALL_IVSHMEM_GET_OUTPUT_MEM_SECTION 344
#define K_SYSCALL_IVSHMEM_GET_PROTOCOL 345
#define K_SYSCALL_IVSHMEM_GET_RW_MEM_SECTION 346
#define K_SYSCALL_IVSHMEM_GET_STATE 347
#define K_SYSCALL_IVSHMEM_GET_VECTORS 348
#define K_SYSCALL_IVSHMEM_INT_PEER 349
#define K_SYSCALL_IVSHMEM_REGISTER_HANDLER 350
#define K_SYSCALL_IVSHMEM_SET_STATE 351
#define K_SYSCALL_KSCAN_CONFIG 352
#define K_SYSCALL_KSCAN_DISABLE_CALLBACK 353
#define K_SYSCALL_KSCAN_ENABLE_CALLBACK 354
#define K_SYSCALL_K_MEM_PAGING_HISTOGRAM_BACKING_STORE_PAGE_IN_GET 355
#define K_SYSCALL_K_MEM_PAGING_HISTOGRAM_BACKING_STORE_PAGE_OUT_GET 356
#define K_SYSCALL_K_MEM_PAGING_HISTOGRAM_EVICTION_GET 357
#define K_SYSCALL_K_MEM_PAGING_STATS_GET 358
#define K_SYSCALL_K_MEM_PAGING_THREAD_STATS_GET 359
#define K_SYSCALL_LED_BLINK 360
#define K_SYSCALL_LED_GET_INFO 361
#define K_SYSCALL_LED_OFF 362
#define K_SYSCALL_LED_ON 363
#define K_SYSCALL_LED_SET_BRIGHTNESS 364
#define K_SYSCALL_LED_SET_CHANNEL 365
#define K_SYSCALL_LED_SET_COLOR 366
#define K_SYSCALL_LED_WRITE_CHANNELS 367
#define K_SYSCALL_MAXIM_DS3231_GET_SYNCPOINT 368
#define K_SYSCALL_MAXIM_DS3231_REQ_SYNCPOINT 369
#define K_SYSCALL_MBOX_MAX_CHANNELS_GET 370
#define K_SYSCALL_MBOX_MTU_GET 371
#define K_SYSCALL_MBOX_SEND 372
#define K_SYSCALL_MBOX_SET_ENABLED 373
#define K_SYSCALL_MDIO_BUS_DISABLE 374
#define K_SYSCALL_MDIO_BUS_ENABLE 375
#define K_SYSCALL_MDIO_READ 376
#define K_SYSCALL_MDIO_WRITE 377
#define K_SYSCALL_NRF_QSPI_NOR_XIP_ENABLE 378
#define K_SYSCALL_PECI_CONFIG 379
#define K_SYSCALL_PECI_DISABLE 380
#define K_SYSCALL_PECI_ENABLE 381
#define K_SYSCALL_PECI_TRANSFER 382
#define K_SYSCALL_PHY_CONFIGURE_LINK 383
#define K_SYSCALL_PHY_GET_LINK_STATE 384
#define K_SYSCALL_PHY_LINK_CALLBACK_SET 385
#define K_SYSCALL_PHY_READ 386
#define K_SYSCALL_PHY_WRITE 387
#define K_SYSCALL_PS2_CONFIG 388
#define K_SYSCALL_PS2_DISABLE_CALLBACK 389
#define K_SYSCALL_PS2_ENABLE_CALLBACK 390
#define K_SYSCALL_PS2_READ 391
#define K_SYSCALL_PS2_WRITE 392
#define K_SYSCALL_PTP_CLOCK_GET 393
#define K_SYSCALL_PWM_CAPTURE_CYCLES 394
#define K_SYSCALL_PWM_DISABLE_CAPTURE 395
#define K_SYSCALL_PWM_ENABLE_CAPTURE 396
#define K_SYSCALL_PWM_GET_CYCLES_PER_SEC 397
#define K_SYSCALL_PWM_SET_CYCLES 398
#define K_SYSCALL_RESET_LINE_ASSERT 399
#define K_SYSCALL_RESET_LINE_DEASSERT 400
#define K_SYSCALL_RESET_LINE_TOGGLE 401
#define K_SYSCALL_RESET_STATUS 402
#define K_SYSCALL_RETAINED_MEM_CLEAR 403
#define K_SYSCALL_RETAINED_MEM_READ 404
#define K_SYSCALL_RETAINED_MEM_SIZE 405
#define K_SYSCALL_RETAINED_MEM_WRITE 406
#define K_SYSCALL_RTC_ALARM_GET_SUPPORTED_FIELDS 407
#define K_SYSCALL_RTC_ALARM_GET_TIME 408
#define K_SYSCALL_RTC_ALARM_IS_PENDING 409
#define K_SYSCALL_RTC_ALARM_SET_CALLBACK 410
#define K_SYSCALL_RTC_ALARM_SET_TIME 411
#define K_SYSCALL_RTC_GET_CALIBRATION 412
#define K_SYSCALL_RTC_GET_TIME 413
#define K_SYSCALL_RTC_SET_CALIBRATION 414
#define K_SYSCALL_RTC_SET_TIME 415
#define K_SYSCALL_RTC_UPDATE_SET_CALLBACK 416
#define K_SYSCALL_RTIO_CQE_COPY_OUT 417
#define K_SYSCALL_RTIO_CQE_GET_MEMPOOL_BUFFER 418
#define K_SYSCALL_RTIO_RELEASE_BUFFER 419
#define K_SYSCALL_RTIO_SQE_CANCEL 420
#define K_SYSCALL_RTIO_SQE_COPY_IN_GET_HANDLES 421
#define K_SYSCALL_RTIO_SUBMIT 422
#define K_SYSCALL_SDHC_CARD_BUSY 423
#define K_SYSCALL_SDHC_CARD_PRESENT 424
#define K_SYSCALL_SDHC_EXECUTE_TUNING 425
#define K_SYSCALL_SDHC_GET_HOST_PROPS 426
#define K_SYSCALL_SDHC_HW_RESET 427
#define K_SYSCALL_SDHC_REQUEST 428
#define K_SYSCALL_SDHC_SET_IO 429
#define K_SYSCALL_SENSOR_ATTR_GET 430
#define K_SYSCALL_SENSOR_ATTR_SET 431
#define K_SYSCALL_SENSOR_CHANNEL_GET 432
#define K_SYSCALL_SENSOR_GET_DECODER 433
#define K_SYSCALL_SENSOR_RECONFIGURE_READ_IODEV 434
#define K_SYSCALL_SENSOR_SAMPLE_FETCH 435
#define K_SYSCALL_SENSOR_SAMPLE_FETCH_CHAN 436
#define K_SYSCALL_SIP_SUPERVISORY_CALL 437
#define K_SYSCALL_SIP_SVC_PLAT_ASYNC_RES_REQ 438
#define K_SYSCALL_SIP_SVC_PLAT_ASYNC_RES_RES 439
#define K_SYSCALL_SIP_SVC_PLAT_FORMAT_TRANS_ID 440
#define K_SYSCALL_SIP_SVC_PLAT_FREE_ASYNC_MEMORY 441
#define K_SYSCALL_SIP_SVC_PLAT_FUNC_ID_VALID 442
#define K_SYSCALL_SIP_SVC_PLAT_GET_ERROR_CODE 443
#define K_SYSCALL_SIP_SVC_PLAT_GET_TRANS_IDX 444
#define K_SYSCALL_SIP_SVC_PLAT_UPDATE_TRANS_ID 445
#define K_SYSCALL_SMBUS_BLOCK_PCALL 446
#define K_SYSCALL_SMBUS_BLOCK_READ 447
#define K_SYSCALL_SMBUS_BLOCK_WRITE 448
#define K_SYSCALL_SMBUS_BYTE_DATA_READ 449
#define K_SYSCALL_SMBUS_BYTE_DATA_WRITE 450
#define K_SYSCALL_SMBUS_BYTE_READ 451
#define K_SYSCALL_SMBUS_BYTE_WRITE 452
#define K_SYSCALL_SMBUS_CONFIGURE 453
#define K_SYSCALL_SMBUS_GET_CONFIG 454
#define K_SYSCALL_SMBUS_HOST_NOTIFY_REMOVE_CB 455
#define K_SYSCALL_SMBUS_HOST_NOTIFY_SET_CB 456
#define K_SYSCALL_SMBUS_PCALL 457
#define K_SYSCALL_SMBUS_QUICK 458
#define K_SYSCALL_SMBUS_SMBALERT_REMOVE_CB 459
#define K_SYSCALL_SMBUS_SMBALERT_SET_CB 460
#define K_SYSCALL_SMBUS_WORD_DATA_READ 461
#define K_SYSCALL_SMBUS_WORD_DATA_WRITE 462
#define K_SYSCALL_SYSCON_GET_BASE 463
#define K_SYSCALL_SYSCON_GET_SIZE 464
#define K_SYSCALL_SYSCON_READ_REG 465
#define K_SYSCALL_SYSCON_WRITE_REG 466
#define K_SYSCALL_SYS_CACHE_DATA_FLUSH_AND_INVD_RANGE 467
#define K_SYSCALL_SYS_CACHE_DATA_FLUSH_RANGE 468
#define K_SYSCALL_SYS_CACHE_DATA_INVD_RANGE 469
#define K_SYSCALL_UART_MUX_FIND 470
#define K_SYSCALL_UPDATEHUB_AUTOHANDLER 471
#define K_SYSCALL_UPDATEHUB_CONFIRM 472
#define K_SYSCALL_UPDATEHUB_PROBE 473
#define K_SYSCALL_UPDATEHUB_REBOOT 474
#define K_SYSCALL_UPDATEHUB_UPDATE 475
#define K_SYSCALL_USER_FAULT 476
#define K_SYSCALL_W1_CHANGE_BUS_LOCK 477
#define K_SYSCALL_W1_CONFIGURE 478
#define K_SYSCALL_W1_GET_SLAVE_COUNT 479
#define K_SYSCALL_W1_READ_BIT 480
#define K_SYSCALL_W1_READ_BLOCK 481
#define K_SYSCALL_W1_READ_BYTE 482
#define K_SYSCALL_W1_RESET_BUS 483
#define K_SYSCALL_W1_SEARCH_BUS 484
#define K_SYSCALL_W1_WRITE_BIT 485
#define K_SYSCALL_W1_WRITE_BLOCK 486
#define K_SYSCALL_W1_WRITE_BYTE 487
#define K_SYSCALL_WDT_DISABLE 488
#define K_SYSCALL_WDT_FEED 489
#define K_SYSCALL_WDT_SETUP 490


#ifndef _ASMLANGUAGE

#include <stdarg.h>
#include <stdint.h>

#endif /* _ASMLANGUAGE */

#endif /* ZEPHYR_SYSCALL_LIST_H */
