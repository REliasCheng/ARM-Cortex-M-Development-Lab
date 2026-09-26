# STM32 HAL and firmware structure

## CubeMX/HAL projects

- [`stm32-hal-led/course/`](stm32-hal-led/course/) — GPIO initialization.
- [`stm32-hal-usart/course/`](stm32-hal-usart/course/) — serial configuration and generated initialization.
- [`stm32-hal-adc/course/`](stm32-hal-adc/course/) — ADC configuration.
- [`stm32-hal-spi/course/`](stm32-hal-spi/course/) — SPI and OLED-facing hardware module.
- [`stm32-hal-timer/course/`](stm32-hal-timer/course/) — timer configuration.

Each directory contains an `.ioc` file and a Keil project. The `.ioc` files identify the STM32F407 LQFP100 target; the original Keil configurations should be checked because some retain a GD32 device-pack selection.

## Firmware skeleton

[`debug-middleware-skeleton/course/`](debug-middleware-skeleton/course/) contains the later GD32 debug project. Its `Hardware/`, `Middleware/`, `User/` and `Project/` directories show an interface split across EXTI, I²C, SPI and USART/DMA code.

This directory is presented as a firmware and debugging skeleton, not as a completed multi-peripheral application.

