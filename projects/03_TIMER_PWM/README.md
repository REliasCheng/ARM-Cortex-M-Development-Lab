# Timer and PWM

## Selected project

[`timer-pwm-buzzer/course/`](timer-pwm-buzzer/course/) retains the timer-library example connected to a buzzer output. It shows timer base configuration, channel setup and runtime output updates through a board-facing module.

## Timing relationship

```text
Timer input clock
        ↓
Prescaler → counter → auto-reload period
                         ↓
                 compare register
                         ↓
                  PWM output pin
                         ↓
                       buzzer
```

Timer input frequency can differ from the visible APB clock after prescaling. Check the project clock configuration before changing PWM frequency or duty cycle.

## Hardware relationship

The timer channel is routed to a GPIO alternate function. Prescaler and auto-reload values define the PWM period; the channel compare value controls the active portion of that period before the signal reaches the buzzer circuit.

## Key interfaces

- [`TIMER.c`](timer-pwm-buzzer/course/Library/TIMER.c) — timer base and channel configuration.
- [`TIMER_config.h`](timer-pwm-buzzer/course/Library/TIMER_config.h) — timer/channel selection.
- [`bsp_buzzer2.c`](timer-pwm-buzzer/course/Hardware/bsp_buzzer2.c) — buzzer-facing output control.
- [`main.c`](timer-pwm-buzzer/course/User/main.c) — application update flow.
