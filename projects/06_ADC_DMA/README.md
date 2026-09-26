# ADC and DMA

## Selected projects

- [`adc-scan-dma/course/`](adc-scan-dma/course/) — regular-channel scan sequence with DMA transfer into memory.
- [`adc-injected/course/`](adc-injected/course/) — injected conversion group and result handling.

## Acquisition path

```text
Analog input
     ↓
ADC channel and sample time
     ↓
regular scan / injected sequence
     ↓
data register
     ↓
DMA buffer or CPU read
```

Channel order, data alignment, trigger source and DMA width must agree. The two retained projects keep regular and injected conversions separate so their control paths remain easy to inspect.

## Hardware relationship

Analog-capable GPIO pins feed ADC channels. The regular group defines a conversion sequence and can request DMA transfers after each conversion; the injected group uses its own sequence and result registers for higher-priority sampling.

## Key interfaces

- [`DMA scan main.c`](adc-scan-dma/course/User/main.c) — channel sequence, DMA buffer and foreground processing.
- [`Injected conversion main.c`](adc-injected/course/User/main.c) — injected-channel configuration and result access.
- [`gd32f4xx_it.c`](adc-injected/course/User/gd32f4xx_it.c) — interrupt integration used by the project framework.
