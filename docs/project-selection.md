# 工程筛选记录 | Project Selection

源资料中识别出 80 个 Keil 工程。仓库保留 19 个技术差异明确的工程，不重复发布每个中间版本。

## 保留工程 | Selected Projects

| 分组 | 源工程 | 保留理由 |
| --- | --- | --- |
| GPIO | `03_GD32_Leds_Battery_Optimized3` | GPIO 与应用序列的最终优化版本 |
| Interrupt | `07_EXTI_Library` | EXTI、NVIC、回调和 SysTick 消抖 |
| Timer/PWM | `02_Timer_Library_Buzzer` | Timer 接口与 PWM 蜂鸣器驱动 |
| UART | `01_GD32_USART_Library_Optimize` | IRQ 接收与回调接口 |
| UART/DMA | `07_DMA_usart_Driver` | DMA TX/RX 与串口驱动组合 |
| I²C | `05_I2C_Library` | 软件/硬件 I²C 与 PCF8563 访问 |
| OLED | `06_OLED_GD32_IIC_optimize` | Frame Buffer 与批量 I²C 更新 |
| SPI/Flash | `06_GD25Q32_FLASH_Library` | SPI 接口与外部 Flash 访问 |
| ADC/DMA | `03_ADC_routine_chns_DMA` | 常规通道扫描与 DMA |
| ADC injected | `05_ADC_inserted_chns` | 注入通道转换组 |
| RTC | `04_RTC_Alarm` | 时钟源、备份域与 Alarm |
| Watchdog | `05_WatchDog_FWDGT` | 独立看门狗配置 |
| Power | `01_GD32_PMU_mode` | 低功耗进入与唤醒路径 |
| STM32 HAL | LED、USART、ADC、SPI 与 Timer CubeMX 工程 | HAL/CubeMX 外设配置 |
| Firmware architecture | `04_Debug_project` | Hardware/Middleware 分层与调试骨架 |

## 参考资料 | Reference Material

模板、厂商固件包、数据手册、中间版本、重复示例、CircuitJS 文件、FreeRTOS 概念图和平衡车规划没有进入项目主体。

## 未迁移内容 | Excluded Material

安装程序、授权包、下载工具、完整手册、厂商源码仓库、自测 HEX、IDE 缓存和构建输出均未迁移。

## 完整性 | Integrity

迁移文件的源路径、仓库路径与 SHA-256 记录在 [`course-source-sha256.csv`](course-source-sha256.csv) 中；`MATCH` 列用于核对复制前后的文件内容。
