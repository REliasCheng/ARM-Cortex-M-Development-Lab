# ARM-Cortex-M-Development-Lab

面向 GD32F407 / STM32F407 Cortex-M4 的固件架构实验，包含一条可运行的 Host 定时采样控制链路。

**⚙️ Cortex-M Firmware Stack**

![Cortex-M firmware stack](assets/images/architecture/portfolio-overview.svg)

## Firmware Snapshot

| Firmware Focus | Current Scope |
| --- | --- |
| Repository Type | Cortex-M architecture and portable control software lab |
| Reference Platforms | GD32F407VE、STM32F407、ARM Cortex-M4 |
| Architecture | Application → Board Module → Driver → CMSIS / Vendor Interface |
| Documentation | Startup、clock、NVIC、interrupt、peripheral mapping、firmware layering |
| Public Implementation | Original C11 timer-event → sample → hysteresis → status-output Host core; no target driver |
| Verification | GCC/Clang/ASan/UBSan Host CI；target build and hardware evidence not provided |

## 📌 Overview

仓库整理 Cortex-M4 固件的启动链、时钟树、Vector Table、NVIC、中断回调、外设资源映射和固件分层方法。GD32 与 STM32 只作为平台参考，不被描述成当前已发布、可直接构建的完整工程。

当前默认分支已提供[可移植控制核心](docs/control-foundation.md)：定时事件进入有界队列，主循环读取模拟采样、更新迟滞状态并经可注入接口输出状态。它不访问真实外设。仓库不分发课程应用、CMSIS 副本、GD32/STM32 厂商库、startup 文件、CubeMX 工程或外部硬件图片。目标适配须从合规渠道取得组件并验证工具链、引脚与时钟。

## 🏗️ Architecture

![ARM Cortex-M firmware layering](assets/images/architecture/firmware-stack.svg)

```text
Application / System State
            ↓
Board Modules / Device Interfaces
            ↓
Peripheral Drivers / DMA / Callbacks
            ↓
Vendor Library Interface
            ↓
CMSIS / Startup / NVIC / SysTick
            ↓
Cortex-M4 Hardware
```

## ✨ Key Features

| Capability | Documentation Entry |
| --- | --- |
| Startup and vector-table model | [Architecture](docs/architecture.md) |
| Clock-tree reasoning | [Clock System](docs/clock-system.md) |
| Interrupt and NVIC boundary | [Interrupt System](docs/interrupt-system.md) |
| Firmware layer responsibilities | [Firmware Architecture](docs/firmware-architecture.md) |
| Peripheral resource mapping | [Peripheral Map](docs/peripheral-map.md) |
| Toolchain boundary | [Development Environment](docs/development-environment.md) |
| Tested control application boundary | [Timer-to-Sample Control](docs/control-foundation.md) |

## 📂 Project Structure

```text
ARM-Cortex-M-Development-Lab/
├── README.md
├── LICENSE
├── THIRD_PARTY_NOTICES.md
├── assets/images/architecture/  # Repository-authored SVG
├── include/, src/               # Original portable C11 control core
├── tests/, tools/               # Host regression tests and runnable demo
├── .github/workflows/           # GCC, Clang and sanitizer CI
└── docs/                        # Architecture and implementation boundaries
```

## 📚 Documentation

- [Architecture](docs/architecture.md)
- [Clock System](docs/clock-system.md)
- [Interrupt System](docs/interrupt-system.md)
- [Firmware Architecture](docs/firmware-architecture.md)
- [Peripheral Map](docs/peripheral-map.md)
- [Development Environment](docs/development-environment.md)
- [Timer-to-Sample Control Foundation](docs/control-foundation.md)
- [Local Asset and Source Boundary](docs/source-provenance.md)

## 🧪 Verification

| Verification Layer | Status | Boundary |
| --- | --- | --- |
| Host Test | VERIFIED | Original control core, injected board callbacks, fault and recovery tests, executable demo |
| Build Verification | NOT PROVIDED | No current Keil or CubeMX project is distributed |
| Hardware Validation | NOT PROVIDED | No reviewable GD32F407 or STM32F407 board record |
| Device Runtime Evidence | NOT PROVIDED | Host demo output is reproducible; no device serial log, waveform, or measurement result |

Host 复现命令见[控制核心说明](docs/control-foundation.md)。`ControlPort` 的采样和状态输出由 Host Mock 提供；测试不证明真实 Timer、ADC 或 UART 已在 GD32/STM32 开发板运行。

## License Boundary

根目录 MIT License 覆盖当前默认分支中仓库维护者编写的文档、配置、自绘 SVG、原创控制核心、测试与 Host Demo。课程源码、CMSIS、厂商库、startup、工程文件和外部图片未包含在当前默认分支；历史提交风险单独保留。详见 [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md)。
