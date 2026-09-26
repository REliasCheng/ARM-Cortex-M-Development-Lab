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

