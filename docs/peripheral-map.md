# 外设资源关系 | Peripheral Map

## 功能映射 | Functional Map

| 功能 | Cortex-M / 总线资源 | 设计检查点 |
| --- | --- | --- |
| 板级输出 | GPIO | 有效电平、复位状态与驱动能力 |
| 外部事件 | GPIO + EXTI + NVIC | 触发边沿、去抖与 pending flag |
| 周期输出 | Timer Channel + GPIO AF | Timer clock、channel 与 pin AF |
| 串口通信 | USART + NVIC | 波特率、错误处理与缓冲区 |
| 缓冲串口传输 | USART + DMA + NVIC | DMA mapping、长度与所有权 |
| 双线器件 | GPIO 模拟或硬件 I²C | 引脚、地址、时序与上拉 |
| 同步串行总线 | SPI + 片选 GPIO | Mode、clock、chip select |
| 模拟采集 | ADC 常规组/注入组 | 采样时间、触发源与 DMA |
| 日历与闹钟 | RTC + Backup Domain + EXTI | 时钟源、备份域与 Alarm |
| 系统监控 | FWDGT | 喂狗窗口与故障策略 |
| 电源控制 | PMU + System Clock | 唤醒源与时钟恢复 |

## 板级资源 | Board Resources

当前默认分支不分发开发板原理图。板级资源必须以实际硬件原理图、芯片数据手册和器件包文档为准，不能从本文档推断具体连接。

## 共享资源 | Shared Resources

引脚复用、DMA 通道与中断优先级由各工程独立配置。组合模块前需要核对：

1. GPIO Alternate Function 与电气模式。
2. APB/AHB 时钟门控和 Timer 时钟计算。
3. NVIC Priority Grouping 与共享中断处理函数。
4. DMA Controller、Channel 映射和传输方向。
5. SPI/I²C 的片选或设备地址归属。

未来实现应保持独立工程边界，不把不同参考项目的资源配置直接合并。
