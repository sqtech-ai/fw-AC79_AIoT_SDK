[tag download]:https://gitee.com/Jieli-Tech/fw-AC79_AIoT_SDK/tags
[tag_badgen]:https://img.shields.io/badge/Tag-AC79__SDK__V1.2.0-informational?style=plastic&logo=gitee&labelColor=ffffff&logoColor=C71D23

# fw-AC79_AIoT_SDK  [![tag][tag_badgen]][tag download]

<div align="center">

**杰理 AC791N 系列 WiFi & 蓝牙 AIoT 多媒体 SoC 通用 SDK 固件程序**

[English](./README-en.md) · [文档中心](https://doc.zh-jieli.com/AC79/zh-cn/release_v1.2.0/index.html) · [SDK 版本历史](https://doc.zh-jieli.com/AC79/zh-cn/release_v1.2.0/other/version/index.html) · [报告问题](https://gitee.com/Jieli-Tech/fw-AC79_AIoT_SDK/issues)

</div>

---

## 📋 目录

- [一、概述](#一概述)
- [二、支持的芯片与平台](#二支持的芯片与平台)
- [三、芯片软硬件能力总览](#三芯片软硬件能力总览)
- [四、环境搭建](#四环境搭建)
- [五、快速开始](#五快速开始)
- [六、工程结构](#六工程结构)
- [七、应用与示例指南](#七应用与示例指南)
- [八、编译指南](#八编译指南)
- [九、烧录与升级](#九烧录与升级)
- [十、配置说明](#十配置说明)
- [十一、常见问题](#十一常见问题)
- [十二、社区与支持](#十二社区与支持)
- [十三、认证信息](#十三认证信息)
- [十四、版本与分支说明](#十四版本与分支说明)
- [十五、免责声明](#十五免责声明)

---

## 一、概述

`fw-AC79_AIoT_SDK` 是杰理科技为 **AC791N 系列 AIoT 多媒体 SoC** 提供的通用 SDK 固件开发包。AC791N 是一款低成本、高集成度的 **WiFi 802.11b/g/n + 双模蓝牙（BR/EDR + BLE，V2.1 ~ V5.0）** 音视频多媒体系统级芯片，内部集成主频高达 **320MHz 的双核浮点 DSP**，并完整集成音频（ADC/DAC）、视频（摄像头 ISC）、显示（RGB 推屏）与丰富外设资源，可一站式实现各类音视频物联网方案。

本 SDK 提供完整的协议栈、媒体框架与丰富的应用示例，支持以下典型应用场景：

| 应用类型 | 典型产品 |
|---------|---------|
| **WiFi 音频方案** | WiFi/蓝牙智能音箱、儿童绘本故事机、点读笔/扫描笔/翻译笔 |
| **WiFi 视频方案** | WiFi 监控摄像头 IP Camera、可视门铃/视频门锁/楼宇智能、婴儿监护器、宠物喂食机 |
| **蓝牙 + USB 交互** | 蓝牙/USB 扫码枪、HID 人机交互设备 |
| **智能家居/物联网** | WiFi 可视美容仪、WiFi 摄像头玩具、各类 AIoT 联网设备 |

本仓库为 SDK Release 版本代码及示例工程，需配合对应平台的库文件（`*.a`）进行编译，并引用了 lwIP、mbedTLS、FreeRTOS 等开源项目。SDK 固件包不含开发文档，开发前请详细阅读 [SDK 在线开发文档](https://doc.zh-jieli.com/AC79/zh-cn/release_v1.2.0/index.html)。

---

## 二、支持的芯片与平台

### 2.1 芯片系列

| 芯片平台 | 芯片型号 | 内核 | 适用方案 |
|---------|---------|------|---------|
| **wl82** | AC7911B / AC7912A / AC7913A / AC7915A / AC7915B / AC7916A | 杰理自研双核浮点 DSP @ 320MHz | wifi_camera / wifi_story_machine |

### 2.2 内核与片上资源

| 项目 | 规格 |
|------|------|
| **CPU** | 双核 DSP，最高主频 320MHz，支持单精度浮点及数学运算加速引擎 |
| **缓存与内存管理** | 带 I-Cache、D-Cache、MMU |
| **片上 SRAM** | 共 578 KB |
| **外扩 SDRAM** | 部分封装支持 2 / 8 MB SDRAM |
| **供电** | 内置 PMU，支持 LDO / DCDC 供电模式与多种低功耗模式 |
| **无线共存** | 内部集成 PTA 共存分时模块，WiFi / 蓝牙 V2.1 / 蓝牙 V5.0 可同时工作 |

---

## 三、芯片软硬件能力总览

AC791N 作为 AIoT 多媒体 SoC，集无线、音频、视频、显示、AI 于一体。以下为各子系统能力一览。

### 3.1 外设接口

GPIO、IIC、SPI、SDIO、PWM、MCPWM、UART、USB 1.1、USB 2.0、ADC、TIMER、IR 接收、电容触摸按键、GPCNT、RTC。

### 3.2 MATH 硬件加速

| 能力 | 说明 |
|------|------|
| 数字信号 | 硬件 FFT、IFFT、矩阵运算 |
| 加解密 | 硬件 AES128/256、SHA128/256 |
| 其他 | 硬件随机数、CRC16 |

### 3.3 蓝牙

- 符合蓝牙 **V5.0 + BR + EDR + BLE** 规范
- 满足 Class2 / Class3 发射功率要求，支持 GFSK 与 π/4 DQPSK 所有包类型
- 最大 **+15 dBm** 发射功率，接收灵敏度 **-93 dBm**

### 3.4 WiFi

- 支持 IEEE **802.11b/g/n**，802.11n 支持 MCS0~MCS7、20MHz/40MHz 带宽，800ns / 400ns 保护间隔
- 支持 **AP / STA / Monitor（配网）** 模式；AP 多站接入；STA 多网络记忆、择优连接、冷启动快连
- 支持 Power Save 省电模式；支持 Open / WEP / WPA-PSK / WPA2-PSK + TKIP/AES/CCMP 加密
- 支持脱离 802.11 协议直接收发底层 RF 数据包，可对接 CMW270 等测试仪测试 RF 性能
- 发射功率：DSSS 1M/s 17dBm / MCS0 16dBm / MCS7 12dBm
- 接收灵敏度：DSSS 1M/s -95dBm / MCS0 -91dBm / MCS7 -72dBm

### 3.5 音频

- 集成 DAC、MIC、LINEIN、IIS、PDM、SPDIF 硬件模块；IIS 最高 8 通道（可独立配置输入/输出，16/24bit）；PDMLINK 支持 4 路 16bit 数字麦；ADC 4 通道（每通道可配 MIC 或 LINEIN）
- **编码**：SBC、MSBC、CVSD、AAC、MP2、MP3、ADPCM、AMR、OPUS、SPX、WAV、PCM
- **解码**：SBC、MSBC、CVSD、AAC、ADPCM、AMR、APE、DTS、FLAC、M4A、MP1/2/3、OPUS、SPX、WAV、WMA、PCM；数据源支持 FLASH/SD卡/U盘/LINEIN/外挂FM/网络URL/经典蓝牙等
- **音效**：混响、回声、电音、变声变调、变速、移频、啸叫抑制、EQ、DRC、回声消除、传统降噪、神经网络降噪
- **语音**：活动语音检测 VAD、单/双 MIC 的打断唤醒 ASR；部分格式支持断点/快进快退/定点/AB 复读

### 3.6 视频

- 支持 DVP-1/2/4/8bit、BT656 图像传感器接口的 YUV sensor，最大 720P 分辨率
- JPEG 编码最大 **720P@30fps@AVI** 封装；支持任意尺寸 JPEG 单张编解码
- 支持 SPI 接口摄像头；支持一路 DVP + 一路 SPI 摄像头同时输出 YUV（一路图传/录卡，另一路光流算法）
- 支持图像拼接（176×128，最高 60fps）；支持软件对 YUV 帧缩放、裁剪

### 3.7 显示

- 支持 SPI、EMI、PAP、RGB888(8bit) / RGB666(6bit) 推屏接口（RGB 可达 480×272@15fps、320×240@30fps）
- 杰理 UI 工具支持屏触摸、软件图层/旋转、音标显示、自定义合成区域、多字号同时显示、SD 卡加载 UI 资源
- 支持播放 JPG、AVI、GIF 文件

### 3.8 网络协议栈与 AI 云平台

- **基础协议**：lwIP、mbedTLS、HTTP/HTTPS、WebSocket、CoAP、nopoll、cURL、MQTT、FTP、uIP、iperf
- **AI 云平台**：图灵、百度云、腾讯云、中国电信智能家居、涂鸦、阿里云、华为 HiLink、天猫精灵、亚马逊、思必驰、玩瞳，以及火山大模型（LLM）

### 3.9 固件升级

- 支持 U盘 / SD卡 单备份升级
- 支持 U盘 / SD卡 / WiFi 双备份升级
- 支持「代码双备份 + 资源部分备份 + 资源部分固定」方式升级

### 3.10 SDK 中间件

FAT 文件系统、数据存储记忆、FreeRTOS / Pthread API、循环缓冲 CBUF、帧缓冲 LBUF。

---

## 四、环境搭建

### 4.1 前提条件

| 系统 | 说明 |
|------|------|
| **Windows** | ✅ 推荐使用 Code::Blocks IDE 编译 |
| **Linux** | ✅ 支持 Makefile 命令行编译 |
| **macOS** | ⚠️ 需自行配置交叉编译工具链 |

### 4.2 安装编译工具链

1. 下载并安装 **杰理编译工具链**：[开发环境文档](https://doc.zh-jieli.com/Tools/zh-cn/dev_tools/dev_env/index.html)
2. Linux 用户可从 [pkgman.jieliapp.com](http://pkgman.jieliapp.com/doc/all) 下载：
   - 解压到 `/opt/jieli` 目录
   - 确保 `/opt/jieli/common/bin/clang` 存在
3. 安装完成后验证：

```bash
# 验证工具链是否安装成功
clang --version
```

### 4.3 安装烧录与测试工具

| 工具 | 用途 | 获取方式 |
|------|------|---------|
| **USB 升级工具** | 将固件烧录到目标板 | [使用文档](https://doc.zh-jieli.com/Tools/zh-cn/dev_tools/forced_upgrade/index.html) |
| **生产烧写工具** | 量产/裸片烧写 | [使用文档](https://doc.zh-jieli.com/Tools/zh-cn/mass_prod_tools/burner_1tuo2/index.html) |

---

## 五、快速开始

### 5.1 克隆仓库

```bash
git clone https://gitee.com/Jieli-Tech/fw-AC79_AIoT_SDK.git
cd fw-AC79_AIoT_SDK
```

### 5.2 选择合适的工程

根据你的产品需求选择应用工程：

```
SDK 根目录
├── apps/wifi_camera/          # WiFi 摄像头方案
├── apps/wifi_ipc/             # WiFi IPC 网络摄像机方案
├── apps/wifi_story_machine/   # WiFi 故事机方案
├── apps/scan_box/             # 扫码盒方案
└── apps/demo/                 # 11 个功能 demo（ble/wifi/edr/ui/uvc/video/audio…）
```

### 5.3 选择板级配置

每个应用目录下都有 `board/wl82/` 子目录，包含：

- `Makefile` - 编译脚本
- `AC791N_*.cbp` - Code::Blocks 工程文件
- `board_79xx_cfg.h` - 板级配置（引脚、外设等，按芯片型号区分，如 `board_7916A_cfg.h`）
- `board_*.c` - 板级初始化代码

### 5.4 编译并烧录

**方式一：Code::Blocks（推荐 Windows 用户）**

```bash
# 1. 进入对应的板级目录
cd apps/wifi_camera/board/wl82/

# 2. 双击打开 .cbp 工程文件（如 AC791N_WIFI_CAMERA.cbp）
# 3. 在 Code::Blocks 中点击 Build → Build (Ctrl+F9)
# 4. 编译成功后，使用 USB 升级工具烧录生成的固件文件
```

**方式二：Makefile 命令行**

```bash
# Windows 用户：双击 tools/make_prompt.bat 打开命令行环境

# Linux 用户：在 SDK 根目录执行
make ac791n_wifi_camera
```

> **💡 提示**：所有支持的 target 名称见 [Makefile](Makefile) 开头的注释。

**方式三：VS Code 编译**

仓库可配合 VS Code 任务编译，按 `Ctrl+Shift+B` 选择编译目标。

---

## 六、工程结构

```
fw-AC79_AIoT_SDK/
├── apps/                     # 应用层代码
│   ├── common/               # 公共模块（asr/audio_music/ble/camera/eth/fm/
│   │                         #   gsensor/jl_math/LLM/ui/usb/update/third_party_profile…）
│   ├── wifi_camera/          # 📌 WiFi 摄像头方案
│   ├── scan_box/             # 📌 扫码盒方案
│   ├── wifi_ipc/             # 📌 WiFi IPC 网络摄像机方案
│   ├── wifi_story_machine/   # 📌 WiFi 故事机方案
│   └── demo/                 # 📌 11 个功能 demo（ble/wifi/edr/ui/uvc/video/audio…）
├── cpu/wl82/                 # CPU 平台代码 + 预编译库(liba) + 烧录工具(tools)
├── include_lib/              # 头文件（btctrler/btstack/driver/media/net/system/update…）
├── lib/                      # 预编译库（net/server/utils…）
├── doc/                      # 数据手册(datasheet/AC791N规格书) 与资料(stuff)
├── docs/                     # 在线文档源（rst）
├── tools/                    # 编译工具（make_prompt.bat + utils）
├── Makefile                  # 顶层统一编译入口
└── default.workspace         # Code::Blocks 工作空间
```

### 6.1 关键目录说明

| 目录 | 作用 |
|------|------|
| `apps/*/board/wl82/` | **板级配置**：引脚定义、外设初始化、编译选项、`.cbp` 工程 |
| `apps/common/` | **公共模块**：跨工程共享的音频、视频、蓝牙、WiFi、UI、AI 等中间件 |
| `apps/common/config/` | **库配置**：蓝牙 Profile、用户配置、日志等裁剪配置 |
| `apps/demo/` | **功能示例**：11 个可直接参考或修改的最小 demo 工程 |
| `cpu/wl82/liba/` | **预编译库**：`*.a` 静态库文件 |
| `cpu/wl82/tools/` | **烧录工具**：下载脚本、固件打包工具等 |
| `include_lib/` | **头文件**：协议栈、驱动、媒体、系统、网络等模块接口 |

---

## 七、应用与示例指南

### 7.1 方案工程

| 方案 | 路径 | 适用场景 | make target |
|------|------|---------|-------------|
| **WiFi 摄像头** | `apps/wifi_camera/` | WiFi 监控摄像头、可视门铃、图传 | `ac791n_wifi_camera` |
| **WiFi IPC** | `apps/wifi_ipc/` | 网络摄像机 IP Camera、录卡/网络推流 | `ac791n_wifi_ipc` |
| **WiFi 故事机** | `apps/wifi_story_machine/` | 儿童故事机、智能音箱、网络音频播放 | `ac791n_wifi_story_machine` |
| **扫码盒** | `apps/scan_box/` | 蓝牙/USB 扫码枪、HID/POS 设备 | `ac791n_scan_box` |

### 7.2 功能 Demo 矩阵

| Demo | 路径 | 演示功能 | make target |
|------|------|---------|-------------|
| demo_ble | `apps/demo/demo_ble/` | BLE 低功耗蓝牙数传 | `ac791n_demo_demo_ble` |
| demo_edr | `apps/demo/demo_edr/` | 经典蓝牙 EDR（音乐/SPP/发射器/解码） | `ac791n_demo_demo_edr` |
| demo_wifi | `apps/demo/demo_wifi/` | WiFi STA/AP 联网 | `ac791n_demo_demo_wifi` |
| demo_wifi_ext | `apps/demo/demo_wifi_ext/` | 外置 WiFi / LTE 扩展联网 | `ac791n_demo_demo_wifi_ext` |
| demo_audio | `apps/demo/demo_audio/` | 音频采集/播放/编解码 | `ac791n_demo_demo_audio` |
| demo_video | `apps/demo/demo_video/` | 视频采集/JPEG 编码/录像 | `ac791n_demo_demo_video` |
| demo_uvc | `apps/demo/demo_uvc/` | USB UVC 摄像头 | `ac791n_demo_demo_uvc` |
| demo_ui | `apps/demo/demo_ui/` | UI 显示/图层/触摸 | `ac791n_demo_demo_ui` |
| demo_hello | `apps/demo/demo_hello/` | 最小启动工程（入门骨架） | `ac791n_demo_demo_hello` |
| demo_DevKitBoard | `apps/demo/demo_DevKitBoard/` | 官方开发板综合演示 | `ac791n_demo_demo_devkitboard` |
| demo_matter | `apps/demo/demo_matter/` | Matter 物联网协议（需进入 board 目录编译） | — |

> **参考文档**：各模块详细开发说明见 [AC79 模块示例文档](https://doc.zh-jieli.com/AC79/zh-cn/release_v1.2.0/index.html)。

---

## 八、编译指南

### 8.1 编译命令速查表

以下命令在 SDK 根目录下执行：

| 目标 | 应用 | 命令 |
|------|------|------|
| WiFi 摄像头 | wifi_camera | `make ac791n_wifi_camera` |
| WiFi IPC | wifi_ipc | `make ac791n_wifi_ipc` |
| WiFi 故事机 | wifi_story_machine | `make ac791n_wifi_story_machine` |
| 扫码盒 | scan_box | `make ac791n_scan_box` |
| BLE Demo | demo_ble | `make ac791n_demo_demo_ble` |
| EDR Demo | demo_edr | `make ac791n_demo_demo_edr` |
| WiFi Demo | demo_wifi | `make ac791n_demo_demo_wifi` |
| 扩展 WiFi  Demo | demo_wifi_ext | `make ac791n_demo_demo_wifi_ext` |
| 音频 Demo | demo_audio | `make ac791n_demo_demo_audio` |
| 视频 Demo | demo_video | `make ac791n_demo_demo_video` |
| UVC Demo | demo_uvc | `make ac791n_demo_demo_uvc` |
| UI Demo | demo_ui | `make ac791n_demo_demo_ui` |
| Hello Demo | demo_hello | `make ac791n_demo_demo_hello` |
| 开发板 Demo | demo_DevKitBoard | `make ac791n_demo_demo_devkitboard` |
| **全部** | 全部 | `make all` |
| **清理全部** | 全部 | `make clean` |

### 8.2 清理单个工程

```bash
make clean_ac791n_wifi_camera     # 清理 wifi_camera 编译产物
make clean_ac791n_demo_demo_ble   # 清理 demo_ble 编译产物
```

### 8.3 Linux 编译注意事项

```bash
# 1. 确保文件描述符限制足够大（链接阶段需要打开大量文件）
ulimit -n 8096

# 2. 进入 SDK 根目录并行编译
make ac791n_wifi_camera -j`nproc`
```

### 8.4 常见编译错误

| 错误提示 | 解决方法 |
|---------|---------|
| `clang: command not found` | 未安装杰理编译工具链，或环境变量未配置 |
| `Too many open files` | Linux 下执行 `ulimit -n 8096` 增加文件描述符限制 |
| `cannot find -lxxx` | 缺少对应的 `.a` 库文件，检查 `cpu/wl82/liba/` 与 `lib/` 目录 |
| `undefined reference to ...` | 功能裁剪配置未包含对应模块，检查 `apps/common/config/` 下的配置 |

---

## 九、烧录与升级

### 9.1 首次烧录

1. **连接硬件**：将开发板通过 USB 或 UART 连接到 PC
2. **进入编程模式**：按住开发板上的烧录按键，然后复位或重新上电
3. **打开 USB 升级工具**：启动烧录上位机
4. **选择固件**：选择编译生成的固件文件
5. **开始烧录**：点击下载按钮，等待烧录完成

> **⚠️ 注意**：烧录前请确保 USB 升级工具正确连接且目标板已进入编程模式。

### 9.2 固件升级

AC791N 支持多种升级方式，详见 [系统升级文档](https://doc.zh-jieli.com/AC79/zh-cn/release_v1.2.0/index.html)：

- **单备份升级**：U盘 / SD卡
- **双备份升级**：U盘 / SD卡 / WiFi（升级更安全，失败可回滚）
- **代码双备份 + 资源部分备份 + 资源部分固定** 升级

---

## 十、配置说明

### 10.1 功能裁剪配置

在 `apps/common/config/` 目录下，通过以下配置可灵活裁剪 SDK 功能、减小固件体积：

```bash
apps/common/config/
├── bt_profile_config.c     # 蓝牙 Profile 配置
├── user_cfg.c              # 用户参数配置
├── new_cfg_tool.c          # 配置工具
├── ci_transport_uart.c     # 串口通信传输配置
└── log_config/             # 日志输出配置
```

### 10.2 板级配置

每个板级目录 `apps/*/board/wl82/` 下的 `board_79xx_cfg.h`（按芯片型号区分，如 `board_7916A_cfg.h`）包含：

- **引脚映射**：UART / SPI / IIC / SDIO / GPIO 等外设的引脚分配
- **外设使能**：开启或关闭特定外设模块（音频、摄像头、屏、按键等）
- **时钟配置**：CPU 频率、外设时钟源
- **功能开关与内存配置**：按需启用功能、堆栈与缓冲池大小

---

## 十一、常见问题

### 11.1 开发流程相关

**Q: 如何创建自己的工程？**
A: 复制 `apps/` 下最接近的方案工程，进入其 `board/wl82/` 目录，复制并修改与目标芯片型号最接近的 `board_79xx_cfg.h`，调整引脚和外设配置即可。

### 11.2 编译相关

**Q: Windows 下编译报错 `make` 不是有效命令？**
A: 使用 `tools/make_prompt.bat` 进入预配置的命令行环境，该脚本已设置好 `make` 路径和环境变量。

**Q: 如何加快编译速度？**
A: 使用 `-j` 参数并行编译，如 `make ac791n_wifi_camera -j4`。

### 11.3 调试技巧

- **串口日志**：通过 `apps/common/config/log_config/` 配置日志输出等级和通道
- **GPIO Debug**：利用空闲 GPIO 输出调试波形，测量时序
- **更多问题**：请参考 [在线 FAQ 文档](https://doc.zh-jieli.com/AC79/zh-cn/release_v1.2.0/other/faq/index.html)

---

## 十二、社区与支持

### 技术交流

| 平台 | 群号 |
|------|------|
| **钉钉技术支持群 1** | `15375032297` |
| **钉钉技术支持群 2** | `31275808` |

### 资源链接

| 资源 | 链接 |
|------|------|
| 📖 **在线文档中心** | [doc.zh-jieli.com/AC79](https://doc.zh-jieli.com/AC79/zh-cn/release_v1.2.0/index.html) |
| 📄 **芯片数据手册** | [AC791N 规格书（本地）](./doc/datasheet/AC791N规格书) |
| 📚 **SDK 版本历史** | [版本发布记录](https://doc.zh-jieli.com/AC79/zh-cn/release_v1.2.0/other/version/index.html) |
| 💻 **Gitee 仓库** | [Jieli-Tech/fw-AC79_AIoT_SDK](https://gitee.com/Jieli-Tech/fw-AC79_AIoT_SDK) |
| 🐛 **问题反馈** | [Gitee Issues](https://gitee.com/Jieli-Tech/fw-AC79_AIoT_SDK/issues) |

---

## 十三、认证信息

本 SDK 支持的蓝牙协议栈与 WiFi 已通过官方认证：

### 蓝牙官方认证

经典蓝牙 LMP / 低功耗蓝牙 Link Layer 层和 Host 协议栈均支持蓝牙 5.0 与 5.1 实现：

| 蓝牙规范 | QDID | 认证链接 |
|---------|------|---------|
| Core v5.0 | QDID 134104 | [查看认证详情](https://launchstudio.bluetooth.com/ListingDetails/88799) |
| Core v5.1 | QDID 136145 | [查看认证详情](https://launchstudio.bluetooth.com/ListingDetails/91371) |

### WiFi 官方认证

已完成 Wi-Fi 联盟互操作性测试，包含 WPA/WPA2-Personal 安全认证、B/G/N 连通性测试：

| 认证 | 编号 | 认证链接 |
|------|------|---------|
| Wi-Fi CERTIFIED™ 互操作性 | WFA100441 | [查看认证详情](https://www.wi-fi.org/content/search-page?keys=WFA100441) |

---

## 十四、版本与分支说明

> AC79 Gitee 仓库时常更新，特别是用于开发新特性的 `master` 分支。
> 如需测试/研发/尝试最新特性，建议参考 `master` 分支；如有量产用途，建议使用稳定发布版本（`release/AC79NN_SDK_Vx.x.x`）。一般新项目建议使用最新稳定版本。

### 14.1 分支策略

| 分支 | 用途 | 配套文档 |
|------|------|---------|
| `release/AC79NN_SDK_V1.2.0` | 最新稳定发布版本（推荐量产） | [v1.2.0 文档](https://doc.zh-jieli.com/AC79/zh-cn/release_v1.2.0/index.html) |

### 14.2 版本号规则（major.minor.patch）

- **major**：主版本号，结构发生重大变化、无法与旧版本兼容
- **minor**：次版本号，反映较大更改（如 API 增加），但不影响源码与二进制兼容性
- **patch**：补丁版本，仅修改内部实现、不影响 API 接口

> 可在文档页面左上角的版本下拉菜单中切换不同 AC79 文档版本。

---

## 十五、免责声明

`fw-AC79_AIoT_SDK` 支持 AC791N 系列芯片开发。AC791N 系列芯片支持 WiFi、蓝牙、音频、视频、显示等 AIoT 多媒体常见应用，可作为开发、评估、样品及量产使用，对应 SDK 版本请见 [Tags](https://gitee.com/Jieli-Tech/fw-AC79_AIoT_SDK/tags)。

本 SDK 以 [Apache License 2.0](./LICENSE) 协议开源。

---

<div align="center">
  <sub>Copyright © 2024-2026 珠海杰理科技股份有限公司. All rights reserved.</sub>
</div>
