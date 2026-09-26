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

