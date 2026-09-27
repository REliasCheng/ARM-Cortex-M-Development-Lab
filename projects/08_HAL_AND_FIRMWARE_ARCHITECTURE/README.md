# STM32 HAL 与固件结构 | STM32 HAL and Firmware Structure

## CubeMX/HAL 工程

- [`stm32-hal-led/course/`](stm32-hal-led/course/)：GPIO 初始化。
- [`stm32-hal-usart/course/`](stm32-hal-usart/course/)：USART 配置与生成的初始化代码。
- [`stm32-hal-adc/course/`](stm32-hal-adc/course/)：ADC 配置。
- [`stm32-hal-spi/course/`](stm32-hal-spi/course/)：SPI 与 OLED 板级模块。
- [`stm32-hal-timer/course/`](stm32-hal-timer/course/)：Timer 配置。

各目录包含 `.ioc` 与 Keil 工程。`.ioc` 指向 STM32F407 LQFP100；部分原始 Keil 配置仍选择 GD32 Device Pack，打开时需要交叉核对目标器件。

## 固件骨架 | Firmware Skeleton

[`debug-middleware-skeleton/course/`](debug-middleware-skeleton/course/) 保存后期 GD32 调试工程。`Hardware/`、`Middleware/`、`User/` 与 `Project/` 目录展示 EXTI、I²C、SPI 和 USART/DMA 之间的接口边界。

该目录用于阅读固件分层和调试入口，不包含多外设应用状态机。
