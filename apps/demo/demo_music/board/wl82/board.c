#include "app_config.h"

#include "system/includes.h"
#include "device/includes.h"
#include "asm/includes.h"
#include "server/audio_dev.h"
#ifdef CONFIG_USB_ENABLE
#include "otg.h"
#include "usb_host.h"
#include "usb_storage.h"
#endif

// *INDENT-OFF*

/* 物理 UART 打印（与 DevKitBoard 一致，勿占用 USB 引脚 PORTUSB_A） */
#ifdef CONFIG_DEBUG_ENABLE
UART1_PLATFORM_DATA_BEGIN(uart1_data)
    .baudrate = 1000000,
    .port = PORT_REMAP,
    .output_channel = OUTPUT_CHANNEL0,
    .tx_pin = IO_PORTB_03,
    .rx_pin = -1,
    .max_continue_recv_cnt = 1024,
    .idle_sys_clk_cnt = 500000,
    .clk_src = PLL_48M,
    .flags = UART_DEBUG,
UART1_PLATFORM_DATA_END();
#endif

UART2_PLATFORM_DATA_BEGIN(uart2_data)
    .baudrate = 1000000,
    .port = PORT_REMAP,
    .output_channel = OUTPUT_CHANNEL0,
    .tx_pin = IO_PORTC_00,
    .rx_pin = -1,
    .max_continue_recv_cnt = 1024,
    .idle_sys_clk_cnt = 500000,
    .clk_src = PLL_48M,
    .flags = UART_DEBUG,
UART2_PLATFORM_DATA_END();

#if TCFG_ADKEY_ENABLE
/* 与 demo_DevKitBoard / DevKit 开源板 8 键 AD 矩阵一致 */
#define ADKEY_UPLOAD_R  22
#define ADC_VDDIO (0x3FF)
#define ADC_09   (0x3FF)
#define ADC_08   (0x3FF)
#define ADC_07   (0x3FF * 150 / (150 + ADKEY_UPLOAD_R))
#define ADC_06   (0x3FF * 62  / (62  + ADKEY_UPLOAD_R))
#define ADC_05   (0x3FF * 36  / (36  + ADKEY_UPLOAD_R))
#define ADC_04   (0x3FF * 22  / (22  + ADKEY_UPLOAD_R))
#define ADC_03   (0x3FF * 13  / (13  + ADKEY_UPLOAD_R))
#define ADC_02   (0x3FF * 75  / (75  + ADKEY_UPLOAD_R * 10))
#define ADC_01   (0x3FF * 3   / (3   + ADKEY_UPLOAD_R))
#define ADC_00   (0)
#define ADKEY_V_9      	((ADC_09 + ADC_VDDIO)/2)
#define ADKEY_V_8 		((ADC_08 + ADC_09)/2)
#define ADKEY_V_7 		((ADC_07 + ADC_08)/2 + 50)
#define ADKEY_V_6 		((ADC_06 + ADC_07)/2)
#define ADKEY_V_5 		((ADC_05 + ADC_06)/2)
#define ADKEY_V_4 		((ADC_04 + ADC_05)/2)
#define ADKEY_V_3 		((ADC_03 + ADC_04)/2)
#define ADKEY_V_2 		((ADC_02 + ADC_03)/2)
#define ADKEY_V_1 		((ADC_01 + ADC_02)/2)
#define ADKEY_V_0 		((ADC_00 + ADC_01)/2)

const struct adkey_platform_data adkey_data = {
    .enable     = 1,
    .adkey_pin  = IO_PORTB_01,
    .extern_up_en = 1,
    .ad_channel = 3,
    .ad_value = {
        ADKEY_V_0,
        ADKEY_V_1,
        ADKEY_V_2,
        ADKEY_V_3,
        ADKEY_V_4,
        ADKEY_V_5,
        ADKEY_V_6,
        ADKEY_V_7,
        ADKEY_V_8,
        ADKEY_V_9,
    },
    .key_value = {
        KEY_POWER,       /* K8 */
        KEY_ENC,         /* K7 */
        KEY_PHOTO,       /* K6 */
        KEY_OK,          /* K5 */
        KEY_VOLUME_INC,  /* K4 */
        KEY_VOLUME_DEC,  /* K3 */
        KEY_MODE,        /* K2 */
        KEY_CANCLE,      /* K1 */
        NO_KEY,
        NO_KEY,
    },
};
#endif

static const struct dac_platform_data dac_data = {
    .pa_auto_mute = 1,
    .pa_mute_port = IO_PORTB_02,
    .pa_mute_value = 0,
    .differ_output = 1,
    .hw_channel = 0x03,
    .ch_num = 1,
    .vcm_init_delay_ms = 1000,
};

static const struct adc_platform_data adc_data = {
    .mic_channel = LADC_CH_MIC1_P,
    .linein_channel = 0,
    .mic_ch_num = 1,
    .linein_ch_num = 0,
    .all_channel_open = 0,
    .isel = 2,
    .dump_num = 480,
};

static const struct audio_pf_data audio_pf_d = {
    .adc_pf_data = &adc_data,
    .dac_pf_data = &dac_data,
};

static const struct audio_platform_data audio_data = {
    .private_data = (void *)&audio_pf_d,
};

#ifdef CONFIG_USB_ENABLE
static const struct otg_dev_data otg_data = {
    .usb_dev_en = 0x03,
#if TCFG_USB_SLAVE_ENABLE
    .slave_online_cnt = 10,
    .slave_offline_cnt = 10,
#endif
#if TCFG_USB_HOST_ENABLE
    .host_online_cnt = 10,
    .host_offline_cnt = 10,
#endif
    .detect_mode = OTG_HOST_MODE | OTG_SLAVE_MODE | OTG_CHARGE_MODE,
    .detect_time_interval = 50,
};
#endif

#if defined CONFIG_WIFI_ENABLE
#include "wifi/wifi_connect.h"
const struct wifi_calibration_param wifi_calibration_param = {
    .xosc_l     = 0xb,
    .xosc_r     = 0xb,
    .pa_trim_data  = {1, 7, 4, 7, 11, 1, 7},
    .mcs_dgain     = {
        50, 50, 50, 50,
        72, 72, 85, 80, 64, 64, 62, 52,
        72, 90, 80, 64, 64, 64, 50, 43,
    }
};
#endif

const struct rtc_init_config rtc_init_data = {
    .rtc_clk_sel = RTC_CLK_SEL_INSIDE,
    .rtc_power_sel = RTCVDD_SUPPLY_OUTSIDE,
};

static const struct low_power_param power_param = {
    .config         = TCFG_LOWPOWER_LOWPOWER_SEL,
    .btosc_disable  = TCFG_LOWPOWER_BTOSC_DISABLE,
    .vddiom_lev     = TCFG_LOWPOWER_VDDIOM_LEVEL,
    .vddiow_lev     = TCFG_LOWPOWER_VDDIOW_LEVEL,
    .vdc14_dcdc     = TRUE,
    .vdc14_lev      = VDC14_VOL_SEL_LEVEL,
    .sysvdd_lev     = SYSVDD_VOL_SEL_LEVEL,
    .vlvd_enable    = TRUE,
    .vlvd_value     = VLVD_SEL_25V,
};

REGISTER_DEVICES(device_table) = {
#ifdef CONFIG_DEBUG_ENABLE
    {"uart1", &uart_dev_ops, (void *)&uart1_data },
#endif
    {"uart2", &uart_dev_ops, (void *)&uart2_data },
    {"audio", &audio_dev_ops, (void *)&audio_data },
#ifdef CONFIG_USB_ENABLE
    {"otg", &usb_dev_ops, (void *)&otg_data},
#endif
    {"rtc", &rtc_dev_ops, NULL},
};

#ifdef CONFIG_DEBUG_ENABLE
void debug_uart_init()
{
    uart_init(&uart1_data);
}
#endif

void board_early_init()
{
    dac_early_init(0, dac_data.differ_output ? (dac_data.ch_num > 1 ? 0xf : 0x3) : dac_data.hw_channel, 1000);
    devices_init();
}

static void board_power_init(void)
{
    power_init(&power_param);
}

void board_init()
{
    board_power_init();
#ifdef CONFIG_RTC_ENABLE
    rtc_early_init();
#endif
#if TCFG_ADKEY_ENABLE || defined CONFIG_WIFI_ENABLE
    adc_init();
#endif
#if TCFG_ADKEY_ENABLE
    key_driver_init();
#endif
}
