# UART and DMA

## Selected projects

- [`uart-callback/course/`](uart-callback/course/) — USART initialization, transmit/receive functions, IRQ handling and callback registration.
- [`uart-dma/course/`](uart-dma/course/) — DMA-backed USART transmit/receive flow with interrupt handling.

## Receive path

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

The projects preserve their original baud-rate and pin settings. Confirm the board's USART route and voltage level before connecting an external serial adapter.

## Hardware relationship

USART pins use GPIO alternate-function mode. RCU provides both GPIO and USART clocks; NVIC handles receive events, while the DMA version adds peripheral-to-memory and memory-to-peripheral transfers around the USART data register.

## Key interfaces

- [`USART0.h`](uart-callback/course/Library/USART0.h) and [`USART0.c`](uart-callback/course/Library/USART0.c) — serial API, IRQ setup and callbacks.
- [`DMA USART0.c`](uart-dma/course/Library/USART0.c) — DMA transfer configuration.
- [`USART_config.h`](uart-dma/course/Library/USART_config.h) — serial and DMA resource configuration.
- [`gd32f4xx_it.c`](uart-dma/course/User/gd32f4xx_it.c) — receive and DMA interrupt handling.
