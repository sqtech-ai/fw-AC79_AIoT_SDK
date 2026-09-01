# IOTSdk 对接接口说明

本文档依据源码头文件 [`IOTSdk.h`](../IOTSdk.h) 整理，供第三方在集成 **IOTSdk** 时对照使用。入参、出参均为 **JSON 字符串**。

> **sdk-3.2：** HTTP 模式新增传输层失败码 `IOTSDK_RET_HTTP_ERROR`（-9）及可配置重试（`retryCount`，默认 3 次，含首次调用）；`Init` / `Search` / `Report` / `GetMusicInfo` / `QueryMusicSheet` / `QueryMusicSheetMusic` 在 HTTP 模式下均适用。  
> **sdk-3.0：** 对外保留 `Init` / `SetOnGetBoardMqtt` / `SetOnGetBoardHttp` / `Search` / `Report` / `GetMusicInfo` / `QueryMusicSheet` / `QueryMusicSheetMusic`。已移除的 `SearchPlay` / `SearchByKey` / `SearchSongEx` 等接口不再提供。

---

## 1. 错误码宏

| 宏 | 值 | 含义 |
|----|-----|------|
| `IOTSDK_RET_OK` | `0` | 成功 |
| `IOTSDK_RET_ERROR` | `-1` | 未知错误 |
| `IOTSDK_RET_INVALID_PARAMS` | `-2` | 无效参数 |
| `IOTSDK_RET_RESOURCE_NOT_READY` | `-3` | 资源未就绪 |
| `IOTSDK_RET_REQUEST_EXCEPTION` | `-4` | 请求异常 |
| `IOTSDK_RET_INVALID_REQUEST` | `-5` | 请求不支持或无效 |
| `IOTSDK_RET_TIMEOUT` | `-6` | 请求超时 |
| `IOTSDK_RET_INVALID_DEVICE_ID` | `-7` | 设备 ID 为空 |
| `IOTSDK_RET_UNPLAYABLE` | `-8` | 不可播放（`VIP`、`isCpAuth` 不满足播放条件） |
| `IOTSDK_RET_HTTP_ERROR` | `-9` | HTTP 传输失败（网络异常、连接超时、HTTP 状态非 200、空响应等） |

接口注释中“`@return 0 成功，其他失败`”与上述错误码约定一致；具体失败时返回哪一个负值，以实现为准。

**HTTP 模式下的两类失败：**

| 返回码 | 层级 | 典型场景 | 是否自动重试 |
|--------|------|----------|--------------|
| `IOTSDK_RET_HTTP_ERROR` | 传输层 | 网络断开、超时、HTTP 4xx/5xx、响应体为空 | **是**（见第 4.1 节 `retryCount`） |
| `IOTSDK_RET_REQUEST_EXCEPTION` | 业务层 | HTTP 200 但 `code != 1`、JSON 结构异常 | **否** |

`IOTSDK_RET_UNPLAYABLE` 常见于调用 `GetMusicInfo` 成功后校验曲目可播性：`musicInfo` 中 `isCpAuth`、`VIP` 须均为 `"1"`，且存在有效 `listenUrl`；否则视为不可播放（参见 `MusicInfo::Playable`）。

---

## 2. 内容提供商（provider）

| 宏 | 字符串值 | 说明 |
|----|-----------|------|
| `IOTSDK_PROVIDER_MIGU` | `"migu"` | 咪咕 |
| `IOTSDK_PROVIDER_XMLY` | `"xmly"` | 喜马拉雅 |

业务 JSON 中的 `provider` 字段应使用上表中的字符串值（可直接使用宏对应的字面量）。

---

## 3. HTTP 连接意图（action）

通过 `SetOnGetBoardHttp` 注册板载 HTTP 工厂时（**4G 板卡必选，WiFi 板卡通常省略**），SDK 会按 **action** 区分连接用途；回调须返回 [`BoardHttp`](../IOTBoard.h) 实现实例。

| 宏 | 值 | 说明 |
|----|-----|------|
| `IOTSDK_MIGU_HTTPS` | `1` | 咪咕 HTTPS API / 封面 / 歌词等短请求 |
| `IOTSDK_MIGU_MUSIC` | `2` | 咪咕音乐流拉流 |
| `IOTSDK_OPENAPI_HTTPS` | `3` | IOTSdk OpenAPI 接口 |

实现侧示例：`IOTHttpClient_board` 在访问咪咕 API 时使用 `IOTSDK_MIGU_HTTPS`，访问 OpenAPI 时使用 `IOTSDK_OPENAPI_HTTPS`；`EspMusicPlayer` / `EspMusicPlayerEx` 封面与歌词使用 `IOTSDK_MIGU_HTTPS`，拉流使用 `IOTSDK_MIGU_MUSIC`。

**板载参考实现（[`EspBoard.cpp`](../esp32/EspBoard.cpp)）：** `BoardHttpImpl(action)` 内部调用 `CreateHttp` 时，`IOTSDK_MIGU_MUSIC` 使用连接参数 `2`，其余 action 使用 `1`，便于与模组多 HTTP 槽位配合（音乐流与短请求分槽）。

---

## 4. C++ 接口：`class IOTSdk`

单例获取：

```cpp
IOTSdk& IOTSdk::Singleton();
```

以下方法均在 `IOTSdk` 上调用。板载类型为 [`BoardHttp`](../IOTBoard.h) / [`BoardMqtt`](../IOTBoard.h)（由 `IOTSdk.h` 通过 `#include "IOTBoard.h"` 引入）。

### 4.1 `Init`

```cpp
virtual int Init(const std::string& workspace, const std::string& args) = 0;
```

| 参数 | 类型 | 说明 |
|------|------|------|
| `workspace` | `std::string` | 工作目录 |
| `args` | `std::string` | 初始化参数，**JSON 字符串** |

**`args` JSON 字段：**

| 字段 | 必填 | 说明 |
|------|------|------|
| `deviceId` | 是 | 设备唯一标识（通常为 WiFi STA 的 MAC 地址，格式如 `88:56:a6:e8:88:60`） |
| `appLicenseId` | 是 | 许可证 ID |
| `appKey` | 是 | 应用 Key |
| `serverToken` | 是 | 服务端 token |
| `regionCode` | 是 | 区域编码 |
| `servicePackageCode` | 是 | 服务套餐码 |
| `username` | 条件 | MQTT 认证用户名；与 `password` **同时非空** 时走 **MQTT 模式** |
| `password` | 条件 | MQTT 认证密码；缺一则走 **HTTP 模式**（OpenAPI） |
| `xmlySk` | 否 | 喜马拉雅 URL 解密密钥（AES）；搜到喜马结果时用于解密 `playUrl` |
| `env` | 否 | 环境：`prod`（生产）、其它/缺省为测试 |
| `mqttMode` | 否 | MQTT/WS 地址选型，`0~3`，默认 `0`（见下表） |
| `retryCount` | 否 | HTTP 模式请求重试次数，**含首次调用**；正整数，缺省 **`3`** |

**`mqttMode` 与地址映射（`env=prod` / 测试各有一套 host）：**

| `mqttMode` | 协议/形态 |
|------------|-----------|
| `0` | `mqtts://…` |
| `1` | `wss://…/api/v1/mcp` |
| `2` | `mqtt://…` |
| `3` | `ws://…/api/v1/mcp` |

**通信模式选择（实现行为）：**

| 条件 | 模式 | 说明 |
|------|------|------|
| `username` 与 `password` 均非空 | MQTT | `InitMqtt`；业务 `Search` / `Report` / `GetMusicInfo` / `QueryMusicSheet` / `QueryMusicSheetMusic` 走 MQTT 路径 |
| 否则 | HTTP | `InitHttp`；业务走 OpenAPI / 咪咕 HTTP |

**HTTP 模式重试（`retryCount`）：**

- 适用接口：`Init`（`InitHttp`）、`Search`、`Report`、`GetMusicInfo`、`QueryMusicSheet`、`QueryMusicSheetMusic`。
- 仅当底层 HTTP 返回 **`IOTSDK_RET_HTTP_ERROR`** 时重试；参数错误、资源未就绪、业务异常（`IOTSDK_RET_REQUEST_EXCEPTION`）**不重试**。
- 每次重试前清空 `output`（`Init` 无 output，仅重调 `InitHttp`）。
- 最坏耗时约为 **`retryCount × timeout`**（`Init` 固定单次超时 10s）；调用方总超时需留足余量。

4G 板卡在 MQTT 模式下通常还需 `SetOnGetBoardMqtt` + `SetOnGetBoardHttp`（均在 `Init` 之前注册）。

**`deviceId` 获取指引：**

1. **小智设备**：可从设备串口日志中获取 WiFi STA 模式的 MAC 地址，作为 `deviceId`。日志示例：

   ```
   I (615) wifi:mode : sta (88:56:a6:e8:88:60)
   ```

   其中括号内的 `88:56:a6:e8:88:60` 即为设备唯一标识，填写到 `args` 的 `deviceId` 字段。

**返回值：** `0` 表示成功，非 `0` 表示失败（见第 1 节错误码）。HTTP 模式下传输失败可能返回 `IOTSDK_RET_HTTP_ERROR`（已按 `retryCount` 重试后仍失败）。

---

### 4.2 `SetOnGetBoardMqtt`

```cpp
virtual void SetOnGetBoardMqtt(
    std::function<std::unique_ptr<BoardMqtt>()> on_get_board_mqtt) = 0;
```

| 参数 | 类型 | 说明 |
|------|------|------|
| `on_get_board_mqtt` | 回调 | 创建板载 `BoardMqtt` 实例；返回 `nullptr` 表示无法提供 |

**说明：**

1. 须在 **`Init()` 之前** 注册（若需要）。
2. `BoardMqtt` 实现可参考 [`EspBoard.cpp`](../esp32/EspBoard.cpp)（`BoardMqttImpl` 包装板载 `CreateMqtt`）。
3. **板载 4G 模组**且使用 **MQTT 模式**时：**必须**设置该回调，否则无法建立板载 MQTT。
4. WiFi + 内建 MQTT 客户端时可不注册（由 SDK 内部实现承接）。

**回调示例（4G + MQTT）：**

```cpp
IOTSdk::Singleton().SetOnGetBoardMqtt([]() -> std::unique_ptr<BoardMqtt> {
    return std::make_unique<BoardMqttImpl>();
});
IOTSdk::Singleton().SetOnGetBoardHttp([](int action) -> std::unique_ptr<BoardHttp> {
    return std::make_unique<BoardHttpImpl>(action);
});
IOTSdk::Singleton().Init(workspace, mqtt_args_json);  // args 含 username/password
```

---

### 4.3 `SetOnGetBoardHttp`

```cpp
virtual void SetOnGetBoardHttp(
    std::function<std::unique_ptr<BoardHttp>(int action)> on_get_board_http) = 0;
```

| 参数 | 类型 | 说明 |
|------|------|------|
| `on_get_board_http` | 回调 | 按 **action**（见第 3 节）创建板载 `BoardHttp` 实例；返回 `nullptr` 表示无法提供实例 |

**说明：**

1. 须在 **`Init()` 之前** 注册（若需要）。
2. `BoardHttp` 实现可参考 [`EspBoard.cpp`](../esp32/EspBoard.cpp)（`BoardHttpImpl`）。
3. **板载 4G 模组**：**必须**调用该函数设置回调，否则无法使用网络功能。
4. **板载 WiFi 模组**：**不建议**调用该函数设置回调，更节省内存、性能更优。

若已注册该回调，`EspMusicPlayer` / `EspMusicPlayerEx` 封面、歌词、音乐拉流均通过该回调按 `action` 创建 `BoardHttp` 实例。

**回调示例（4G 板卡）：**

```cpp
IOTSdk::Singleton().SetOnGetBoardHttp([](int action) -> std::unique_ptr<BoardHttp> {
    return std::make_unique<BoardHttpImpl>(action);
});
IOTSdk::Singleton().Init(workspace, init_args_json);
```

---

### 4.4 `Search`

喜马拉雅 + 咪咕**合并内容搜索**。调用 OpenAPI `/openapi/iot/content/search`（HTTP 模式）或等价 MQTT 请求：服务端按 `providerOrder` 编排供应商搜索顺序。SDK 将顶层扁平入参拆分为 `xmly` / `migu` 分区提交，再把命中分区提升为统一 `data` 结构返回。

详细字段与编排逻辑可参见 [`喜马拉雅+咪咕合并内容查询-接口设计.md`](./喜马拉雅+咪咕合并内容查询-接口设计.md)。

```cpp
virtual int Search(const std::string& input, std::string& output, int timeout = 10000) = 0;
```

| 参数 | 类型 | 说明 |
|------|------|------|
| `input` | `std::string` | 请求参数，**JSON 字符串**（扁平传入，勿再嵌套 `xmly`/`migu`；由 SDK 内部组装） |
| `output` | `std::string&` | 返回结果，**JSON 字符串**（由实现写入） |
| `timeout` | `int` | 超时时间，单位 **毫秒**，默认 `10000` |

**`input` JSON 字段：**

**通用：**

| 字段 | 必填 | 说明 |
|------|------|------|
| `text` | 是 | 搜索内容；映射为咪咕 `migu.text`，同时作为喜马 `xmly.content`；当 `type=1` 且 `searchType=3`，或 `type=7` 时，`text` 为歌手 id |
| `providerOrder` | 否 | 内容供应商**搜索顺序**，`string` 数组；元素为 `"xmly"` / `"migu"`。示例：`["xmly","migu"]` 先喜马后咪咕；`["migu"]` 仅搜咪咕。**SDK 未传时默认注入 `["migu"]`（仅搜咪咕）** |

**喜马扩展（可选，进入 `xmly` 分区）：**

| 字段 | 必填 | 说明 |
|------|------|------|
| `intent` | 否 | 意图：`listen_audiobook`（有声书，**默认**）、`listen_music`（音乐） |
| `position` | 否 | 有声书集数；未传时可用 `pageIndex` 映射 |
| `isRecommend` | 否 | 未搜到时是否返回推荐（boolean，默认 `false`） |

**咪咕扩展（可选，进入 `migu` 分区）：**

| 字段 | 必填 | 说明 |
|------|------|------|
| `type` | 否 | 搜索目的，默认 `1`：`1` 歌曲、`2` 专辑、`3` 歌手、`4` 标签下歌曲、`5` 无维度、`6` 联想、`7` 歌手下单曲/专辑/MV、`8` 歌词 |
| `pageIndex` | 否 | 当前页（起始为 1）；亦可用于映射喜马 `position` |
| `pageSize` | 否 | 每页条数，`[0-50]` |
| `searchType` | 否 | `type` 为 `1` 或 `5` 时有效：`1` 智能、`2` 关键词、`3` 歌手下歌曲、`4` 指定范围搜索等 |
| `issemantic` | 否 | `type` 为 `1/2/3/4` 时有效：`1` 语义、`0` 否 |
| `isCorrect` | 否 | `type` 为 `1/2/3/4/5` 时有效：`0` 关、`1` 开 |
| `searchRange` | 否 | JSON **对象**，`type=1` 且 `searchType=4` 时精确搜索范围（意图） |

**`output` JSON：**

- `code` / `message`：平台统一业务码与消息
- `data`：命中侧结果（已从服务端的 `data.xmly` 或 `data.migu` 提升为顶层 `data`）
  - `provider`：内容来源，`"xmly"` 或 `"migu"`
  - `searchSong`：歌曲列表（数组）；喜马结果已字段对齐为咪咕形态
    - `musicId`：歌曲 ID
    - `musicName`：歌曲名称
    - `singerName`：歌手名称
    - `listenUrl`：试听地址
    - `picUrl`：封面下载地址
    - `lrcUrl`：歌词下载地址
    - `length`：歌曲时长（格式：`HH:mm:ss`）
    - 喜马侧由 `trackId` / `albumName` / `artists` / `playUrl` / `coverUrl` / `duration` 等转换而来

**示例入参：**

```json
{
  "text": "稻香",
  "providerOrder": ["xmly", "migu"],
  "intent": "listen_music",
  "type": 1,
  "pageIndex": 1,
  "pageSize": 10,
  "searchType": 4,
  "searchRange": {
    "songName": ["稻香"],
    "singerName": ["周杰伦"]
  }
}
```

仅搜咪咕示例：

```json
{
  "text": "稻香",
  "providerOrder": ["migu"],
  "type": 1,
  "pageIndex": 1,
  "pageSize": 10
}
```

**返回值：** `0` 成功，其他失败。HTTP 模式下传输失败返回 `IOTSDK_RET_HTTP_ERROR`（可重试）；业务失败返回 `IOTSDK_RET_REQUEST_EXCEPTION` 等。

---

### 4.5 `Report`

```cpp
virtual int Report(const std::string& input, std::string& output, int timeout = 10000) = 0;
```

| 参数 | 类型 | 说明 |
|------|------|------|
| `input` | `std::string` | **JSON 字符串** |
| `output` | `std::string&` | **JSON 字符串** |
| `timeout` | `int` | 超时（毫秒），默认 `10000` |

**`input` 公共字段：**

| 字段 | 必填 | 说明 |
|------|------|------|
| `provider` | 是 | 内容供应商（头文件约定；HTTP 实现路径当前按咪咕听歌上报处理） |

**咪咕（migu）侧字段：**

| 字段 | 必填 | 说明 |
|------|------|------|
| `contentId` | 是 | 歌曲 ID |
| `toneQuality` | 否 | 音质：`1` 标清，`2` 高清，`3` 无损 |
| `runningTime` | 否 | 播放时长（毫秒） |
| `startTime` | 否 | 开始时间，格式 `yyyymmdd HHMMSS.mmmmmm` |
| `stopTime` | 否 | 停止时间，格式同上 |
| `userId` | 否 | 用户 ID（合作方自定义，不校验） |

**喜马拉雅（xmly）侧字段（头文件约定）：**

| 字段 | 必填 | 说明 |
|------|------|------|
| `trackId` | 是 | 声音 Id（对应搜索结果中的 `trackId`） |
| `duration` | 否 | 本次播放总时长（秒） |
| `playedSecs` | 否 | 播放到第几秒 / 相对音频起点的位置（秒） |
| `startedAt` | 否 | 播放开始时刻（Unix 毫秒时间戳） |

**说明：** HTTP 模式下 `Report` 走咪咕听歌上报接口，要求有效 `contentId`。MQTT 模式下上报路径以实现为准（当前实现可能尚未完整打通 xmly 上报）。

**返回值：** `0` 成功，其他失败。HTTP 模式下传输失败返回 `IOTSDK_RET_HTTP_ERROR`（可重试）。

---

### 4.6 `GetMusicInfo`

```cpp
virtual int GetMusicInfo(const std::string& input, std::string& output, int timeout = 10000) = 0;
```

| 参数 | 类型 | 说明 |
|------|------|------|
| `input` | `std::string` | **JSON 字符串** |
| `output` | `std::string&` | **JSON 字符串** |
| `timeout` | `int` | 超时（毫秒），默认 `10000` |

**`input` JSON 字段：**

| 字段 | 必填 | 说明 |
|------|------|------|
| `provider` | 是 | 仅支持 **`migu`** |
| `musicId` | 是 | 歌曲 ID（支持 11 位版权 ID 或 18 位内容 ID） |
| `picSize` | 否 | 封面尺寸：`L` / `S` / `M`，默认 `M` |

**`output` 中 `musicInfo` 字段：** 与头文件中咪咕 MusicInfo 描述一致（`musicId`, `musicName`, `bpm`, `singerName`, `albumNames`, `songAuthorName`, `lyricAuthorName`, `length`, `language`, `picUrl`, 各清晰度试听 URL、`lrcUrl`, `isCollection`, `isCpAuth`, `singerId`, `musicSource`, `auditionsFlag`, `VIP`, `contentId`, `isUserCollection`, `listenFlag` 等）。

**可播性：** 集成方若需判断曲目是否可播，可校验 `isCpAuth`、`VIP` 均为 `"1"` 且 `listenUrl` 有效；不满足时上层可能返回 `IOTSDK_RET_UNPLAYABLE`（见第 1 节）。

**返回值：** `0` 成功，其他失败。HTTP 模式下传输失败返回 `IOTSDK_RET_HTTP_ERROR`（可重试）。

---

### 4.7 `QueryMusicSheet`

查询音乐歌单（咪咕）。对应 OpenAPI：`/openapi/iot/migu/musicSheet/query`；MQTT `action=queryMusicSheet`。

```cpp
virtual int QueryMusicSheet(const std::string& input, std::string& output, int timeout = 10000) = 0;
```

| 参数 | 类型 | 说明 |
|------|------|------|
| `input` | `std::string` | **JSON 字符串**，可为空或 `"{}"` |
| `output` | `std::string&` | **JSON 字符串** |
| `timeout` | `int` | 超时（毫秒），默认 `10000` |

**`input` JSON 字段：**

| 字段 | 必填 | 说明 |
|------|------|------|
| `typeId` | 否 | 歌单分类：`"1"` 运动歌单、`"2"` 咪咕歌单；未传时 SDK 默认补 `"2"` |

**`output` JSON 字段：**

| 字段 | 说明 |
|------|------|
| `code` / `message` | 平台统一业务码与消息（HTTP 成功多为 `"1"`；MQTT 成功多为 `"001000"` / `1000`） |
| `data` | `MusicSheetInfo` 数组（HTTP 由 `data.musicSheetInfos` 提升为 `data`；MQTT 为 `extendParam` 列表） |

**`data[]`（MusicSheetInfo）常见字段：**

| 字段 | 说明 |
|------|------|
| `musicSheetId` | 歌单 ID |
| `typeId` | 歌单分类 ID |
| `title` | 歌单名称 |
| `description` | 描述 |
| `ImgUrl` | 图片地址 |
| `smallImgUrl` | 缩略图 |
| `auditionCount` | 歌单试听次数 |
| `minBpm` / `maxBpm` | 最小 / 最大节拍 |
| `isCollection` | 是否收藏（`1` 已收藏，`0` 未收藏） |
| `musicCount` | 歌单下歌曲总数 |
| `musicSheetType` | 歌单类型（`1` 咪咕善跑，`2` 咪咕音乐） |
| `length` | 歌单总时长（`typeId=1` 时常见，否则可能缺省） |

**返回值：** `0` 成功，其他失败。HTTP 模式下传输失败返回 `IOTSDK_RET_HTTP_ERROR`（可重试）。

---

### 4.8 `QueryMusicSheetMusic`

查询音乐歌单下歌曲（咪咕）。对应 OpenAPI：`/openapi/iot/migu/musicSheet/music/query`；MQTT `action=queryMusicSheetMusic`。

```cpp
virtual int QueryMusicSheetMusic(const std::string& input, std::string& output, int timeout = 10000) = 0;
```

| 参数 | 类型 | 说明 |
|------|------|------|
| `input` | `std::string` | **JSON 字符串** |
| `output` | `std::string&` | **JSON 字符串** |
| `timeout` | `int` | 超时（毫秒），默认 `10000` |

**`input` JSON 字段：**

| 字段 | 必填 | 说明 |
|------|------|------|
| `musicSheetId` | 是 | 音乐歌单 ID |
| `startNum` | 否 | 分页起始位（服务端默认 `0`） |
| `endNum` | 否 | 分页结束位（服务端默认 `20`） |
| `picSize` | 否 | 图片尺寸：`L` / `S` / `M`；未传时 SDK 默认补 `"S"` |
| `uid` | 否 | 用户 id（写入咪咕 Authorization） |
| `msisdn` | 否 | 用户手机号（善跑引流） |

**`output` JSON 字段：**

| 字段 | 说明 |
|------|------|
| `code` / `message` | 平台统一业务码与消息（HTTP 成功多为 `"1"`；MQTT 成功多为 `"001000"` / `1000`） |
| `data` | `MusicInfo` 数组（HTTP 由 `data.musicInfos` 提升为 `data`；MQTT 为 `extendParam` 列表；**不含**上游 `count` 信封） |

**`data[]`（MusicInfo）常见字段（有则下发）：** `musicId`、`musicName`、`singerName`、`picUrl`、`listenUrl` / `hqListenUrl` / `sqListenUrl`、`lrcUrl`、`isCollection`、`isCpAuth`、`VIP`、`auditionsFlag`、`contentId`、`songAuthorName`、`lyricAuthorName`、`albumNames`、`length`、`language`、`singerId`、`musicSource`、`bmp`（BPM）等。

**返回值：** `0` 成功，其他失败。HTTP 模式下传输失败返回 `IOTSDK_RET_HTTP_ERROR`（可重试）。

---

## 5. 集成注意点（摘要）

1. **注册顺序（4G）：** `SetOnGetBoardMqtt`（若用 MQTT）→ `SetOnGetBoardHttp` → `Init`。WiFi 通常只 `Init`，不必注册板载 HTTP/MQTT。
2. **模式选择：** `username` + `password` 同时非空 → MQTT；否则 → HTTP OpenAPI。`mqttMode` 仅影响 MQTT/WS 地址形态。
3. **HTTP 重试：** `Init` 的 `args` 可设 `retryCount`（默认 `3`，含首次）。HTTP 模式下 `Init` 及全部业务 HTTP 接口在返回 `IOTSDK_RET_HTTP_ERROR` 时自动重试；业务异常不重试。
4. **action：** 咪咕短请求用 `IOTSDK_MIGU_HTTPS`，音乐流用 `IOTSDK_MIGU_MUSIC`，OpenAPI 用 `IOTSDK_OPENAPI_HTTPS`（见第 3 节）。音乐流建议与短请求分槽（参考 `BoardHttpImpl`）。
5. **JSON 格式：** 字段名、类型需与文档及头文件注释一致。
6. **`Search`：** 喜马+咪咕合并搜索；未传 `providerOrder` 时 SDK 默认仅搜咪咕。`IMusicPlayer::Search` 侧也会强制写入 `providerOrder: ["migu"]`。
7. **`GetMusicInfo`：** `provider` 仅支持 **`migu`**；入参歌曲 ID 字段为 **`musicId`**。`Report` 头文件同时描述咪咕/喜马字段；HTTP 路径当前按咪咕 `contentId` 上报。
8. **歌单接口：** `QueryMusicSheet` 未传 `typeId` 时默认 `"2"`；`QueryMusicSheetMusic` 必填 `musicSheetId`，未传 `picSize` 时 SDK 默认 `"S"`。成功时 `data` 均为列表（HTTP 会把内层 `musicSheetInfos` / `musicInfos` 提升为 `data`）。

---

## 6. 文档维护

- 规范来源：[`IOTSdk.h`](../IOTSdk.h)、[`IOTBoard.h`](../IOTBoard.h)。
- 板载参考：[`EspBoard.cpp`](../esp32/EspBoard.cpp)。
- 若头文件中的宏、接口签名或 JSON 字段有变更，请同步更新本文档。
