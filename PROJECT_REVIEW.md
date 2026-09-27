# ARM Cortex-M Development Lab 源资料审查

## 审查结论

这批资料适合建设为 `ARM-Cortex-M-Development-Lab`，但不适合原样上传。

可公开内容的主体不是安装包和手册，而是 80 个按外设递进的 Keil 工程、7 个 STM32CubeMX 配置工程、GD32/STM32 双平台实现、天空星开发板原理图，以及围绕 GPIO、UART、EXTI、Timer/PWM、DMA、RTC、I2C、OLED、ADC、SPI 和外部 Flash 形成的驱动演进。

仓库定位：

> ARM Cortex-M firmware projects for GD32F4 and STM32F4, covering peripheral drivers, interrupt-driven design, DMA, communication interfaces and firmware modularization.

本文件记录建仓前的只读审查基线；公开仓库从这些资料中筛选代表工程，源目录保持不变。

## 1. 审查范围与方法

审查对象为 `04-ARM Cortex-M(STM32_GD32) 高级开发-配套资料` 的完整目录树。

执行内容：

- 递归统计外层文件和目录。
- 对 `.zip`、`.7z`、`.pack` 执行成员列表审查，共读取 36 个压缩容器。
- 将 day01-day14 的 `Code.zip` 解压到独立审查目录，读取 Keil、CubeMX、源码和构建文件。
- 解析 Keil `.uvprojx`、CubeMX `.ioc`、Draw.io、Excel 项目表、嘉立创 EDA `.epro` 和关键原理图。
- 对源目录建立 SHA-256 基线；审查结束后再次核验。

审查副本和统计文件位于独立目录，未写入源目录。

## 2. 文件统计

### 2.1 外层目录

| 指标 | 数量 |
| --- | ---: |
| 文件 | 110 |
| 目录 | 32 |
| 总大小 | 3,839,177,424 bytes（约 3.576 GiB） |
| ZIP | 28 |
| 7z | 6 |
| PACK | 2 |
| PDF | 22 |
| EXE | 6 |
| Markdown | 7 |
| CircuitJS 电路 | 8 |
| PNG | 3 |
| Draw.io | 2 |
| 嘉立创 EDA 工程 `.epro` | 2（内容相同） |
| HEX | 2 |
| Excel | 1 |

外层没有直接展开的 `.c`、`.h`、`.s` 和 Keil 工程；主要源码均封装在每日 `Code.zip` 和厂商资料包内。

### 2.2 全部压缩包成员

36 个压缩容器共列出 106,611 个成员文件。这里包含课程代码、STM32Cube 固件仓库、芯片支持包、CMSIS、u8g2 和厂商示例，因此不能把下表全部视为个人实践工程。

| 类型 | 数量 |
| --- | ---: |
| `.c` | 22,127 |
| `.h` | 16,263 |
| `.s` | 5,133 |
| `.ld` | 1,342 |
| `.uvprojx` | 1,420 |
| `.uvproj` | 17 |
| `.ioc` | 9 |
| `.project` | 1,252 |
| `.cproject` | 1,252 |

其中 1,245 个 `.uvprojx` 和 1,232 组 `.project/.cproject` 来自 CubeMX Repository，不是主课程工程。

### 2.3 day01-day14 课程代码包

| 指标 | 数量 |
| --- | ---: |
| 压缩包成员文件 | 21,266 |
| C 源文件 | 6,129 |
| 头文件 | 6,945 |
| 汇编文件 | 898 |
| Keil scatter 文件 `.sct` | 92 |
| GNU linker 文件 `.ld` | 5 |
| Keil `.uvprojx` | 80 |
| CubeMX `.ioc` | 7 |
| CubeIDE `.project/.cproject` | 0 |
| 图片 | 217 |
| 已生成构建文件 | 4,041 |

根目录 `Template.zip` 另含 3 个可复用模板；它们与每日工程存在重复，不计入 80 个课程工程。

构建文件包括 `.o`、`.d`、`.crf`、`.map`、`.axf`、`.hex`、`.bin`、`.lst`、`.lnp`、`.dep` 和 `.bak`。这些文件不应进入公开仓库。

## 3. 工程识别

### 3.1 主工程类型

| 工程类型 | 实际情况 | 结论 |
| --- | --- | --- |
| Keil MDK | day01-day14 共 80 个 `.uvprojx` | 主开发环境 |
| GD32 标准外设库 / CMSIS | 课程序列中约 63 个工程 | 主平台 |
| STM32 标准外设库 / CMSIS | 课程序列中约 10 个工程 | 对照平台 |
| STM32 HAL / CubeMX | 7 个 `.ioc` + 对应 Keil 工程 | 可建立独立 HAL 区域 |
| STM32CubeIDE | 每日代码中没有 `.project/.cproject` | 不应作为已完成开发环境宣传 |
| 裸机固件 | 80 个每日工程均未接入运行中的 RTOS | 可以描述为 bare-metal firmware |
| RTOS | 有 `FreeRTOS.drawio` 和已移除的 RTX 配置痕迹 | 仅概念资料，不能建立 RTOS 成果目录 |
| USB | 板卡原理图、启动文件和厂商库包含 USB | 没有发现独立 USB 应用工程 |

### 3.2 目标器件与开发板

已从工程配置、原理图和资料文件中确认：

- 主要 GD32 Keil 目标为 `GD32F407VE`，CPU 配置为 Cortex-M4 + FPU。
- 主板资料为立创梁山派·天空星核心开发板及扩展板；核心板原理图采用 100 引脚 MCU 结构。
- STM32CubeMX 工程使用 `STM32F407V(E-G)Tx`、LQFP100、自定义板卡配置。
- STM32 标准库示例还包含 `STM32F407ZG` OLED 工程。
- day14/day15 含一份 `GD32F427RKT6` 嘉立创 EDA 原生工程，内部包含 1 份原理图和 1 份 PCB 文件。两天的 `.epro` SHA-256 完全相同，应只保留一份参考副本。

注意：若干名称为 STM32 的 Keil 工程仍在 `.uvprojx` 中选择 `GD32F407VE/GD32F470ZG` 和 GigaDevice DFP，而对应源码或 `.ioc` 指向 STM32F407。公开文档保留这项配置差异，原工程文件不作改写。

### 3.3 硬件资料

天空星核心板原理图包含：

- USB 2.0 device 接口。
- SWD 调试与下载接口。
- USART0 调试串口。
- 用户 LED、按键和 BOOT 配置。
- SDIO/TF 卡。
- SPI Flash。
- 双排扩展引脚与供电接口。

天空星扩展板原理图包含：

- SN74HC595 驱动的 8 位数码管。
- 4x4 矩阵键盘、独立按键。
- PCF8563 RTC。
- 有源/无源蜂鸣器与振动电机。
- SPI OLED、I2C OLED、1.69 寸触摸屏接口。
- DHT11、NTC、电位器。
- LED、I2C 扩展接口。

交互扩展板原理图包含：

- 电流和电压检测。
- WS2812。
- I2C 屏幕、ADC 按键。
- 188 数码管、旋转编码器、点阵屏。
- SC12B 触摸按键。

`项目开发文档-GD32天空星.xlsx` 记录了引脚复用、板卡资源和二轮平衡车模块规划。表内多数模块标记为“未完成”，因此它可作为资源规划参考，不能作为已完成综合项目发布。

## 4. 技术内容与证据

| 技术主题 | 实际工程或资料 | 可公开结论 |
| --- | --- | --- |
| Cortex-M / CMSIS / 启动 | startup 汇编、system 文件、CMSIS、Keil scatter 文件、`ARM32.drawio` | 可说明启动链、异常向量、NVIC 和 Cortex-M4 工程组成 |
| 时钟系统 | GD32 RCU、STM32 RCC、HXTAL/IRC32、RTC 时钟源工程 | 可形成时钟与外设时钟文档 |
| GPIO | 输入、LED、推挽/开漏、按键、电池灯序列及优化版本 | 适合展示寄存器/库函数、模式配置和板级 IO |
| EXTI / NVIC | PA0、PC0、PC1、软件触发、优先级、封装库 | 可展示外部中断、优先级和 SysTick 防抖 |
| UART / USART | TX、RX、IRQ、回调、封装库、DMA 收发 | 可展示中断驱动通信与回调接口 |
| Timer / PWM | 基本/通用/高级定时器、PWM、互补极性、蜂鸣器、LED | 可展示定时、通道配置与 PWM 驱动 |
| DMA | memory-to-memory、动态地址、USART TX/RX、驱动封装 | 适合展示 CPU 与外设之间的数据搬运 |
| RTC / Watchdog | HXTAL/IRC32、备份域、Alarm、FWDGT、WWDGT | 可展示时钟源、备份域与系统可靠性 |
| I2C | 软件 I2C、硬件 I2C、START/STOP/ACK、PCF8563 | 适合展示协议时序和统一读写接口 |
| OLED / 数码管 | I2C OLED、SPI OLED、u8g2、帧缓存更新、数码管 | 可展示显示缓冲、批量传输和显示驱动封装 |
| ADC | 内部温度、常规组、扫描、DMA、非 DMA、注入组 | 适合展示采样序列、触发和 DMA 配合 |
| SPI / External Flash | 软件/硬件 SPI、GD25Q32、SPI OLED | 可展示全双工传输、片选和 Flash 访问 |
| PMU | GD32 低功耗模式工程 | 可展示睡眠、深睡眠和时钟恢复 |
| STM32 HAL / CubeMX | LED、USART、ADC、I2C、SPI、Timer | 可展示 CubeMX 生成工程与 HAL 外设配置 |
| 调试 | 链接错误记录、Debug project、串口重定向、DMA UART | 可整理构建、下载和运行时调试方法 |
| RTOS | 4 页 FreeRTOS 概念图，没有实际任务工程 | 只保留为参考，不列为已完成项目 |
| USB | 原理图与厂商库可见，没有独立应用 | 不建立 USB 项目 |

## 5. 工程演进

源码展示了较清晰的固件演进，不应在公开仓库中继续按 day 编号平铺：

1. GD32/STM32 模板、startup、CMSIS 和外设库。
2. GPIO 输入输出及 LED 控制。
3. USART 收发、中断回调和接口封装。
4. EXTI/NVIC、SysTick 计时与按键防抖。
5. Timer/PWM 与硬件驱动封装。
6. DMA 内存搬运及 USART DMA 收发。
7. RTC、备份域、Alarm 与看门狗。
8. 软件/硬件 I2C、PCF8563 和 OLED。
9. ADC 多通道、DMA 与注入组。
10. 软件/硬件 SPI、OLED 和 GD25Q32 Flash。
11. PMU 低功耗。
12. STM32CubeMX + HAL 外设工程。
13. I2C/SPI/USART/EXTI 中间层与调试工程。

## 6. 价值分类

### A 类：适合公开展示

建议从重复工程中保留最终或最有差异的版本：

| 公开主题 | 推荐源工程 | 选择理由 |
| --- | --- | --- |
| GPIO 与板级输入输出 | `day04/03_GD32_Leds_Battery_Optimized3` | 保留优化后的 GPIO 与应用逻辑，而非所有中间版本 |
| UART 驱动 | `day05/01_GD32_USART_Library_Optimize` | 包含收发、IRQ、回调和接口封装 |
| EXTI / NVIC | `day05/07_EXTI_Library` | 包含外部中断、优先级、回调和 SysTick 防抖 |
| Timer / PWM | `day07/01_Timer_Library` 与 `02_Timer_Library_Buzzer` | 定时器、PWM、通道更新与硬件驱动关系清晰 |
| UART + DMA | `day07/07_DMA_usart_Driver` | 包含 DMA TX/RX 和中断接收流程 |
| RTC / Watchdog | day08 的 RTC、FWDGT、WWDGT 代表工程 | 覆盖时钟源、备份域、闹钟和可靠性机制 |
| I2C / PCF8563 | `day09/05_I2C_Library` | 软件/硬件 I2C 和设备层接口都存在 |
| OLED | `day10/06_OLED_GD32_IIC_optimize` | 显示缓冲与批量 I2C 写入有明确优化过程 |
| ADC | `day11/03_ADC_routine_chns_DMA`、`05_ADC_inserted_chns` | 覆盖扫描、DMA 和注入组 |
| SPI / Flash | `day12/06_GD25Q32_FLASH_Library` | SPI 抽象、外部 Flash 与显示模块关系完整 |
| 低功耗 | `day13/01_GD32_PMU_mode` | 有独立 PMU 工程 |
| STM32 HAL | day13-day14 的 7 个 CubeMX/HAL 工程 | 展示从 SPL 到 HAL/CubeMX 的对照 |
| 固件分层与调试 | `day14/04_Debug_project` | 存在 Middleware、USART DMA、I2C、SPI、EXTI；应定位为调试/中间层骨架，不称为综合应用 |

可使用的真实视觉素材：

- 天空星核心板和扩展板原理图局部。
- 交互扩展板原理图局部。
- `ARM32.drawio` 中与 Cortex-M、NVIC、外设总线有关的页面。
- 3 张 OLED 帧缓存/I2C 批量写入技术图。

### B 类：保留作学习与工程参考

- GD32/STM32 模板工程和 RTE 模板。
- 芯片数据手册、用户手册、器件规格书。
- GD32/STM32 官方固件库、CMSIS、DFP 和 CubeMX Repository。
- `FreeRTOS.drawio` 概念图。
- `项目开发文档-GD32天空星.xlsx` 中的引脚表和未完成项目规划。
- `GD32F427RKT6` 原理图/PCB EDA 工程；它是硬件参考资产，不等于已完成打样或板级验证。
- CircuitJS GPIO/晶体管电路。
- 中间步骤、Bug/NoBug 对比、功能高度重复的前置工程。

B 类资料应优先转化为引用、分析和链接，不应整包复制到公开仓库。

### C 类：不上传

- Keil、CubeMX、Bandizip、程序下载工具、串口工具、分压计算器等安装程序。
- license 压缩包、驱动安装包和无关工具。
- `Repository.7z`、DFP、CMSIS、完整厂商固件库、u8g2 源仓压缩包。
- 完整 PDF 手册和商业配套资料；公开文档改为链接厂商原始页面。
- `.o`、`.d`、`.crf`、`.map`、`.axf`、`.hex`、`.bin`、`.lst`、`.lnp`、`.dep`、`.bak` 等构建产物。
- `.DS_Store`、IDE 临时状态和缓存。
- 重复的 EDA 工程和重复模板。
- 课堂工程邀请链接、课程封面、安装截图和纯教学材料。
- 自测 HEX；它们缺少可审查源码，不适合作为公开技术成果。

## 7. 仓库结构设计

仓库名称：

`ARM-Cortex-M-Development-Lab`

公开目录按固件机制组织，不沿用 day 编号：

```text
ARM-Cortex-M-Development-Lab/
├── README.md
├── LICENSE
├── THIRD_PARTY_NOTICES.md
├── .gitignore
├── assets/
│   └── images/
├── docs/
│   ├── cortex-m-project-structure.md
│   ├── hardware-platforms.md
│   ├── clock-and-interrupts.md
│   └── development-environment.md
└── projects/
    ├── 01-core-and-gpio/
    ├── 02-interrupts-and-timers/
    ├── 03-uart-and-dma/
    ├── 04-i2c-spi-and-display/
    ├── 05-adc-rtc-watchdog-pmu/
    ├── 06-stm32-hal-cubemx/
    └── 07-firmware-architecture/
```

每个代表项目内部使用：

```text
course/    # 原工程，保持内容和文件名
docs/      # 项目说明和接口分析
practice/  # 仅保存已有的独立实现
```

现有资料中的 RTOS 和 USB 内容没有形成独立应用工程，因此不设置对应项目目录。

## 8. 迁移规则

1. 只迁移 A 类代表工程，不复制全部 80 个工程。
2. 课程工程放在 `course/`，迁移前后对每个文件计算 SHA-256。
3. 不修改函数、变量、格式或工程配置；发现配置问题只写说明。
4. 删除构建产物后再形成公开副本。
5. 图片仅选原理图局部、Draw.io 技术图和 OLED 技术图，并在 `assets/images/SOURCES.md` 记录来源。
6. 数据手册和固件包使用厂商链接，不直接入库。
7. STM32 HAL 工程单独成组，避免与 GD32 SPL 工程混写。
8. README 中只描述已存在工程；RTOS、USB、完整 PCB 制造和综合应用不作完成声明。

## 9. 配置与来源注意事项

- 多个 STM32 工程的 Keil Device/Pack 仍指向 GD32；公开仓库保留原工程，并在开发环境文档中说明配置差异。
- 课程代码包含 4,041 个构建产物，需要建立严格 `.gitignore` 和迁移过滤清单。
- 每日“知识整理”是累积复制文档，内容重复，不应逐日公开。
- 部分源码注释存在旧编码显示问题，不应在课程原版上批量转码。
- `FreeRTOS.drawio` 只有概念图，尚无可公开的 RTOS 代码工程。
- CubeIDE 工程只存在于厂商资料包，不属于课程主实践。
- `项目开发文档-GD32天空星.xlsx` 中多数综合模块未完成，不应转述为已实现项目。
- `.epro` 内含 PCB 文件，但没有 DRC、Gerber、打样和实物验证记录。

## 10. 审查基线

```text
REVIEW_MODE=READ_ONLY_AUDIT
SOURCE_FILES=110
SOURCE_BYTES=3839177424
ARCHIVES_INDEXED=36
ARCHIVE_MEMBERS=106611
COURSE_KEIL_PROJECTS=80
CUBEMX_PROJECTS=7
CUBEIDE_COURSE_PROJECTS=0
SOURCE_MODIFIED=NO
REVIEW_TIME_GIT_REPOSITORY_CREATED=NO
REVIEW_TIME_PUBLIC_FILES_MIGRATED=NO
```
