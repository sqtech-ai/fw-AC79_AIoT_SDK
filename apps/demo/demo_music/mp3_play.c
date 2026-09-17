#include "app_config.h"

#ifdef CONFIG_MP3_DEC_ENABLE

#include "device/device.h"
#include "system/includes.h"
#include "sys_common.h"

#include "server/audio_server.h"

#define SHUTDOWN_CMD    (1001)

struct flash_mp3_hdl {
    struct server *dec_server;
    char file_path[64];
    u8 dec_volume;
    FILE *file;
};

static struct flash_mp3_hdl *mp3_info = NULL;
static u8 play_mp3_flag = 0;

static void dec_server_event_handler(void *priv, int argc, int *argv)
{
    int msg = 0;
    switch (argv[0]) {
    case AUDIO_SERVER_EVENT_ERR:
        printf("AUDIO_SERVER_EVENT_ERR\n");
        break;
    case AUDIO_SERVER_EVENT_END:
        msg = SHUTDOWN_CMD;
        os_taskq_post(os_current_task(), 1, msg);
        break;
    default:
        break;
    }
}

static void play_mp3_task(void *priv)
{
    int msg[32] = {0};

    mp3_info->file = fopen(mp3_info->file_path, "r");
    if (!mp3_info->file) {
        puts("demo_music: mp3 file not found\n");
        os_taskq_post("flash_mp3_play_task", 1, mp3_info);
        return;
    }

    mp3_info->dec_server = server_open("audio_server", "dec");
    if (!mp3_info->dec_server) {
        puts("demo_music: open audio_server fail\n");
        fclose(mp3_info->file);
        os_taskq_post("flash_mp3_play_task", 1, mp3_info);
        return;
    }

    server_register_event_handler(mp3_info->dec_server, NULL, dec_server_event_handler);

    union audio_req req = {0};
    req.dec.cmd             = AUDIO_DEC_OPEN;
    req.dec.volume          = mp3_info->dec_volume;
    req.dec.output_buf      = NULL;
    req.dec.output_buf_len  = 12 * 1024;
    req.dec.channel         = 0;
    req.dec.sample_rate     = 0;
    req.dec.priority        = 1;
    req.dec.vfs_ops         = NULL;
    req.dec.file            = mp3_info->file;
    req.dec.dec_type        = "mp3";
    req.dec.sample_source   = CONFIG_AUDIO_DEC_PLAY_SOURCE;
    if (server_request(mp3_info->dec_server, AUDIO_REQ_DEC, &req) != 0) {
        puts("demo_music: audio dec open err\n");
        goto __err;
    }

    req.dec.cmd = AUDIO_DEC_START;
    if (server_request(mp3_info->dec_server, AUDIO_REQ_DEC, &req) != 0) {
        puts("demo_music: audio dec start err\n");
        goto __err;
    }

    for (;;) {
        os_task_pend("taskq", msg, ARRAY_SIZE(msg));
        if (msg[1] == SHUTDOWN_CMD) {
            req.dec.cmd = AUDIO_DEC_STOP;
            server_request(mp3_info->dec_server, AUDIO_REQ_DEC, &req);
            server_close(mp3_info->dec_server);
            if (mp3_info->file) {
                fclose(mp3_info->file);
            }
            os_taskq_post("flash_mp3_play_task", 1, mp3_info);
            break;
        }
    }
    return;

__err:
    if (mp3_info->file) {
        fclose(mp3_info->file);
    }
    server_close(mp3_info->dec_server);
    os_taskq_post("flash_mp3_play_task", 1, mp3_info);
}

static void flash_mp3_play_task(void *priv)
{
    int err;
    int msg[32] = {0};

    os_time_dly(50);
    while (1) {
        play_mp3_flag = 1;
        err = os_taskq_pend("taskq", msg, ARRAY_SIZE(msg));
        if (err != OS_TASKQ || msg[0] != Q_USER) {
            continue;
        }

        mp3_info = (struct flash_mp3_hdl *)msg[1];
        thread_fork("play_mp3_task", 10, 1024, 32, NULL, play_mp3_task, NULL);

        err = os_taskq_pend("taskq", msg, ARRAY_SIZE(msg));
        if (err != OS_TASKQ || msg[0] != Q_USER) {
            continue;
        }
    }
}

static void flash_mp3_open(void)
{
    mp3_info = (struct flash_mp3_hdl *)calloc(1, sizeof(struct flash_mp3_hdl));
    thread_fork("flash_mp3_play_task", 10, 1024, 32, NULL, flash_mp3_play_task, NULL);
}
late_initcall(flash_mp3_open);

void post_msg_play_flash_mp3(char *file_name, u8 dec_volume)
{
    if (!play_mp3_flag || !mp3_info) {
        return;
    }
    snprintf(mp3_info->file_path, sizeof(mp3_info->file_path),
             CONFIG_VOICE_PROMPT_FILE_PATH "%s", file_name);
    mp3_info->dec_volume = dec_volume;
    os_taskq_post("play_mp3_task", 1, SHUTDOWN_CMD);
    os_taskq_post("flash_mp3_play_task", 1, mp3_info);
}

#endif
