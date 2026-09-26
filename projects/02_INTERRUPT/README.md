# EXTI and NVIC

## Selected project

[`exti-callback/course/`](exti-callback/course/) contains the reusable EXTI example. It combines GPIO input configuration, EXTI line routing, NVIC priority, interrupt flag handling and a callback-facing interface.

SysTick timing is used in the button path to separate mechanical debounce timing from raw edge detection.

## Event flow

```text
Button edge
    ↓
GPIO input → EXTI line → NVIC → ISR
                                  ↓
                         clear pending flag
                                  ↓
                         callback / LED action
```

The Keil project is `course/Project/GD32F407.uvprojx`.

## Hardware relationship

The button GPIO is mapped to an EXTI line through the system configuration block. The EXTI controller detects the configured edge and forwards the pending request to an NVIC channel; the handler clears the pending state before dispatching the registered action.

## Key interfaces

- [`EXTI.h`](exti-callback/course/Library/EXTI.h) — initialization and callback-facing interface.
- [`EXTI.c`](exti-callback/course/Library/EXTI.c) — line routing, trigger and NVIC configuration.
- [`EXTI_config.h`](exti-callback/course/Library/EXTI_config.h) — project-level interrupt mapping.
- [`gd32f4xx_it.c`](exti-callback/course/User/gd32f4xx_it.c) — exception handlers.
