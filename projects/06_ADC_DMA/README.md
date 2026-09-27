# ADC 与 DMA | ADC and DMA

## 代表工程 | Selected Projects

- [`adc-scan-dma/course/`](adc-scan-dma/course/)：常规通道扫描与 DMA 内存传输。
- [`adc-injected/course/`](adc-injected/course/)：注入通道组与结果读取。

## 采集路径 | Acquisition Path

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

Channel 顺序、Data Alignment、Trigger Source 与 DMA Transfer Width 需要相互一致。两个工程分别保留常规组和注入组的控制路径。

## 硬件关系 | Hardware Relationship

模拟功能 GPIO 连接 ADC Channel。常规组定义转换序列，并可在每次转换后请求 DMA；注入组使用独立的序列和结果寄存器处理更高优先级的采样路径。

## 关键接口 | Key Interfaces

- [`DMA scan main.c`](adc-scan-dma/course/User/main.c)：Channel Sequence、DMA Buffer 与前台处理。
- [`Injected conversion main.c`](adc-injected/course/User/main.c)：注入通道配置与结果读取。
- [`gd32f4xx_it.c`](adc-injected/course/User/gd32f4xx_it.c)：工程框架使用的中断入口。
