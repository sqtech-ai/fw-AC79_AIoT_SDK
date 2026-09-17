//
//  IOTSdk.h
//
#ifndef _IOT_SDK_H_
#define _IOT_SDK_H_

#include <string>
#include <memory>
#include "IOTBoard.h"

/*------------------------- 错误码定义 -------------------------*/
/**
 * 成功
 */
#define IOTSDK_RET_OK 0
/**
 * 未知错误
 */
#define IOTSDK_RET_ERROR -1
/**
 * 无效的参数
 */
#define IOTSDK_RET_INVALID_PARAMS -2
/**
 * 资源未就绪(如：初始化不成功，租户信息缺失)
 */
#define IOTSDK_RET_RESOURCE_NOT_READY -3
/**
 * 请求发生异常(如：返回数据异常)
 */
#define IOTSDK_RET_REQUEST_EXCEPTION -4
/**
 * 请求不支持/无效的请求
 */
#define IOTSDK_RET_INVALID_REQUEST -5
/**
 * 请求超时
 */
#define IOTSDK_RET_TIMEOUT -6
/**
 * 设备ID为空
 */
#define IOTSDK_RET_INVALID_DEVICE_ID -7
/**
 * 不可播放(如：版权问题，无法播放)
 */
#define IOTSDK_RET_UNPLAYABLE -8
/**
 * HTTP请求失败(如：网络异常，读写超时等)
 */
#define IOTSDK_RET_HTTP_ERROR -9

/*------------------------- 内容提供商定义 -------------------------*/
/**
 * 内容提供商：咪咕
 */
#define IOTSDK_PROVIDER_MIGU "migu"
/**
 * 内容提供商：喜马拉雅
 */
#define IOTSDK_PROVIDER_XMLY "xmly"

/*------------------------- HTTP连接意图定义 -------------------------*/
/**
 * 咪咕HTTPS API接口
 */
#define IOTSDK_MIGU_HTTPS 1
/**
 * 咪咕HTTPS 音乐流
 */
#define IOTSDK_MIGU_MUSIC 2
/**
 * IOTSdk OpenAPI接口
 */
#define IOTSDK_OPENAPI_HTTPS 3

class IOTSdk
{
public:
    /**
     * 获取单例
     */
    static IOTSdk& Singleton();
    /**
     * 初始化
     * @param workspace 工作目录
     * @param args 初始化参数(json字符串)
     * deviceId: 设备唯一标识(必填)
     * appLicenseId: 许可证id(必填)
     * appKey: 区域编码(必填)
     * serverToken: 服务端token(必填)
     * regionCode: 区域编码(必填)
     * servicePackageCode: 服务套餐码(必填)
     * username: 认证用户名(必填)
     * password: 认证密码(必填)
     * xmlySk: 喜马拉雅授权密钥
     * env: 环境选择。生产环境(prod)，测试环境(test)
     * mqttMode: MQTT连接方式(0~3)
     * @return 0 成功，其他失败
     */
    virtual int Init(const std::string& workspace, const std::string& args) = 0;
    /**
     * 设置获取板载MQTT客户端回调(可选)
     * 1.必须在IOTSdk::Init()之前注册
     * 2.BoardMqtt实现可以参考EspBoard.cpp
     * 3.如果是板载4G模组，必须调用该函数设置回调，否则无法使用网络功能
     */
     virtual void SetOnGetBoardMqtt(std::function<std::unique_ptr<BoardMqtt>()> on_get_board_mqtt) = 0;
    /**
     * 设置获取板载HTTP客户端回调
     * 特别注意：
     * 1.必须在IOTSdk::Init()之前注册
     * 2.BoardHttp实现可以参考EspBoard.cpp
     * 3.如果是板载4G模组，必须调用该函数设置回调，否则无法使用网络功能
     * 4.如果是板载WiFi模组，不建议调用该函数设置回调，更节省内存，性能更优
     */
     virtual void SetOnGetBoardHttp(std::function<std::unique_ptr<BoardHttp>(int action)> on_get_board_http) = 0;
    /**
     * 合并内容搜索（喜马拉雅 + 咪咕）
     * @param input 输入参数(json字符串)
     * 通用:
     *      text: 搜索内容(必填)
     *      providerOrder: 内容供应商搜索顺序(可选，string 数组)；元素为 "xmly" / "migu"，
     *          按数组先后决定服务端编排优先级。示例：["xmly","migu"] 先喜马后咪咕；
     *          ["migu"] 仅搜咪咕。未传时由服务端默认编排（通常先喜马后咪咕）
     * 喜马扩展(可选，进入 xmly 分区):
     *      intent: 意图，listen_audiobook-有声书(默认)、listen_music-音乐
     *      position: 有声书集数；未传时可用 pageIndex 映射
     *      isRecommend: 未搜到时是否返回推荐(boolean，默认 false)
     * 咪咕扩展(可选，进入 migu 分区):
     *      type: 搜索目的(可选)；默认为1（1-歌曲 2-专辑 3-歌手 4-标签下歌曲 5-无维度 6-联想 7-歌手下单曲专辑MV 8-歌词）
     *      pageIndex: 当前页(起始页为1)，可选；亦可用于映射喜马 position
     *      pageSize: 每页条数，[0-50]，可选
     *      searchType: 搜索类型：type 为 1 或 5 时使用（1-智能 2-关键词 3-歌手下歌曲 4-指定范围搜索等，见文档）
     *      issemantic: 语义判定：type 为 1/2/3/4 时有效（1-语义 0-否）
     *      isCorrect: 容错：type 为 1/2/3/4/5 时有效（0-关 1-开）
     *      searchRange: 对象，type=1 且 searchType=4 时精确指定搜索范围（意图），见接口文档
     * @param output 返回结果(json字符串)
     * code / message: 平台统一业务码与消息
     * data: 命中侧结果
     *      provider: 内容来源，"xmly" 或 "migu"
     *      searchSong: 歌曲列表（数组）；喜马结果已字段对齐为咪咕形态
     *          musicId: 歌曲ID
     *          musicName: 歌曲名称
     *          singerName: 歌手名称
     *          listenUrl: 试听地址
     *          picUrl: 封面下载地址
     *          lrcUrl: 歌词下载地址
     *          length: 歌曲时长（格式：HH:mm:ss）
     *          （喜马侧由 trackId/albumName/artists/playUrl/coverUrl/duration 等转换）
     * @param timeout 超时时长，单位ms
     * @return 0 成功，其他失败
     */
    virtual int Search(const std::string& input, std::string& output, int timeout = 10000) = 0;
    /**
     * 用户听歌记录上报
     * @param input 输入参数(json字符串)
     * provider: 内容供应商(必填)
     * 咪咕要求参数:
     *      contentId: 歌曲ID(必填)
     *      toneQuality: 音质。1：标清；2：高清；3：无损
     *      runningTime: 播放时长(毫秒)
     *      startTime: 播放开始时间  格式yyyymmdd HHMMSS.mmmmmm
     *      stopTime: 播放停止时间  格式yyyymmdd HHMMSS.mmmmmm
     *      userId: 用户ID（合作伙伴自定义，不校验）
     *  喜马拉雅要求参数:
     *      trackId(String): 声音Id, 对应1.5.1接口返回的trackId
     *      duration(int): 播放时长, 即本次播放总共播放了多长时间, 单位为秒
     *      playedSecs(int): 播放到第几秒或最后播放到的位置, 是相对于这个音频开始位置的一个值
     *      startedAt(long): 播放开始时刻, Unix毫秒数时间戳
     * @param output 返回结果(json字符串)
     * @param timeout 超时时长，单位ms
     * @return 0 成功，其他失败
     */    
    virtual int Report(const std::string& input, std::string& output, int timeout = 10000) = 0;
    /**
     * 根据歌曲ID查询歌曲信息
     * @param input 输入参数(json字符串)
     * provider: 内容供应商(必填, 仅支持migu)
     * musicId: 歌曲ID(必填, 支持11位版权ID或18位内容ID)
     * picSize: 图片尺寸(可选, L-大图，S-小图，M-中图，默认M)
     * @param output 返回结果(json字符串)
     *   musicInfo:
     *      musicId: 歌曲ID（版权ID）
     *      musicName: 歌曲名称
     *      bpm: 歌曲步频
     *      singerName: 歌手名称
     *      albumNames: 专辑名称列表（最多返回2个）
     *      songAuthorName: 曲作者
     *      lyricAuthorName: 词作者
     *      length: 歌曲时长（格式：HH:mm:ss）
     *      language: 歌曲语种
     *      picUrl: 歌曲封面图URL
     *      listenUrl: 试听地址（标清格式）
     *      hqListenUrl: 试听地址（高清格式）
     *      sqListenUrl: 试听地址（无损格式）
     *      lrcUrl: 歌词地址
     *      isCollection: 是否收藏（1-已收藏，0-未收藏）
     *      isCpAuth: 是否CP授权（1-已授权，0-未授权）
     *      singerId: 歌手ID
     *      musicSource: 歌曲来源类型（1-咪咕善跑APP，2-咪咕音乐APP）
     *      auditionsFlag: 试听标签
     *      VIP: VIP标识（0-CP未授权，1-普通歌曲，2-白金会员歌曲，3-敏感库歌曲，4-数字专辑，5-单曲按次）
     *      contentId: 内容ID
     *      isUserCollection: 用户是否收藏（1-收藏，0-未收藏）
     *      listenFlag: 试听标识（0-不可以试听，1-可以试听）
     * @param timeout 超时时长，单位ms
     * @return 0 成功，其他失败
     */
    virtual int GetMusicInfo(const std::string& input, std::string& output, int timeout = 10000) = 0;
    /**
     * 查询音乐歌单（咪咕）
     * @param input 输入参数(json字符串)，可为空或 "{}"
     * typeId: 歌单分类(可选)；"1"-运动歌单、"2"-咪咕歌单；未传时 SDK 默认补 "2"
     * @param output 返回结果(json字符串)
     * code / message: 平台统一业务码与消息（HTTP 成功多为 "1"；MQTT 成功多为 "001000"/1000）
     * data: MusicSheetInfo 数组（HTTP 由 data.musicSheetInfos 提升为 data；MQTT 为 extendParam 列表）
     *      musicSheetId: 歌单 ID
     *      typeId: 歌单分类 ID
     *      title: 歌单名称
     *      description: 描述
     *      ImgUrl: 图片地址
     *      smallImgUrl: 缩略图
     *      auditionCount: 歌单试听次数
     *      minBpm / maxBpm: 最小/最大节拍
     *      isCollection: 是否收藏（1-已收藏，0-未收藏）
     *      musicCount: 歌单下歌曲总数
     *      musicSheetType: 歌单类型（1-咪咕善跑，2-咪咕音乐）
     *      length: 歌单总时长（typeId=1 时常见，否则可能缺省）
     * @param timeout 超时时长，单位ms
     * @return 0 成功，其他失败
     */
    virtual int QueryMusicSheet(const std::string& input, std::string& output, int timeout = 10000) = 0;
    /**
     * 查询音乐歌单下歌曲（咪咕）
     * @param input 输入参数(json字符串)
     * musicSheetId: 音乐歌单 ID(必填)
     * startNum: 分页起始位(可选，服务端默认 0)
     * endNum: 分页结束位(可选，服务端默认 20)
     * picSize: 图片尺寸(可选, L/S/M)；未传时 SDK 默认补 "S"
     * uid: 用户 id(可选，写入咪咕 Authorization)
     * msisdn: 用户手机号(可选，善跑引流)
     * @param output 返回结果(json字符串)
     * code / message: 平台统一业务码与消息（HTTP 成功多为 "1"；MQTT 成功多为 "001000"/1000）
     * data: MusicInfo 数组（HTTP 由 data.musicInfos 提升为 data；MQTT 为 extendParam 列表；不含上游 count 信封）
     *      musicId / musicName / singerName / picUrl
     *      listenUrl / hqListenUrl / sqListenUrl / lrcUrl
     *      isCollection / isCpAuth / VIP / auditionsFlag / contentId
     *      songAuthorName / lyricAuthorName / albumNames / length / language
     *      singerId / musicSource / bmp(BPM) 等（有则下发）
     * @param timeout 超时时长，单位ms
     * @return 0 成功，其他失败
     */
    virtual int QueryMusicSheetMusic(const std::string& input, std::string& output, int timeout = 10000) = 0;
};

#endif//_IOT_SDK_H_
