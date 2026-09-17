# fw-AC79_AIoT_SDK

<div align="center">

**基于杰理 AC791N SDK，集成 IOTSdk 的 WiFi AIoT 多媒体固件工程**

[English](./README-en.md) · [IOTSdk 接口文档](./apps/demo/demo_music/docs/IOTSdk.md) · [杰理 AC79 官方文档](https://doc.zh-jieli.com/AC79/zh-cn/release_v1.1.0/index.html)

</div>

---

## 仓库说明

本工程托管于：

**https://github.com/sqtech-ai/fw-AC79_AIoT_SDK.git**

在 [杰理科技 AC79 AIoT SDK](https://gitee.com/Jieli-Tech/fw-AC79_AIoT_SDK) 基础上，完成了 **IOTSdk** 的移植与集成，并在 **`demo_music`** 工程中提供可运行的示例代码，演示 WiFi 联网、NTP 校时及咪咕音乐搜索等能力。

| 项目 | 说明 |
|------|------|
| **上游 SDK** | 杰理 AC791N 系列 WiFi / 蓝牙 AIoT 多媒体 SoC 通用固件 SDK |
| **本仓库增强** | 集成 IOTSdk 静态库、`IOTSdkBridge` C 接口及 `demo_music` 示例 |
| **详细接口文档** | [`apps/demo/demo_music/docs/IOTSdk.md`](./apps/demo/demo_music/docs/IOTSdk.md) |

---

## IOTSdk 集成概览

### 移植内容

- **IOTSdk 静态库**：`apps/demo/demo_music/lib/migumusic.a`
- **C 桥接层**：[`IOTSdkBridge.h`](./apps/demo/demo_music/include/IOTSdkBridge.h) — 供 C 应用调用的 `IOTSdk_Init`、`IOTSdk_SearchSongEx` 等接口
- **头文件**：[`IOTSdk.h`](./apps/demo/demo_music/include/IOTSdk.h) — C++ 侧完整 API 定义（详见 IOTSdk.md）
- **网络栈**：基于 SDK 自带 `http_cli` / `mbedtls`，无需额外移植 curl

### 示例工程：`demo_music`

路径：[`apps/demo/demo_music/`](./apps/demo/demo_music/)

| 文件 | 作用 |
|------|------|
| [`app_main.c`](./apps/demo/demo_music/app_main.c) | 主逻辑：WiFi 事件、NTP 同步、按键触发 IOTSdk 初始化与搜歌 |
| [`wifi_demo_task.c`](./apps/demo/demo_music/wifi_demo_task.c) | WiFi STA 联网任务 |
| [`board/wl82/Makefile`](./apps/demo/demo_music/board/wl82/Makefile) | 板级编译脚本，链接 `migumusic.a` |
| [`board/wl82/AC791N_DEMO_DEMO_MUSIC.cbp`](./apps/demo/demo_music/board/wl82/AC791N_DEMO_DEMO_MUSIC.cbp) | Code::Blocks 工程（Windows 推荐） |

**示例交互（按键）：**

| 按键 | 功能 |
|------|------|
| **K1** | 调用 `IOTSdk_Init()`，传入 JSON 初始化参数（`appLicenseId`、`appKey`、`serverToken` 等） |
| **K4** | 调用 `IOTSdk_SearchSongEx()`，搜索咪咕歌曲并打印 JSON 结果 |

联网成功后自动触发 NTP 时间同步，保证 OpenAPI 请求时间戳有效。`deviceId` 默认由设备 MAC 地址生成。

### 初始化参数

按 **K1** 触发 `IOTSdk_Init()` 时，需在 [`app_main.c`](./apps/demo/demo_music/app_main.c) 的 `demo_iotsdk_init()` 中填入 JSON 初始化参数。**`appLicenseId`、`appKey`、`serverToken`、`regionCode`、`servicePackageCode` 须向速启科技申请获取**，填入从速启科技获得的正式值后方可正常联网鉴权与调用业务接口。

| 字段 | 必填 | 说明 |
|------|------|------|
| `appLicenseId` | 是 | 应用许可证 ID |
| `appKey` | 是 | 应用密钥（OpenAPI 签名） |
| `serverToken` | 是 | 服务端 Token |
| `regionCode` | 是 | 区域编码 |
| `servicePackageCode` | 是 | 服务套餐码 |
| `env` | 否 | 环境：`test`（测试）/ `prod`（生产），默认 `test` |

> 各字段含义、示例代码及接口详情见 **[`apps/demo/demo_music/docs/IOTSdk.md`](./apps/demo/demo_music/docs/IOTSdk.md)** §4.1 `Init`。

---

## 快速开始

### 1. 环境准备

与杰理 AC79 SDK 要求一致：

1. 安装杰理工具链（Windows 默认路径 `C:\JL\pi32\bin`）
2. 双击 [`tools/make_prompt.bat`](./tools/make_prompt.bat) 打开带编译环境的命令行（或将 `tools/utils` 与 `C:\JL\pi32\bin` 加入 `PATH`）
3. 详细环境说明见 [杰理 AC79 开发文档 · 环境搭建](https://doc.zh-jieli.com/AC79/zh-cn/release_v1.1.0/index.html)

### 2. 编译 `demo_music` 固件

**方式一：Code::Blocks（推荐）**

```text
打开 apps/demo/demo_music/board/wl82/AC791N_DEMO_DEMO_MUSIC.cbp
Build → Build (Ctrl+F9)
```

**方式二：Makefile**

```bat
cd apps/demo/demo_music/board/wl82
make
make clean   rem 清理
```

编译产物位于 `cpu/wl82/tools/`，使用 SDK 自带烧录工具下载固件。

### 3. 运行示例

1. 烧录固件到 AC791N 开发板
2. 配置 WiFi（`wifi_demo_task` 中 SSID/密码）
3. 向速启科技申请 `appLicenseId`、`appKey`、`serverToken`、`regionCode`、`servicePackageCode`，并填入 `app_main.c` 的 `demo_iotsdk_init()`
4. 联网后按 **K1** 初始化 IOTSdk，按 **K4** 测试歌曲搜索

---

## 工程结构（与本仓库相关部分）

```
fw-AC79_AIoT_SDK/
├── apps/demo/demo_music/          # IOTSdk 示例工程
│   ├── app_main.c                 # 示例入口（按键、NTP、IOTSdk 调用）
│   ├── include/
│   │   ├── IOTSdkBridge.h         # C 桥接接口
│   │   └── IOTSdk.h               # C++ SDK 头文件
│   ├── lib/migumusic.a            # IOTSdk 静态库
│   ├── docs/IOTSdk.md             # 接口对接文档（详细）
│   └── board/wl82/                # 板级配置与编译
├── cpu/wl82/                      # CPU 平台、预编译库、烧录工具
├── include_lib/                   # SDK 头文件（含 http_cli、mbedtls、cJSON）
├── tools/make_prompt.bat          # Windows 编译环境入口
└── Makefile                       # 顶层编译入口（其他方案工程）
```

---

## 上游杰理 SDK 参考

本仓库基于杰理 AC79 AIoT SDK，芯片为 **AC791N 系列（wl82）**，集成 WiFi 802.11b/g/n、双模蓝牙、320MHz 双核 DSP，适用于智能音箱、故事机、IPC 摄像头等 AIoT 多媒体方案。

| 资源 | 链接 |
|------|------|
| 官方 SDK 仓库 | [gitee.com/Jieli-Tech/fw-AC79_AIoT_SDK](https://gitee.com/Jieli-Tech/fw-AC79_AIoT_SDK) |
| 在线开发文档 | [doc.zh-jieli.com/AC79](https://doc.zh-jieli.com/AC79/zh-cn/release_v1.1.0/index.html) |
| 英文 README | [README-en.md](./README-en.md) |
| 其他方案工程 | `wifi_camera`、`wifi_story_machine`、`wifi_ipc`、`scan_box` 及 `apps/demo/` 下各功能 Demo |

顶层 `Makefile` 支持的编译目标示例：

```bash
make ac791n_wifi_camera
make ac791n_wifi_story_machine
make ac791n_demo_demo_wifi
# 完整列表见 Makefile 头部注释
```

---

## 常见问题

**Q：`IOTSdk_Init` 返回设备 ID 无效？**

确认 WiFi 已联网且 MAC 可读；或在初始化 JSON 中显式传入 `deviceId` 字段（见 IOTSdk.md）。

**Q：HTTP 请求失败或解密异常？**

确认 NTP 已同步（日志中有 `NET_NTP_GET_TIME_SUCC`）；检查已向速启科技申请并正确填写 `appKey`、`serverToken` 等鉴权参数，且 `env` 与测试/生产环境一致。

更多接口与 JSON 字段说明，请参阅 **[`apps/demo/demo_music/docs/IOTSdk.md`](./apps/demo/demo_music/docs/IOTSdk.md)**。

---

## 许可证

本 SDK 以 [Apache License 2.0](./LICENSE) 协议开源。上游杰理 SDK 版权归珠海杰理科技股份有限公司所有。

`apps/demo/demo_music/lib/migumusic.a` 为预编译二进制库，其使用与分发须遵守速启科技 / IOTSdk 相关授权约定，**不等同于 Apache 开源源码**。

---

## 联系我们

商务合作与 SDK 获取请联系：

[lz@suqi.tech](mailto:lz@suqi.tech)  
[zhouwanguang@suqi.tech](mailto:zhouwanguang@suqi.tech)

<a href="apps/demo/demo_music/docs/contacts.png" target="_blank" title="企业微信">
  <img src="apps/demo/demo_music/docs/contacts.png" width="240" />
</a>

<div align="center">
  <sub>Copyright © 2024-2026 珠海杰理科技股份有限公司 · IOTSdk 集成维护见 <a href="https://github.com/sqtech-ai/fw-AC79_AIoT_SDK">sqtech-ai/fw-AC79_AIoT_SDK</a></sub>
</div>
