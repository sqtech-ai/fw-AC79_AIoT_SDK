[tag download]:https://gitee.com/Jieli-Tech/fw-AC79_AIoT_SDK/tags
[tag_badgen]:https://img.shields.io/badge/Tag-AC79__SDK__V1.2.0-informational?style=plastic&logo=gitee&labelColor=ffffff&logoColor=C71D23

# fw-AC79_AIoT_SDK  [![tag][tag_badgen]][tag download]

<div align="center">

**Jieli AC791N Series WiFi & Bluetooth AIoT Multimedia SoC General-Purpose SDK Firmware**

[中文](./README.md) · [Documentation](https://doc.zh-jieli.com/AC79/zh-cn/release_v1.2.0/index.html) · [Release Notes](https://doc.zh-jieli.com/AC79/zh-cn/release_v1.2.0/other/version/index.html) · [Report an Issue](https://gitee.com/Jieli-Tech/fw-AC79_AIoT_SDK/issues)

</div>

---

## 📋 Table of Contents

- [1. Overview](#1-overview)
- [2. Supported Chips & Platforms](#2-supported-chips--platforms)
- [3. Hardware & Software Capabilities](#3-hardware--software-capabilities)
- [4. Environment Setup](#4-environment-setup)
- [5. Quick Start](#5-quick-start)
- [6. Project Structure](#6-project-structure)
- [7. Application & Example Guide](#7-application--example-guide)
- [8. Build Guide](#8-build-guide)
- [9. Flashing & Upgrade](#9-flashing--upgrade)
- [10. Configuration](#10-configuration)
- [11. FAQ](#11-faq)
- [12. Community & Support](#12-community--support)
- [13. Certification](#13-certification)
- [14. Versions & Branches](#14-versions--branches)
- [15. Disclaimer](#15-disclaimer)

---

## 1. Overview

`fw-AC79_AIoT_SDK` is the general-purpose SDK firmware development package provided by Jieli Technology for the **AC791N series AIoT multimedia SoC**. The AC791N is a low-cost, highly integrated audio/video multimedia system-on-chip combining **WiFi 802.11b/g/n + dual-mode Bluetooth (BR/EDR + BLE, V2.1–V5.0)**. It integrates a **dual-core floating-point DSP running up to 320MHz**, together with audio (ADC/DAC), video (camera ISC), display (RGB panel) and rich peripheral resources, enabling one-stop implementation of a wide range of audio/video IoT solutions.

This SDK provides a complete protocol stack, media framework, and a rich set of application examples, supporting the following typical use cases:

| Application Type | Typical Products |
|------------------|------------------|
| **WiFi Audio Solutions** | WiFi/Bluetooth smart speakers, children's story machines, reading/scanning/translation pens |
| **WiFi Video Solutions** | WiFi IP cameras, video doorbells/door locks/building intercom, baby monitors, pet feeders |
| **Bluetooth + USB Interaction** | Bluetooth/USB barcode scanners, HID human-interface devices |
| **Smart Home / IoT** | WiFi visual beauty devices, WiFi camera toys, various AIoT connected devices |

This repository contains the SDK Release source code and example projects. It must be built together with the matching platform library files (`*.a`), and references open-source projects such as lwIP, mbedTLS, and FreeRTOS. The firmware package does not include development documentation — please read the [online SDK documentation](https://doc.zh-jieli.com/AC79/zh-cn/release_v1.2.0/index.html) before development.

---

## 2. Supported Chips & Platforms

### 2.1 Chip Series

| Platform | Chip Models | Core | Applicable Solutions |
|----------|-------------|------|----------------------|
| **wl82** | AC7911B / AC7912A / AC7913A / AC7915A / AC7915B / AC7916A | Jieli in-house dual-core floating-point DSP @ 320MHz | wifi_camera / wifi_story_machine |

### 2.2 Core & On-Chip Resources

| Item | Specification |
|------|---------------|
| **CPU** | Dual-core DSP, up to 320MHz, with single-precision floating point and math acceleration engine |
| **Cache & MMU** | I-Cache, D-Cache, MMU |
| **On-chip SRAM** | 578 KB total |
| **External SDRAM** | 2 / 8 MB SDRAM supported on selected packages |
| **Power** | Built-in PMU, LDO / DCDC supply modes and multiple low-power modes |
| **Wireless Coexistence** | Built-in PTA time-sharing coexistence module; WiFi / Bluetooth V2.1 / Bluetooth V5.0 can operate simultaneously |

---

## 3. Hardware & Software Capabilities

As an AIoT multimedia SoC, the AC791N integrates wireless, audio, video, display and AI in a single chip. The following is an overview of each subsystem.

### 3.1 Peripheral Interfaces

GPIO, IIC, SPI, SDIO, PWM, MCPWM, UART, USB 1.1, USB 2.0, ADC, TIMER, IR receiver, capacitive touch keys, GPCNT, RTC.

### 3.2 MATH Hardware Acceleration

| Capability | Description |
|------------|-------------|
| Digital signal | Hardware FFT, IFFT, matrix operations |
| Cryptography | Hardware AES128/256, SHA128/256 |
| Others | Hardware random number generator, CRC16 |

### 3.3 Bluetooth

- Compliant with Bluetooth **V5.0 + BR + EDR + BLE** specifications
- Meets Class2 / Class3 transmit power requirements; supports all GFSK and π/4 DQPSK packet types
- Up to **+15 dBm** transmit power, **-93 dBm** receive sensitivity

### 3.4 WiFi

- Supports IEEE **802.11b/g/n**; 802.11n supports MCS0–MCS7, 20MHz/40MHz bandwidth, 800ns / 400ns guard intervals
- Supports **AP / STA / Monitor (provisioning)** modes; multi-station access in AP mode; STA multi-network memory, best-signal selection, cold-start fast connect
- Supports Power Save mode; supports Open / WEP / WPA-PSK / WPA2-PSK + TKIP/AES/CCMP encryption
- Supports raw RF packet TX/RX bypassing the 802.11 protocol; can connect to instruments such as CMW270 for RF performance testing
- Transmit power: DSSS 1M/s 17dBm / MCS0 16dBm / MCS7 12dBm
- Receive sensitivity: DSSS 1M/s -95dBm / MCS0 -91dBm / MCS7 -72dBm

### 3.5 Audio

- Integrated DAC, MIC, LINEIN, IIS, PDM, SPDIF hardware modules; IIS supports up to 8 channels (independently configurable as input/output, 16/24-bit); PDMLINK supports 4 channels of 16-bit digital MICs; ADC supports 4 channels (each configurable as MIC or LINEIN)
- **Encoding**: SBC, MSBC, CVSD, AAC, MP2, MP3, ADPCM, AMR, OPUS, SPX, WAV, PCM
- **Decoding**: SBC, MSBC, CVSD, AAC, ADPCM, AMR, APE, DTS, FLAC, M4A, MP1/2/3, OPUS, SPX, WAV, WMA, PCM; sources include FLASH/SD card/USB drive/LINEIN/external FM/network URL/classic Bluetooth, etc.
- **Audio effects**: reverb, echo, electric voice, voice change/pitch shift, time stretch, frequency shift, howling suppression, EQ, DRC, echo cancellation, conventional noise reduction, neural-network noise reduction
- **Voice**: Voice Activity Detection (VAD), single/dual-MIC barge-in wakeup ASR; selected formats support breakpoint resume / fast-forward & rewind / fixed-point playback / A-B repeat

### 3.6 Video

- Supports DVP-1/2/4/8bit and BT656 image sensor interfaces for YUV sensors, up to 720P resolution
- JPEG encoding up to **720P@30fps@AVI** packaging; supports single-frame JPEG encode/decode of arbitrary size
- Supports SPI camera; supports one DVP + one SPI camera outputting YUV simultaneously (one for image transmission/card recording, the other for optical-flow algorithms)
- Supports image stitching (176×128, up to 60fps); supports software scaling and cropping of YUV frames

### 3.7 Display

- Supports SPI, EMI, PAP, RGB888(8bit) / RGB666(6bit) panel interfaces (RGB up to 480×272@15fps, 320×240@30fps)
- Jieli UI tool supports touch screens, software layers/rotation, phonetic-symbol display, custom composited regions, multiple font sizes simultaneously, and loading UI resources from SD card
- Supports playback of JPG, AVI, GIF files

### 3.8 Network Stack & AI Cloud Platforms

- **Base protocols**: lwIP, mbedTLS, HTTP/HTTPS, WebSocket, CoAP, nopoll, cURL, MQTT, FTP, uIP, iperf
- **AI cloud platforms**: Turing, Baidu Cloud, Tencent Cloud, China Telecom Smart Home, Tuya, Alibaba Cloud, Huawei HiLink, Tmall Genie, Amazon, AISpeech, Wonders, plus Volcano LLM (large language model)

### 3.9 Firmware Upgrade

- Single-backup upgrade via USB drive / SD card
- Dual-backup upgrade via USB drive / SD card / WiFi
- "Code dual-backup + partial resource backup + partial fixed resource" upgrade

### 3.10 SDK Middleware

FAT file system, persistent data storage, FreeRTOS / Pthread API, circular buffer (CBUF), frame buffer (LBUF).

---

## 4. Environment Setup

### 4.1 Prerequisites

| OS | Notes |
|----|-------|
| **Windows** | ✅ Code::Blocks IDE recommended |
| **Linux** | ✅ Makefile command-line build supported |
| **macOS** | ⚠️ Cross-compilation toolchain must be configured manually |

### 4.2 Install the Toolchain

1. Download and install the **Jieli compilation toolchain**: [Dev Environment Docs](https://doc.zh-jieli.com/Tools/zh-cn/dev_tools/dev_env/index.html)
2. Linux users can download from [pkgman.jieliapp.com](http://pkgman.jieliapp.com/doc/all):
   - Extract to the `/opt/jieli` directory
   - Make sure `/opt/jieli/common/bin/clang` exists
3. Verify after installation:

```bash
# Verify the toolchain is installed
clang --version
```

### 4.3 Install Flashing & Test Tools

| Tool | Purpose | Access |
|------|---------|--------|
| **USB Upgrade Tool** | Flash firmware to the target board | [User Guide](https://doc.zh-jieli.com/Tools/zh-cn/dev_tools/forced_upgrade/index.html) |
| **Production Burner** | Mass production / bare-die programming | [User Guide](https://doc.zh-jieli.com/Tools/zh-cn/mass_prod_tools/burner_1tuo2/index.html) |

---

## 5. Quick Start

### 5.1 Clone the Repository

```bash
git clone https://gitee.com/Jieli-Tech/fw-AC79_AIoT_SDK.git
cd fw-AC79_AIoT_SDK
```

### 5.2 Choose a Project

Select an application project based on your product requirements:

```
SDK root
├── apps/wifi_camera/          # WiFi camera solution
├── apps/wifi_ipc/             # WiFi IPC network camera solution
├── apps/wifi_story_machine/   # WiFi story machine solution
├── apps/scan_box/             # Barcode scanner box solution
└── apps/demo/                 # 11 functional demos (ble/wifi/edr/ui/uvc/video/audio…)
```

### 5.3 Choose a Board Configuration

Each application directory contains a `board/wl82/` subdirectory with:

- `Makefile` - build script
- `AC791N_*.cbp` - Code::Blocks project file
- `board_79xx_cfg.h` - board configuration (pins, peripherals, etc., per chip model, e.g. `board_7916A_cfg.h`)
- `board_*.c` - board initialization code

### 5.4 Build and Flash

**Option 1: Code::Blocks (recommended for Windows users)**

```bash
# 1. Enter the corresponding board directory
cd apps/wifi_camera/board/wl82/

# 2. Double-click the .cbp project file (e.g. AC791N_WIFI_CAMERA.cbp)
# 3. In Code::Blocks, click Build → Build (Ctrl+F9)
# 4. After a successful build, flash the generated firmware with the USB upgrade tool
```

**Option 2: Makefile command line**

```bash
# Windows users: double-click tools/make_prompt.bat to open the build console

# Linux users: run from the SDK root
make ac791n_wifi_camera
```

> **💡 Tip**: All supported target names are listed in the comments at the top of the [Makefile](Makefile).

**Option 3: VS Code build**

The repository can be built with VS Code tasks — press `Ctrl+Shift+B` to select a build target.

---

## 6. Project Structure

```
fw-AC79_AIoT_SDK/
├── apps/                     # Application-layer code
│   ├── common/               # Common modules (asr/audio_music/ble/camera/eth/fm/
│   │                         #   gsensor/jl_math/LLM/ui/usb/update/third_party_profile…)
│   ├── wifi_camera/          # 📌 WiFi camera solution
│   ├── scan_box/             # 📌 Barcode scanner box solution
│   ├── wifi_ipc/             # 📌 WiFi IPC network camera solution
│   ├── wifi_story_machine/   # 📌 WiFi story machine solution
│   └── demo/                 # 📌 11 functional demos (ble/wifi/edr/ui/uvc/video/audio…)
├── cpu/wl82/                 # CPU platform code + prebuilt libraries (liba) + flashing tools (tools)
├── include_lib/              # Header files (btctrler/btstack/driver/media/net/system/update…)
├── lib/                      # Prebuilt libraries (net/server/utils…)
├── doc/                      # Datasheet (datasheet/AC791N规格书) and resources (stuff)
├── docs/                     # Online documentation source (rst)
├── tools/                    # Build tools (make_prompt.bat + utils)
├── Makefile                  # Top-level unified build entry
└── default.workspace         # Code::Blocks workspace
```

### 6.1 Key Directories

| Directory | Role |
|-----------|------|
| `apps/*/board/wl82/` | **Board configuration**: pin definitions, peripheral init, build options, `.cbp` projects |
| `apps/common/` | **Common modules**: cross-project audio, video, Bluetooth, WiFi, UI, AI middleware |
| `apps/common/config/` | **Library config**: Bluetooth Profile, user config, logging and other tailoring options |
| `apps/demo/` | **Functional examples**: 11 minimal demo projects to reference or modify directly |
| `cpu/wl82/liba/` | **Prebuilt libraries**: `*.a` static library files |
| `cpu/wl82/tools/` | **Flashing tools**: download scripts, firmware packaging tools, etc. |
| `include_lib/` | **Header files**: protocol stack, driver, media, system, network module interfaces |

---

## 7. Application & Example Guide

### 7.1 Solution Projects

| Solution | Path | Use Cases | make target |
|----------|------|-----------|-------------|
| **WiFi Camera** | `apps/wifi_camera/` | WiFi surveillance camera, video doorbell, image transmission | `ac791n_wifi_camera` |
| **WiFi IPC** | `apps/wifi_ipc/` | IP camera, card recording / network streaming | `ac791n_wifi_ipc` |
| **WiFi Story Machine** | `apps/wifi_story_machine/` | Children's story machine, smart speaker, network audio playback | `ac791n_wifi_story_machine` |
| **Scanner Box** | `apps/scan_box/` | Bluetooth/USB barcode scanner, HID/POS devices | `ac791n_scan_box` |

### 7.2 Functional Demo Matrix

| Demo | Path | Demonstrated Feature | make target |
|------|------|----------------------|-------------|
| demo_ble | `apps/demo/demo_ble/` | BLE low-energy data transfer | `ac791n_demo_demo_ble` |
| demo_edr | `apps/demo/demo_edr/` | Classic Bluetooth EDR (music/SPP/emitter/decode) | `ac791n_demo_demo_edr` |
| demo_wifi | `apps/demo/demo_wifi/` | WiFi STA/AP networking | `ac791n_demo_demo_wifi` |
| demo_wifi_ext | `apps/demo/demo_wifi_ext/` | External WiFi / LTE extended networking | `ac791n_demo_demo_wifi_ext` |
| demo_audio | `apps/demo/demo_audio/` | Audio capture/playback/codec | `ac791n_demo_demo_audio` |
| demo_video | `apps/demo/demo_video/` | Video capture/JPEG encoding/recording | `ac791n_demo_demo_video` |
| demo_uvc | `apps/demo/demo_uvc/` | USB UVC camera | `ac791n_demo_demo_uvc` |
| demo_ui | `apps/demo/demo_ui/` | UI display/layers/touch | `ac791n_demo_demo_ui` |
| demo_hello | `apps/demo/demo_hello/` | Minimal boot project (getting-started skeleton) | `ac791n_demo_demo_hello` |
| demo_DevKitBoard | `apps/demo/demo_DevKitBoard/` | Official dev-board comprehensive demo | `ac791n_demo_demo_devkitboard` |
| demo_matter | `apps/demo/demo_matter/` | Matter IoT protocol (build from its board directory) | — |

> **Reference**: See the [AC79 module example documentation](https://doc.zh-jieli.com/AC79/zh-cn/release_v1.2.0/index.html) for detailed per-module development notes.

---

## 8. Build Guide

### 8.1 Build Command Cheat Sheet

Run the following commands from the SDK root:

| Target | Application | Command |
|--------|-------------|---------|
| WiFi Camera | wifi_camera | `make ac791n_wifi_camera` |
| WiFi IPC | wifi_ipc | `make ac791n_wifi_ipc` |
| WiFi Story Machine | wifi_story_machine | `make ac791n_wifi_story_machine` |
| Scanner Box | scan_box | `make ac791n_scan_box` |
| BLE Demo | demo_ble | `make ac791n_demo_demo_ble` |
| EDR Demo | demo_edr | `make ac791n_demo_demo_edr` |
| WiFi Demo | demo_wifi | `make ac791n_demo_demo_wifi` |
| Extended WiFi Demo | demo_wifi_ext | `make ac791n_demo_demo_wifi_ext` |
| Audio Demo | demo_audio | `make ac791n_demo_demo_audio` |
| Video Demo | demo_video | `make ac791n_demo_demo_video` |
| UVC Demo | demo_uvc | `make ac791n_demo_demo_uvc` |
| UI Demo | demo_ui | `make ac791n_demo_demo_ui` |
| Hello Demo | demo_hello | `make ac791n_demo_demo_hello` |
| Dev-Board Demo | demo_DevKitBoard | `make ac791n_demo_demo_devkitboard` |
| **All** | all | `make all` |
| **Clean all** | all | `make clean` |

### 8.2 Clean a Single Project

```bash
make clean_ac791n_wifi_camera     # Clean wifi_camera build artifacts
make clean_ac791n_demo_demo_ble   # Clean demo_ble build artifacts
```

### 8.3 Linux Build Notes

```bash
# 1. Ensure a large enough file-descriptor limit (linking opens many files)
ulimit -n 8096

# 2. Run a parallel build from the SDK root
make ac791n_wifi_camera -j`nproc`
```

### 8.4 Common Build Errors

| Error Message | Solution |
|---------------|----------|
| `clang: command not found` | Toolchain not installed, or environment variables not configured |
| `Too many open files` | On Linux, run `ulimit -n 8096` to raise the file-descriptor limit |
| `cannot find -lxxx` | Missing `.a` library; check the `cpu/wl82/liba/` and `lib/` directories |
| `undefined reference to ...` | Tailoring config does not include the module; check the configs under `apps/common/config/` |

---

## 9. Flashing & Upgrade

### 9.1 First-Time Flashing

1. **Connect the hardware**: connect the board to the PC via USB or UART
2. **Enter programming mode**: hold the flash button on the board, then reset or re-power
3. **Open the USB upgrade tool**: launch the flashing host software
4. **Select firmware**: choose the firmware file generated by the build
5. **Start flashing**: click the download button and wait for completion

> **⚠️ Note**: Before flashing, make sure the USB upgrade tool is properly connected and the target board is in programming mode.

### 9.2 Firmware Upgrade

The AC791N supports multiple upgrade methods — see the [system upgrade documentation](https://doc.zh-jieli.com/AC79/zh-cn/release_v1.2.0/index.html):

- **Single-backup upgrade**: USB drive / SD card
- **Dual-backup upgrade**: USB drive / SD card / WiFi (safer, supports rollback on failure)
- **Code dual-backup + partial resource backup + partial fixed resource** upgrade

---

## 10. Configuration

### 10.1 Feature Tailoring

Under the `apps/common/config/` directory, the following files let you flexibly tailor SDK features and reduce firmware size:

```bash
apps/common/config/
├── bt_profile_config.c     # Bluetooth Profile configuration
├── user_cfg.c              # User parameter configuration
├── new_cfg_tool.c          # Configuration tool
├── ci_transport_uart.c     # UART transport configuration
└── log_config/             # Log output configuration
```

### 10.2 Board Configuration

The `board_79xx_cfg.h` (per chip model, e.g. `board_7916A_cfg.h`) in each `apps/*/board/wl82/` directory contains:

- **Pin mapping**: pin assignment for UART / SPI / IIC / SDIO / GPIO and other peripherals
- **Peripheral enables**: enable or disable specific peripheral modules (audio, camera, panel, keys, etc.)
- **Clock configuration**: CPU frequency, peripheral clock sources
- **Feature switches & memory config**: enable features on demand, stack and buffer-pool sizes

---

## 11. FAQ

### 11.1 Development Workflow

**Q: How do I create my own project?**
A: Copy the closest solution project under `apps/`, enter its `board/wl82/` directory, copy and modify the `board_79xx_cfg.h` closest to your target chip model, and adjust the pin and peripheral configuration.

### 11.2 Build

**Q: On Windows, `make` is reported as an invalid command — what now?**
A: Use `tools/make_prompt.bat` to open the preconfigured build console, which sets the `make` path and environment variables for you.

**Q: How can I speed up builds?**
A: Use the `-j` flag for parallel builds, e.g. `make ac791n_wifi_camera -j4`.

### 11.3 Debugging Tips

- **Serial log**: configure log output level and channel via `apps/common/config/log_config/`
- **GPIO debug**: use a spare GPIO to output debug waveforms and measure timing
- **More issues**: see the [online FAQ documentation](https://doc.zh-jieli.com/AC79/zh-cn/release_v1.2.0/other/faq/index.html)

---

## 12. Community & Support

### Technical Exchange

| Platform | Group ID |
|----------|----------|
| **DingTalk Support Group 1** | `15375032297` |
| **DingTalk Support Group 2** | `31275808` |

### Resource Links

| Resource | Link |
|----------|------|
| 📖 **Online Documentation** | [doc.zh-jieli.com/AC79](https://doc.zh-jieli.com/AC79/zh-cn/release_v1.2.0/index.html) |
| 📄 **Chip Datasheet** | [AC791N Datasheet (local)](./doc/datasheet/AC791N规格书) |
| 📚 **Release Notes** | [Version history](https://doc.zh-jieli.com/AC79/zh-cn/release_v1.2.0/other/version/index.html) |
| 💻 **Gitee Repository** | [Jieli-Tech/fw-AC79_AIoT_SDK](https://gitee.com/Jieli-Tech/fw-AC79_AIoT_SDK) |
| 🐛 **Issue Tracker** | [Gitee Issues](https://gitee.com/Jieli-Tech/fw-AC79_AIoT_SDK/issues) |

---

## 13. Certification

The Bluetooth protocol stack and WiFi supported by this SDK are officially certified:

### Bluetooth Certification

Classic Bluetooth LMP / BLE Link Layer and the Host protocol stack both implement Bluetooth 5.0 and 5.1:

| Bluetooth Spec | QDID | Certification Link |
|----------------|------|--------------------|
| Core v5.0 | QDID 134104 | [View details](https://launchstudio.bluetooth.com/ListingDetails/88799) |
| Core v5.1 | QDID 136145 | [View details](https://launchstudio.bluetooth.com/ListingDetails/91371) |

### WiFi Certification

Wi-Fi Alliance interoperability testing has been completed, including WPA/WPA2-Personal security certification and B/G/N connectivity testing:

| Certification | ID | Certification Link |
|---------------|-----|--------------------|
| Wi-Fi CERTIFIED™ Interoperability | WFA100441 | [View details](https://www.wi-fi.org/content/search-page?keys=WFA100441) |

---

## 14. Versions & Branches

> The AC79 Gitee repository is updated frequently, especially the `master` branch used for developing new features.
> For testing / R&D / trying the latest features, use the `master` branch; for mass production, use a stable release (`release/AC79NN_SDK_Vx.x.x`). For a new project, the latest stable release is generally recommended.

### 14.1 Branch Strategy

| Branch | Purpose | Documentation |
|--------|---------|---------------|
| `release/AC79NN_SDK_V1.2.0` | Latest stable release (recommended for production) | [v1.2.0 docs](https://doc.zh-jieli.com/AC79/zh-cn/release_v1.2.0/index.html) |

### 14.2 Versioning (major.minor.patch)

- **major**: major version; significant changes, structure not backward-compatible
- **minor**: minor version; larger changes (e.g. API additions) without breaking source/binary compatibility
- **patch**: patch version; internal changes only, no API impact

> You can switch between AC79 documentation versions using the version drop-down menu at the top-left of the documentation page.

---

## 15. Disclaimer

`fw-AC79_AIoT_SDK` supports development on the AC791N series chips. The AC791N series supports common AIoT multimedia applications such as WiFi, Bluetooth, audio, video, and display, and can be used for development, evaluation, sampling, and mass production. For the corresponding SDK versions, see [Tags](https://gitee.com/Jieli-Tech/fw-AC79_AIoT_SDK/tags).

This SDK is open-sourced under the [Apache License 2.0](./LICENSE).

---

<div align="center">
  <sub>Copyright © 2024-2026 Zhuhai Jieli Technology Co., Ltd. All rights reserved.</sub>
</div>
