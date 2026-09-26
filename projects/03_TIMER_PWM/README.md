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

