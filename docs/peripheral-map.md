# 外设资源关系 | Peripheral Map

## 功能映射 | Functional Map

| 功能 | Cortex-M / 总线资源 | 保留实现 |
| --- | --- | --- |
| 板级输出 | GPIO | LED 序列优化版本 |
| 外部事件 | GPIO + EXTI + NVIC | EXTI 回调接口 |
| 周期输出 | Timer Channel + GPIO AF | PWM 蜂鸣器工程 |
| 串口通信 | USART + NVIC | USART 回调工程 |
| 缓冲串口传输 | USART + DMA + NVIC | USART DMA 驱动 |
| 双线器件 | GPIO 模拟或硬件 I²C | PCF8563 与 OLED 工程 |
| 同步串行总线 | SPI + 片选 GPIO | GD25Q32 Flash 工程 |
| 模拟采集 | ADC 常规组/注入组 | ADC DMA 与注入组工程 |
| 日历与闹钟 | RTC + Backup Domain + EXTI | RTC Alarm 工程 |
| 系统监控 | FWDGT | 独立看门狗工程 |
| 电源控制 | PMU + System Clock | 低功耗模式工程 |

## 板级资源 | Board Resources

SkyStar 核心板提供 SWD、调试 USART、USB Device、用户 LED/按键、SDIO/TF、SPI Flash 与扩展排针。扩展板增加 74HC595 驱动显示、矩阵键盘、PCF8563 RTC、蜂鸣器、OLED 接口和模拟输入。

![SkyStar 扩展板原理图](../assets/images/hardware/skystar-extension-board.png)

## 共享资源 | Shared Resources

引脚复用、DMA 通道与中断优先级由各工程独立配置。组合模块前需要核对：

1. GPIO Alternate Function 与电气模式。
2. APB/AHB 时钟门控和 Timer 时钟计算。
3. NVIC Priority Grouping 与共享中断处理函数。
4. DMA Controller、Channel 映射和传输方向。
5. SPI/I²C 的片选或设备地址归属。

仓库保留独立工程边界，不把不同项目的资源配置直接合并。
