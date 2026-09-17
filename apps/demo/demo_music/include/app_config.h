#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#define __FLASH_SIZE__    (8 * 1024 * 1024)
#define __SDRAM_SIZE__    (8 * 1024 * 1024)

//*********************************************************************************//
//                             功能模块使能                                        //
//*********************************************************************************//
#define CONFIG_DEBUG_ENABLE                     /* 打印开关 */
#define CONFIG_UART_ENABLE                      /* 串口打印 */
#define CONFIG_USB_ENABLE                       /* USB 从机 CDC */
#define CONFIG_USB_DEBUG_ENABLE                 /* printf 经 USB CDC 输出 */
#define CONFIG_KEY_ENABLE                       /* 按键 */
#define CONFIG_RTC_ENABLE                       /* RTC */

#define CONFIG_WIFI_ENABLE                      /* WiFi */
#ifdef CONFIG_NO_SDRAM_ENABLE
#define CONFIG_RF_TRIM_CODE_MOVABLE
#else
#define CONFIG_RF_TRIM_CODE_AT_RAM
#endif

#define CONFIG_NET_ENABLE
#define RTOS_STACK_CHECK_ENABLE

//*********************************************************************************//
//                             按键配置                                            //
//*********************************************************************************//
#ifdef CONFIG_KEY_ENABLE
#define TCFG_ADKEY_ENABLE           1
#define TCFG_IRKEY_ENABLE           0
#define TCFG_IOKEY_ENABLE           0
#define TCFG_RDEC_KEY_ENABLE        0
#define TCFG_TOUCH_KEY_ENABLE       0
#endif

//*********************************************************************************//
//                             音频配置                                            //
//*********************************************************************************//
#define CONFIG_MP3_DEC_ENABLE

#define CONFIG_VOICE_PROMPT_FILE_PATH       "mnt/sdfile/res/audlogo/"
#define MP3_VOLUME                          50

#define CONFIG_AUDIO_DEC_PLAY_SOURCE        "dac"

//*********************************************************************************//
//                             存储路径（device_mount.c 依赖）                      //
//*********************************************************************************//
#define TCFG_SD0_ENABLE                     0
#define SDX_DEV                             "sd0"
#define CONFIG_STORAGE_PATH                 "storage/sd0"
#define CONFIG_ROOT_PATH                    CONFIG_STORAGE_PATH "/C/"

//*********************************************************************************//
//                             USB 配置（CDC 虚拟串口）                            //
//*********************************************************************************//
#ifdef CONFIG_USB_ENABLE
#define TCFG_PC_ENABLE                      1
#define USB_PC_NO_APP_MODE                  2
#define USB_MALLOC_ENABLE                   1
#define USB_DEVICE_CLASS_CONFIG             (CDC_CLASS)
#define TCFG_HOST_AUDIO_ENABLE              0
#define TCFG_HOST_UVC_ENABLE                0
#define TCFG_HID_HOST_ENABLE                0
#define TCFG_UDISK_ENABLE                   0
#include "usb_std_class_def.h"
#include "usb_common_def.h"
#endif

//*********************************************************************************//
//                             电源配置                                            //
//*********************************************************************************//
#define TCFG_LOWPOWER_BTOSC_DISABLE         0
#define TCFG_LOWPOWER_LOWPOWER_SEL          0
#define TCFG_LOWPOWER_VDDIOM_LEVEL          VDDIOM_VOL_32V

#ifdef CONFIG_RTC_ENABLE
#define TCFG_LOWPOWER_VDDIOW_LEVEL          VDDIOW_VOL_32V
#else
#define TCFG_LOWPOWER_VDDIOW_LEVEL          VDDIOW_VOL_21V
#endif

#define VDC14_VOL_SEL_LEVEL                 VDC14_VOL_SEL_140V
#define SYSVDD_VOL_SEL_LEVEL                SYSVDD_VOL_SEL_126V

#if !defined CONFIG_DEBUG_ENABLE || defined CONFIG_LIB_DEBUG_DISABLE
#define LIB_DEBUG    0
#else
#define LIB_DEBUG    1
#endif
#define CONFIG_DEBUG_LIB(x)         (x & LIB_DEBUG)

#endif
