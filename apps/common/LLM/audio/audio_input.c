#include "app_config.h"
#include "audio_input.h"
#include "system/includes.h"
#include "device/device.h"
#include "event/device_event.h"
#include "usb/host/usb_host.h"
#include "usb/host/audio.h"
#include "server/audio_server.h"
#include "server/server_core.h"
#include "generic/circular_buf.h"

#define MSG_START_NET_AUDIO_PLAY   1
#define MSG_START_LOCAL_AUDIO_PLAY   2
#define MSG_STOP_NET_AUDIO_PLAY   3
#define MSG_STOP_LOCAL_AUDIO_PLAY   4
#define MSG_START_AUDIO_RECORDER   5
#define MSG_STOP_AUDIO_RECORDER   6
#define MSG_START_AUDIO_TEST   7
#define MSG_STOP_AUDIO_TEST  8
#define MSG_AUDIO_RECORDER_FIRST_PLAY_NEXT_TEST   9
#define MSG_STOP_AUDIO_RECORDER_AND_START_PLAY_TEST   10
#define MSG_AUDIO_RECORDER_FIRST_PLAY_NEXT_STOP_PLAY_TEST   11


#define AUDIO_PLAY_VOICE_VOLUME   50
#define AUDIO_RECORD_VOICE_VOLUME 100

#ifdef AUDIO_TYPE_G711A
#define AUDIO_RECORD_VOICE_UPLORD_LEN (320)
#else
#define AUDIO_RECORD_VOICE_UPLORD_LEN (40)
#endif

#define _AUDIO_TASK_NAME    "audio_task"
#define _AUDIO_WAIT_TIMEOUT    500
#define _AUDIO_UAC_TRY_TIMES    10

#define MSG_AUDIO_RECORDER_FIRST_PLAY_NEXT_TEST_STOP   0
#define MSG_AUDIO_RECORDER_FIRST_PLAY_NEXT_TEST_START   1
#define MSG_AUDIO_RECORDER_FIRST_PLAY_NEXT_TEST_RECORDER_START   2
#define MSG_AUDIO_RECORDER_FIRST_PLAY_NEXT_TEST_PLAY_START   3

int recoder_state = 0;

typedef struct {
    CHAR_T *play_src;
    int   play_src_len;
    INT_T   play_len;
} _AUDIO_LOCAL_MSG;

typedef unsigned short WORD;

typedef struct {
    cbuffer_t pcm_cbuff_w;
    cbuffer_t pcm_cbuff_r;
    struct server *enc_server;
    struct server *dec_server;
    UCHAR_T   status;
    BOOL_T is_audio_play_open;
    BOOL_T is_audio_record_open;
} _AUDIO_HANDLE;

typedef struct {
    OS_SEM    r_sem;
    OS_QUEUE msg_que;
    BOOL_T pcm_wait_sem;
    void *task_handle;
} _AUDIO_CTRL;

typedef struct {
    unsigned int              cmd;
    UCHAR_T                  *data;
    unsigned int              data_len;
} _AUDIO_CTRL_MSG;

static _AUDIO_CTRL g_audio_ctrl = {0};
static _AUDIO_HANDLE g_audio_hdl = {0};
static _AUDIO_LOCAL_MSG  g_local_file = {0};
static unsigned int SAMPLE_RATE = 8000;//48000;//8000;
static unsigned int CHANNEL     = 1;
static unsigned int BIT_DEP     = 16;
static char audio_data[AUDIO_RECORD_VOICE_UPLORD_LEN] = {0};
static unsigned int voice_buf_size = AUDIO_RECORD_VOICE_UPLORD_LEN;
static int audio_power_off = 0;
static int audio_uac_retry_times = 0;
static unsigned int audio_test_recorder_first_play_next = 0;
static unsigned int audio_test_recorder_first_play_times = 0;


FILE *fp = NULL;
/***************************************audio*********************************************/

static int _send_audio_msg(IN CONST unsigned int msgid, IN CONST VOID *data, IN CONST unsigned int len)
{
    int op_ret = 0;
    int msg_num = 0;
    if (!&g_audio_ctrl.msg_que) {
        return -1;
    }

    _AUDIO_CTRL_MSG *msg_data;
    msg_data = malloc(sizeof(_AUDIO_CTRL_MSG) + 1);
    if (!msg_data) {
        return -1;
    }
    memset(msg_data, 0, sizeof(_AUDIO_CTRL_MSG) + 1);
    msg_data->cmd = msgid;

    if (data && len) {
        msg_data->data = malloc(len + 1);
        if (!msg_data->data) {
            if (msg_data) {
                free(msg_data);
            }
            return -1;
        }
        memset(msg_data->data, 0, len + 1);
        memcpy(msg_data->data, data, len);
        msg_data->data_len = len;
    } else {
        msg_data->data = NULL;
    }

    op_ret = os_q_post(&g_audio_ctrl.msg_que, msg_data);
    if (0 != op_ret) {
        if (msg_data->data) {
            free(msg_data->data);
        }
        if (msg_data) {
            free(msg_data);
        }
        return op_ret;
    }

    return 0;
}

int _device_get_voice_data(VOID *data, unsigned int max_len)
{
    cbuffer_t *cbuf = (cbuffer_t *)&g_audio_hdl.pcm_cbuff_w;
    unsigned int rlen;
    static flag = 1;
    mdelay(20);
    rlen = cbuf_get_data_size(cbuf);
    if (rlen == 0) {
        //    audio_debug("device_get_voice_data no data");
        return 0;
    } else {
        // audio_debug("device_get_voice_data %d",rlen);
    }

    if (voice_buf_size <= cbuf_get_data_size(cbuf)) {

        rlen = rlen > max_len ? max_len : rlen;
        cbuf_read(cbuf, data, rlen);
        // printf("get audio_data: %d\n", rlen);
        return rlen;
    }
    return 0;
}


int _device_write_voice_data(VOID *data, unsigned int len)
{
    cbuffer_t *cbuf = (cbuffer_t *)&g_audio_hdl.pcm_cbuff_r;
    int  i = 0;

    unsigned int write_len = cbuf_write(cbuf, data, len);
    // printf("len=%d wite_len=%d", len, write_len);
    if (0 == write_len) {
        //上层buf写不进去时清空一下，避免出现声音滞后的情况
        cbuf_clear(cbuf);
        //audio_debug("cbuf_write full");
    } else if (write_len != len) {
        unsigned int rlen = cbuf_get_data_size(cbuf);
        // audio_debug("wite_len %d len %d rlen %d", write_len, len, rlen);
    }

    os_sem_post(&g_audio_ctrl.r_sem);
    //此回调返回0录音就会自动停止
    return len;
}

//编码器输出PCM数据
static int recorder_vfs_fwrite(VOID *file, VOID *data, unsigned int len)
{
    static int cnt;
    int ret;
    cbuffer_t *cbuf = (cbuffer_t *)file;

    if (0 == cbuf_write(cbuf, data, len)) {
        // 上层buf写不进去时清空一下，避免出现声音滞后的情况
        cbuf_clear(cbuf);
    } else {
        // audio_debug("cbuf write %d data", len);
    }

    return len;
}

static CONST struct audio_vfs_ops recorder_vfs_ops = {
    .fwrite = recorder_vfs_fwrite,
    .fopen = 0,
    .fread = 0,
    .fseek = 0,
    .ftell = 0,
    .flen = 0,
    .fclose = 0,
};


/**
 * @berief: 初始化录音功能,录取PCM的数据,存储到PCM buff中
 * @param   int
 * @return: none
 * @retval: none
 */

static VOID audio_recoder_init()
{
    audio_debug("audio recoder init ");
    int err;
    static BOOL_T flag = FALSE;
    union audio_req req = {0};

    flag = TRUE;

    req.enc.frame_size = SAMPLE_RATE / 100 * 4 * CHANNEL;        //收集够多少字节PCM数据就回调一次fwrite
    req.enc.output_buf_len = req.enc.frame_size * 3; //底层缓冲buf至少设成3倍frame_size
    req.enc.cmd = AUDIO_ENC_OPEN;
    req.enc.channel = CHANNEL;
    req.enc.volume = AUDIO_RECORD_VOICE_VOLUME;
    req.enc.sample_rate = SAMPLE_RATE;

#ifdef AUDIO_TYPE_G711A
    req.enc.format = "pcm";
#elif defined(AUDIO_TYPE_AACLC)
    req.enc.format = "aac";
    req.enc.bitrate = SAMPLE_RATE * 4;
    req.enc.no_header = 0;
#elif defined(AUDIO_TYPE_OPUS)
    req.enc.format = "opus";
#endif

    req.enc.sample_source = "mic";
    req.enc.vfs_ops = &recorder_vfs_ops;
    req.enc.file = (FILE *)&g_audio_hdl.pcm_cbuff_w;
    req.enc.channel_bit_map = BIT(1);

    if (CHANNEL == 1 && !strcmp(req.enc.sample_source, "mic") && (SAMPLE_RATE == 8000 || SAMPLE_RATE == 16000)) {
        req.enc.use_vad = 1;                    //打开VAD断句功能
        req.enc.vad_auto_refresh = 1;   //VAD自动刷新
    }
    if (req.enc.use_vad == 1) {
        req.enc.vad_start_threshold = 0;    //ms
        req.enc.vad_stop_threshold  = 5000;    //ms
    }

#ifdef CONFIG_AEC_ENC_ENABLE
    struct aec_s_attr aec_param = {0};
    req.enc.aec_attr = &aec_param;
    req.enc.aec_enable = 1;

    extern void get_cfg_file_aec_config(struct aec_s_attr * aec_param);
    get_cfg_file_aec_config(&aec_param);

    if (aec_param.EnableBit == 0) {
        req.enc.aec_enable = 0;
        req.enc.aec_attr = NULL;
    }

#if defined CONFIG_ALL_ADC_CHANNEL_OPEN_ENABLE && defined CONFIG_AISP_LINEIN_ADC_CHANNEL && defined CONFIG_AEC_LINEIN_CHANNEL_ENABLE
    if (req.enc.aec_enable) {
        aec_param.output_way = 1;               //1:使用硬件回采 0:使用软件回采
        if (aec_param.output_way) {
            req.enc.channel_bit_map |= BIT(CONFIG_AISP_LINEIN_ADC_CHANNEL);             //配置回采硬件通道
            if (CONFIG_AISP_LINEIN_ADC_CHANNEL < CONFIG_PHONE_CALL_ADC_CHANNEL) {
                req.enc.ch_data_exchange = 1;     //如果回采通道使用的硬件channel比MIC通道使用的硬件channel靠前的话处理数据时需要交换一下顺序
            }
        }
    }
#endif

    if (req.enc.sample_rate == 16000) {
        aec_param.wideband = 1;
        aec_param.hw_delay_offset = 50;
    } else {
        aec_param.wideband = 0;
        aec_param.hw_delay_offset = 75;
    }
#endif

    err = server_request(g_audio_hdl.enc_server, AUDIO_REQ_ENC, &req);
    printf("err:%d", err);
}


static VOID _audio_recoder_stop()
{
    int err;
    union audio_req req = {0};
    req.enc.cmd             = AUDIO_ENC_STOP;
    err = server_request(g_audio_hdl.enc_server, AUDIO_REQ_ENC, &req);
    audio_debug("_audio_recoder_stop err=%d", err);

}


//解码器读取PCM数据
static int audio_play_net_vfs_fread(VOID *file, VOID *data, unsigned int len)
{

    cbuffer_t *cbuf = NULL;
    unsigned int cbuf_len = 0;
    unsigned int rlen = 0;
    unsigned int c_rlen = 0;
    cbuf = (cbuffer_t *)file;
    do {
        cbuf_len = cbuf_get_data_size(cbuf);
        // if(cbuf_len > 1280){

        // }
        // if(cbuf_len >= 8*1024) {
        if (cbuf_len >= 1280) {
            rlen = cbuf_len > len ? len : cbuf_len;
            c_rlen = cbuf_read(cbuf, data, rlen);
            // printf("cbuf_len=%d , clen=%d, len=%d", cbuf_len, c_rlen, len);
            if (c_rlen > 0) {
                //audio_debug("c_rlen=%d rlen=%d cbuf_len=%d len=%d",c_rlen,rlen,cbuf_len,len);
                break;
            }
        }

        //此处等待信号量是为了防止解码器因为读不到数而一直空转
        if (FALSE == g_audio_hdl.is_audio_play_open) {
            rlen = 0;
            break;
        }
        g_audio_ctrl.pcm_wait_sem = 1;
        os_sem_pend(&g_audio_ctrl.r_sem, _AUDIO_WAIT_TIMEOUT);
        g_audio_ctrl.pcm_wait_sem = 0;

    } while (1);

    //返回成功读取的字节数
    return rlen;
}

static int audio_vfs_fseek(void *file, u32 offset, int orig)
{
    printf("audio_vfs_fseek %d", offset);
    // __this->audio_data_offset = offset;
    return 0;
}

static CONST struct audio_vfs_ops audio_play_net_vfs_ops = {
    .fread  = audio_play_net_vfs_fread,
    .fwrite = 0,
    .fopen = 0,
    .fseek = 0, //audio_vfs_fseek,
    .ftell = 0,
    .flen = 0,
    .fclose = 0,
};

/**
 * @berief: 初始化播放功能,播放PCM的数据,从pcm_buff中读取数据播放
 * @param   int
 * @return: none
 * @retval: none
 */
static VOID audio_player_net_init()
{
    static BOOL_T flag = FALSE;
    int err;
    union audio_req req = {0};
    if (flag) {
        return;
    }
    flag = TRUE;

    req.dec.cmd             = AUDIO_DEC_OPEN;
    req.dec.volume          = AUDIO_PLAY_VOICE_VOLUME;
    req.dec.output_buf_len  = 8 * 1024;
    req.dec.priority        = 1;
    req.dec.channel         = CHANNEL;  /*dac 差分输出 单路*/
    req.dec.sample_rate     = SAMPLE_RATE;
    req.dec.vfs_ops         = &audio_play_net_vfs_ops;
#ifdef AUDIO_TYPE_G711A
    req.dec.dec_type 		= "pcm";
#elif defined(AUDIO_TYPE_AACLC)
    req.dec.dec_type 		= "aac";
#elif defined(AUDIO_TYPE_OPUS)
    req.dec.dec_type 		= "opus";
#endif
    req.dec.sample_source   = "dac";
    req.dec.file            = (FILE *)&g_audio_hdl.pcm_cbuff_r;
    audio_debug("audio_player_net_init ");
    err = server_request(g_audio_hdl.dec_server, AUDIO_REQ_DEC, &req);
    if (err) {
        audio_debug("server_request dec_server AUDIO_DEC_OPEN err %d", err);
        goto __err;
    }
    req.dec.cmd = AUDIO_DEC_START;
    server_request(g_audio_hdl.dec_server, AUDIO_REQ_DEC, &req);
    return;
__err :

    return;

}

static VOID _audio_player_stop()
{
    int err;
    union audio_req req = {0};
    req.dec.cmd             = AUDIO_DEC_STOP;
    err = server_request(g_audio_hdl.dec_server, AUDIO_REQ_DEC, &req);
    audio_debug("_audio_player_stop err=%d", err);

}

static VOID enc_server_event_handler(VOID *priv, int argc, int *argv)
{

    switch (argv[0]) {
    case AUDIO_SERVER_EVENT_ERR:
    case AUDIO_SERVER_EVENT_END:
        break;
    case AUDIO_SERVER_EVENT_SPEAK_START:
        /* printf("\n -[function] %s -[lnie] %d\n", __FUNCTION__, __LINE__); */
        printf("speak start\n");
        recoder_state = 1;
        break;
    case AUDIO_SERVER_EVENT_SPEAK_STOP:
        /* printf("\n -[function] %s -[lnie] %d\n", __FUNCTION__, __LINE__); */
        printf("speak stop\n");
        recoder_state = 0;
        break;
    default:
        break;
    }
}

//*****************************收音和播放************************
VOID _device_net_audio(BOOL_T flag)
{
    audio_debug("ty_device_net_audio %d", flag);
    if (flag) {
        //音频播放功放
        _send_audio_msg(MSG_START_NET_AUDIO_PLAY, 0, NULL);
        _send_audio_msg(MSG_START_AUDIO_RECORDER, 0, NULL);
    } else {
        _send_audio_msg(MSG_STOP_NET_AUDIO_PLAY, 0, NULL);
        _send_audio_msg(MSG_STOP_AUDIO_RECORDER, 0, NULL);
    }

}

VOID _device_net_audio_play(BOOL_T flag)
{
    audio_debug("_device_net_audio_play %d", flag);
    if (flag) {
        //音频播放功放
        _send_audio_msg(MSG_START_NET_AUDIO_PLAY, 0, NULL);
    } else {
        _send_audio_msg(MSG_STOP_NET_AUDIO_PLAY, 0, NULL);
    }

}

VOID _device_net_audio_recorder(BOOL_T flag)
{
    audio_debug("_device_net_audio_recorder %d", flag);
    if (flag) {
        //音频播放功放
        _send_audio_msg(MSG_START_AUDIO_RECORDER, 0, NULL);
    } else {
        _send_audio_msg(MSG_STOP_AUDIO_RECORDER, 0, NULL);
    }

}
//***************************************************************
BOOL_T is_audio_play_open(VOID)
{
    return g_audio_hdl.is_audio_play_open;
}

static VOID __audio_task(void *pArg)
{
    int op_ret = 0;
    _AUDIO_CTRL_MSG *msg_data;
    int msg[16] = {0,};
    while (1) {
        //阻塞等待消息
        op_ret = os_q_pend(&g_audio_ctrl.msg_que, 0, msg);
        if (op_ret != 0) {
            if (op_ret != -1) {
                audio_debug("tal_queue_fetch op_ret:%d", op_ret);
            }
            continue;
        }
        msg_data = (_AUDIO_CTRL_MSG *)msg[0];
        switch (msg_data->cmd) {
        case MSG_START_NET_AUDIO_PLAY: {
            cbuf_clear(&g_audio_hdl.pcm_cbuff_r);
            if (g_audio_hdl.is_audio_play_open) {
                _audio_player_stop();
            }
            g_audio_hdl.is_audio_play_open = TRUE;
            audio_player_net_init();
        }
        break;

        case MSG_STOP_NET_AUDIO_PLAY: {
            cbuf_clear(&g_audio_hdl.pcm_cbuff_r);
            g_audio_hdl.is_audio_play_open = FALSE;
            _audio_player_stop();
        }
        break;

        case MSG_START_AUDIO_RECORDER: {
            cbuf_clear(&g_audio_hdl.pcm_cbuff_w);
            if (g_audio_hdl.is_audio_record_open) {
                _audio_recoder_stop();
            }
            g_audio_hdl.is_audio_record_open = true;
            audio_recoder_init();
        }
        break;

        case MSG_STOP_AUDIO_RECORDER: {
            cbuf_clear(&g_audio_hdl.pcm_cbuff_w);
            g_audio_hdl.is_audio_record_open = FALSE;
            _audio_recoder_stop();
        }
        break;

        }
        if (msg_data) {
            if (msg_data->data) {
                free(msg_data->data);
            }
            free(msg_data);
            msg_data = NULL;
        }
    }
}

int audio_cfg_init(_AUDIO_PARAM *audio_param)
{
    audio_debug("into audio cfg init");
    int op_ret = -1;
    if (audio_param->sample_rate > 8000) {
        voice_buf_size = AUDIO_RECORD_VOICE_UPLORD_LEN;
    }
    audio_debug("audio param:%d %d %d %d", audio_param->sample_rate, audio_param->bit_dept, audio_param->channel_num, audio_param->audio_power_off);

    SAMPLE_RATE = audio_param->sample_rate;
    CHANNEL = audio_param->channel_num;
    BIT_DEP = audio_param->bit_dept;
    audio_power_off = audio_param->audio_power_off;
    return 0;
}

static int _audio_soft_init(VOID)
{
    int op_ret;
    u8 *pcm_buff_w = NULL;
    u8 *pcm_buff_r = NULL;
    audio_debug("into volc audio soft init");
    pcm_buff_w = malloc(SAMPLE_RATE * CHANNEL * 0.25);
    cbuf_init(&g_audio_hdl.pcm_cbuff_w, pcm_buff_w, SAMPLE_RATE * CHANNEL * 0.25);
    pcm_buff_r = malloc(SAMPLE_RATE * CHANNEL * 0.25);
    cbuf_init(&g_audio_hdl.pcm_cbuff_r, pcm_buff_r, SAMPLE_RATE * CHANNEL * 0.25);

    if (!g_audio_hdl.enc_server) {
        g_audio_hdl.enc_server = server_open("audio_server", "enc");
        if (!g_audio_hdl.enc_server) {
            op_ret = -1;
            audio_debug("server_open err:%d", op_ret);
            return op_ret;
        }
        audio_debug("enc server_open succ!");
        server_register_event_handler_to_task(g_audio_hdl.enc_server, NULL, enc_server_event_handler, "app_core");
    }

    if (!g_audio_hdl.dec_server) {
        g_audio_hdl.dec_server = server_open("audio_server", "dec");
        if (!g_audio_hdl.dec_server) {
            op_ret = -1;
            audio_debug("server_open err:%d", op_ret);
            return op_ret;
        }
        audio_debug("dec server_open succ!");
        audio_debug("enc_server 0x%x dec_server 0x%x", g_audio_hdl.enc_server, g_audio_hdl.dec_server);
    }

    op_ret = os_sem_create(&g_audio_ctrl.r_sem, 0);
    if (op_ret != 0) {
        audio_debug("_hal_semaphore_create_init create semphore err:%d", op_ret);
        return op_ret;
    }

    QS queue_size = (sizeof(_AUDIO_CTRL_MSG *) * 20 + sizeof(WORD) - 1) / sizeof(WORD);
    op_ret = os_q_create(&g_audio_ctrl.msg_que, queue_size);

    thread_fork(_AUDIO_TASK_NAME, 5, 2 * 1024, 0, g_audio_ctrl.task_handle, __audio_task, NULL);

    audio_debug("_audio_task create");
    return op_ret;
}

VOID _audio_init(_AUDIO_PARAM *audio_param)
{
    audio_debug("into volc audio inti!");
    static char init = 0;

    audio_cfg_init(audio_param);
    if (!init) {
        _audio_soft_init();
    }

    init = 1;
}

void audio_stream_init(int sample_rate, int bit_dept, int channel_num)
{
    _AUDIO_PARAM audio_param;
    audio_param.audio_power_off = false;
    audio_param.bit_dept = bit_dept;
    audio_param.channel_num = channel_num;
    audio_param.sample_rate = sample_rate;
    _audio_init(&audio_param);
}

void start_audio_stream(void)
{
    _device_net_audio(true);
}

void stop_audio_stream(void)
{
    _device_net_audio(false);
}

int get_recoder_state()
{
    return recoder_state;
}

