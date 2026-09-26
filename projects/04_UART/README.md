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

