# Project selection

Phase 1 identified 80 course Keil projects. This repository retains 19 projects that cover distinct firmware mechanisms without publishing every intermediate revision.

## Published projects

| Group | Source project | Reason retained |
| --- | --- | --- |
| GPIO | `03_GD32_Leds_Battery_Optimized3` | Final optimized GPIO/application sequence |
| Interrupt | `07_EXTI_Library` | EXTI, NVIC, callbacks and SysTick debounce |
| Timer/PWM | `02_Timer_Library_Buzzer` | Timer wrapper and PWM-driven buzzer |
| UART | `01_GD32_USART_Library_Optimize` | IRQ receive and callback interface |
| UART/DMA | `07_DMA_usart_Driver` | DMA TX/RX and serial driver integration |
| I²C | `05_I2C_Library` | Software/hardware I²C and PCF8563 access |
| OLED | `06_OLED_GD32_IIC_optimize` | Frame buffer and batched I²C update |
| SPI/Flash | `06_GD25Q32_FLASH_Library` | SPI abstraction and external Flash access |
| ADC/DMA | `03_ADC_routine_chns_DMA` | Regular channel scan with DMA |
| ADC injected | `05_ADC_inserted_chns` | Injected conversion group |
| RTC | `04_RTC_Alarm` | Clock source, backup domain and alarm |
| Watchdog | `05_WatchDog_FWDGT` | Independent watchdog configuration |
| Power | `01_GD32_PMU_mode` | Low-power entry and wake-up path |
| STM32 HAL | LED, USART, ADC, SPI and Timer CubeMX projects | HAL/CubeMX configuration across core peripherals |
| Firmware architecture | `04_Debug_project` | Hardware/Middleware split and debug-oriented skeleton |

## Reference-only material

Templates, vendor firmware packages, data sheets, intermediate revisions, duplicate examples, CircuitJS files, FreeRTOS concept diagrams and the unfinished balancing-car plan remain reference material outside this repository.

## Excluded material

Installers, license packages, programming tools, full manuals, complete vendor repositories, self-test HEX files, cached IDE state and generated build outputs were not migrated.

## Integrity

Each copied course file is hashed before and after migration. [`course-source-sha256.csv`](course-source-sha256.csv) records the source-relative path, repository path and both SHA-256 values. Every row must report `MATCH`.

