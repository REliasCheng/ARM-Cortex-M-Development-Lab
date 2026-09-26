# Cortex-M firmware architecture

## Project startup

The retained bare-metal projects follow the standard Cortex-M startup path:

```mermaid
flowchart LR
    R[Reset] --> V[Vector table]
    V --> S[Startup assembly]
    S --> C[System clock setup]
    C --> M[main]
    M --> P[Peripheral initialization]
    P --> L[Main loop / interrupt callbacks]
```

The startup file supplies the vector table and reset entry. CMSIS defines core registers and exception interfaces. The vendor library then exposes GD32 RCU/GPIO/USART or STM32 HAL initialization functions.

## Interrupt path

Peripheral events are routed through NVIC entries into ISR code. Projects using callbacks keep device-facing initialization separate from the application action:

```text
GPIO edge / USART receive / timer update
                  ↓
          peripheral status flag
                  ↓
               NVIC
                  ↓
                 ISR
                  ↓
       clear flag / move data
                  ↓
       callback or application state
```

The EXTI and USART projects show this progression from direct ISR logic to callback registration and reusable interfaces.

## Data movement

The DMA examples configure source, destination, transfer width, direction and completion handling. USART DMA projects separate byte reception from CPU-driven polling, while ADC scanning uses DMA to move conversion sequences into memory.

## Firmware organization

The GD32 examples commonly contain:

- `Firmware/` — CMSIS and GD32F4 peripheral support.
- `Hardware/` — board-facing LED, key, USART, timer, display or sensor modules.
- `Middleware/` — protocol and service-facing interfaces in the later debug project.
- `User/` — `main.c`, interrupt handlers and application flow.
- `Project/` — Keil target configuration.

CubeMX projects use `Core/`, `Drivers/`, `MDK-ARM/` and an `.ioc` configuration file.

## Scope

All migrated examples are bare-metal firmware. The source collection contains RTOS concept diagrams, but no running FreeRTOS or RTX application has been included.

