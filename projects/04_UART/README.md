# UART 与 DMA | UART and DMA

## 代表工程 | Selected Projects

- [`uart-callback/course/`](uart-callback/course/)：USART 初始化、收发接口、IRQ 处理与回调注册。
- [`uart-dma/course/`](uart-dma/course/)：基于 DMA 的 USART 收发与中断处理。

## 接收路径 | Receive Path

```text
USART RX event
      ↓
status / interrupt flag
      ↓
ISR or DMA transfer
      ↓
receive buffer
      ↓
callback / application handling
```

工程保留原始波特率和引脚配置，连接外部串口适配器前需要核对板卡 USART 通路与电平。

## 硬件关系 | Hardware Relationship

USART 引脚使用 GPIO Alternate Function。RCU 提供 GPIO 与 USART 时钟，NVIC 处理接收事件；DMA 版本在 USART Data Register 与内存缓冲区之间增加双向传输。

## 关键接口 | Key Interfaces

- [`USART0.h`](uart-callback/course/Library/USART0.h) 与 [`USART0.c`](uart-callback/course/Library/USART0.c)：串口接口、IRQ 配置和回调。
- [`DMA USART0.c`](uart-dma/course/Library/USART0.c)：DMA 传输配置。
- [`USART_config.h`](uart-dma/course/Library/USART_config.h)：USART 与 DMA 资源配置。
- [`gd32f4xx_it.c`](uart-dma/course/User/gd32f4xx_it.c)：接收与 DMA 中断处理。
