# ARM Cortex-M Development Lab

Exploring ARM Cortex-M architecture, peripheral drivers and embedded firmware design on GD32F4 and STM32F4.

![Firmware layers](assets/images/architecture/firmware-stack.svg)

## Overview

This repository collects 19 focused Cortex-M4 firmware projects built around startup code, clock configuration, interrupts and peripheral data paths. GD32F407 projects use CMSIS and the GD32F4 standard peripheral library; STM32F407 projects add CubeMX configuration and HAL-based initialization.

The source is organized by firmware mechanism rather than chronological exercises. Each selected project keeps its Keil or CubeMX configuration beside the code it builds.

## Technical scope

### Hardware

- Cortex-M4 core, vector table, SysTick and NVIC
- RCU/RCC clock trees and AHB/APB peripheral clocks
- SkyStar core board, extension board and external devices

### Firmware

- GPIO, EXTI, timers and PWM
- Interrupt-driven USART and DMA transfers
- I²C, SPI, OLED, RTC and external Flash
- ADC regular/injected groups, watchdogs and PMU modes

### Development

- Keil MDK-ARM projects for GD32F407VE and STM32F407
- CMSIS, GD32 standard peripheral library and STM32 HAL
- STM32CubeMX `.ioc` configurations
- Hardware, library/middleware and application-facing modules

## Architecture

```text
Application
    ↓
Hardware modules / device interfaces
    ↓
Peripheral drivers and callbacks
    ↓
GD32 SPL / STM32 HAL
    ↓
CMSIS · startup · NVIC · SysTick
    ↓
Cortex-M4 hardware
```

The GD32 projects expose `User/`, `Hardware/`, `Library/` and later `Middleware/` boundaries. CubeMX projects use `Core/`, `Drivers/`, `MDK-ARM/` and `.ioc` files. See [Cortex-M architecture](docs/architecture.md), [interrupt system](docs/interrupt-system.md) and [firmware architecture](docs/firmware-architecture.md).

## Featured projects

### [GPIO and board output](projects/01_GPIO/)

GPIO clock, output mode and board-facing LED control separated from the application sequence.

### [EXTI and NVIC](projects/02_INTERRUPT/)

External interrupt routing, NVIC priority, pending-flag handling, callbacks and SysTick-based debounce.

### [Timer and PWM](projects/03_TIMER_PWM/)

Timer base configuration, channel compare values and PWM output connected to a buzzer module.

### [UART and DMA](projects/04_UART/)

Interrupt-driven USART receive, callback dispatch and DMA-backed serial transfers.

### [I²C and OLED](projects/05_SPI_I2C/)

Software/hardware I²C, PCF8563 access and buffered OLED updates with batched transfers.

### [SPI and external Flash](projects/05_SPI_I2C/#spi-flash-path)

SPI transport, chip-select control and GD25Q32 device operations.

### [ADC and DMA](projects/06_ADC_DMA/)

Regular-channel scan sequences, DMA buffer transfers and injected conversions.

### [RTC, watchdog and power](projects/07_FLASH_RTC_POWER/)

Backup-domain clocking, RTC alarm delivery, independent watchdog and PMU wake-up paths.

Five STM32CubeMX/HAL examples and the later firmware skeleton are grouped under [HAL and firmware structure](projects/08_HAL_AND_FIRMWARE_ARCHITECTURE/). The complete 19-project selection is recorded in [project selection](docs/project-selection.md).

## Hardware platforms

**GD32F407VE** is the primary target. The SkyStar core board exposes SWD, debug USART, USB device wiring, user LED/button, SDIO/TF, SPI Flash and expansion headers.

![SkyStar core board schematic](assets/images/hardware/skystar-core-board.png)

**STM32F407** projects provide standard-library and CubeMX/HAL counterparts. Their `.ioc` files target an STM32F407 LQFP100 device; the original Keil target configuration should be checked before building.

Hardware resources and shared peripheral constraints are summarized in [peripheral map](docs/peripheral-map.md).

## Build

1. Install Keil MDK-ARM and the device pack required by the selected target.
2. Open the `.uvprojx` file inside the project's `course/Project/` or `course/MDK-ARM/` directory.
3. Confirm the target device, startup file, include paths, Flash algorithm and probe configuration.
4. For HAL projects, inspect the adjacent `.ioc` file with STM32CubeMX before regenerating code.

Detailed setup notes are in [development environment](docs/development-environment.md). Generated binaries and IDE output are excluded from version control.

## Documentation

- [Cortex-M project architecture](docs/architecture.md)
- [Clock system](docs/clock-system.md)
- [Interrupt system](docs/interrupt-system.md)
- [Firmware layering](docs/firmware-architecture.md)
- [Peripheral map](docs/peripheral-map.md)
- [Project selection and integrity](docs/project-selection.md)

## Source and license

Selected source snapshots are stored under `projects/**/course/`; file-level SHA-256 records are kept in [`docs/course-source-sha256.csv`](docs/course-source-sha256.csv). Attribution and third-party terms are listed in [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md).

Related work: [Embedded Systems Foundations](https://github.com/REliasCheng/Embedded-Systems-Foundations) · [Embedded C/C++](https://github.com/REliasCheng/Embedded-C-Cpp-Learning) · [STC89C52](https://github.com/REliasCheng/stc89c52-learning) · [STC8](https://github.com/REliasCheng/STC8-MCU-Learning)
