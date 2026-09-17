#include "app_config.h"
#include "system/includes.h"
#include "os/os_api.h"
#include "event/net_event.h"
#include "event/key_event.h"
#include "event/device_event.h"
#include "wifi/wifi_connect.h"
#include "net/config_network.h"
#include "ntp/ntp.h"
#include "cJSON.h"
#include "IOTSdkBridge.h"

/*中断列表 */
const struct irq_info irq_info_table[] = {
#ifdef CONFIG_IPMASK_ENABLE
    { IRQ_SOFT5_IDX,      6,   0    },
    { IRQ_SOFT4_IDX,      6,   1    },
#endif
#if CPU_CORE_NUM == 1
    { IRQ_SOFT5_IDX,      7,   0    },
    { IRQ_SOFT4_IDX,      7,   1    },
    { -2,     			-2,   -2   },
#endif
    { -1,     -1,   -1    },
};

/*任务列表 */
const struct task_info task_info_table[] = {
    {"init",                30,     512,   256   },
    {"app_core",            15,     2048,  1024  },
    {"sys_event",           29,     512,   0     },
    {"systimer",            14,     256,   0     },
    {"sys_timer",            9,     512,   128   },
    {"audio_server",        16,     1024,  64    },
    {"audio_decoder",       30,     1024,  64    },
    {"usb_msd0",             1,     512,   128   },
    {"tcpip_thread",        16,     800,   0     },
#ifdef CONFIG_WIFI_ENABLE
    {"tasklet",             10,     1400,  0     },
    {"RtmpMlmeTask",        17,     700,   0     },
    {"RtmpCmdQTask",        17,     300,   0     },
    {"wl_rx_irq_thread",     5,     256,   0     },
#endif
    {0, 0, 0, 0, 0},
};

#ifdef CONFIG_MP3_DEC_ENABLE
extern void post_msg_play_flash_mp3(char *file_name, u8 dec_volume);
#endif

static u8 ntp_sync_started = 0;

static void demo_music_ntp_sync_task(void *priv)
{
    ntp_client_get_time(NULL);
}

static void demo_music_start_ntp_sync(void)
{
    if (ntp_sync_started) {
        return;
    }
    if (ntp_client_get_time_status()) {
        puts("[demo_music] NTP already synced\n");
        return;
    }
    ntp_sync_started = 1;
    if (thread_fork("ntp_sync", 10, 1024, 0, 0, demo_music_ntp_sync_task, NULL) != OS_NO_ERR) {
        ntp_sync_started = 0;
        puts("[demo_music] NTP thread_fork fail\n");
    } else {
        puts("[demo_music] NTP sync started\n");
    }
}

static void demo_iotsdk_init(void)
{
    IOTSdk_Init("./", "{\"appLicenseId\":\"\",\"appKey\":\"\",\"serverToken\":\"\",\"regionCode\":\"\",\"servicePackageCode\":\"\",\"env\":\"\"}");
}

static void demo_iotsdk_search_song(void)
{
    char* output = NULL;
    IOTSdk_Search("{\"provider\":\"migu\",\"text\":\"陈奕迅的十年\",\"pageIndex\":1,\"pageSize\":5,\"searchRange\":{\"songName\":[\"十年\"],\"singerName\":[\"陈奕迅\"]}}", &output, 10000);
    printf("[demo_music] IOTSdk_Search: %s\n", output);
    free(output);
}

static int main_key_event_handler(struct key_event *key)
{
    printf(">>>>>>>>>>>>>>main_key_event_handler: key->action=%d, key->value=%d", key->action, key->value);
    switch (key->action) {
    case KEY_EVENT_CLICK:
        switch (key->value) {
        case KEY_K1:
            demo_iotsdk_init();
            break;
        case KEY_K2:
            printf("[demo_music] K2 click (KEY_MODE=%d)\n", KEY_K2);
            break;
        case KEY_K3:
            printf("[demo_music] K3 click (KEY_VOLUME_DEC=%d)\n", KEY_K3);
            break;
        case KEY_K4:
            demo_iotsdk_search_song();
            break;
        case KEY_K5:
            printf("[demo_music] K5 click (KEY_OK=%d), play prompt\n", KEY_K5);
#ifdef CONFIG_MP3_DEC_ENABLE
            post_msg_play_flash_mp3("Volume.mp3", MP3_VOLUME);
#endif
            break;
        case KEY_K6:
            printf("[demo_music] K6 click (KEY_PHOTO=%d)\n", KEY_K6);
            break;
        case KEY_K7:
            printf("[demo_music] K7 click (KEY_ENC=%d)\n", KEY_K7);
            break;
        case KEY_K8:
            printf("[demo_music] K8 click (KEY_POWER=%d)\n", KEY_K8);
            break;
        default:
            printf("[demo_music] unknown key value=%d\n", key->value);
            return false;
        }
        break;
    case KEY_EVENT_LONG:
        switch (key->value) {
        case KEY_K1:
        case KEY_K2:
        case KEY_K3:
        case KEY_K4:
        case KEY_K5:
        case KEY_K6:
        case KEY_K7:
        case KEY_K8:
            printf("[demo_music] key value=%d long press\n", key->value);
            break;
        default:
            return false;
        }
        break;
    default:
        return false;
    }
    return true;
}

static int app_demo_state_machine(struct application *app, enum app_state state, struct intent *it)
{
    return 0;
}

static int app_demo_event_handler(struct application *app, struct sys_event *sys_event)
{
    switch (sys_event->type) {
    case SYS_NET_EVENT:
        struct net_event *net_event = (struct net_event *)sys_event->payload;
        if (!ASCII_StrCmp(net_event->arg, "net", 4)) {
            switch (net_event->event) {
            case NET_EVENT_CONNECTED:
                puts("[demo_music] WiFi STA DHCP ok, network ready\n");
                demo_music_start_ntp_sync();
                break;
            case NET_EVENT_DISCONNECTED:
                ntp_sync_started = 0;
                puts("[demo_music] network disconnected\n");
                break;
            case NET_NTP_GET_TIME_SUCC:
                puts("[demo_music] NTP sync success\n");
                break;
            case NET_CONNECT_TIMEOUT_NOT_FOUND_SSID:
            case NET_CONNECT_ASSOCIAT_FAIL:
                printf("[demo_music] net event %d\n", net_event->event);
                break;
            default:
                break;
            }
        }
        break;
    default:
        return false;
    }
    return false;
}

static const struct application_operation app_demo_ops = {
    .state_machine  = app_demo_state_machine,
    .event_handler  = app_demo_event_handler,
};

REGISTER_APPLICATION(app_demo) = {
    .name   = "app_demo",
    .ops    = &app_demo_ops,
    .state  = APP_STA_DESTROY,
};

void app_default_event_handler(struct sys_event *event)
{
    switch (event->type) {
    case SYS_KEY_EVENT:
        main_key_event_handler((struct key_event *)event->payload);
        break;
    case SYS_DEVICE_EVENT:
        break;
    case SYS_NET_EVENT:
        break;
    default:
        break;
    }
}

void app_main()
{
    printf("\r\n\r\n -------- demo_music app_main %s --------\r\n\r\n", __TIME__);

    struct intent it;
    init_intent(&it);
    it.name = "app_demo";
    it.action = ACTION_DO_NOTHING;
    start_app(&it);
}
