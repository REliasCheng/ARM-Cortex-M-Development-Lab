# 定时器与 PWM | Timer and PWM

## 代表工程 | Selected Project

[`timer-pwm-buzzer/course/`](timer-pwm-buzzer/course/) 保存 Timer 接口与蜂鸣器输出组合工程，包含定时器时基、Channel 配置和运行时输出更新。

## 时序关系 | Timing Relationship

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

APB 分频后，Timer 输入时钟可能与可见的 APB 时钟不同；修改 PWM 频率或占空比前需要核对工程时钟配置。

## 硬件关系 | Hardware Relationship

Timer Channel 通过 GPIO Alternate Function 输出。Prescaler 与 Auto-reload 定义 PWM 周期，Compare Value 决定有效时间，再由板级模块连接蜂鸣器电路。

## 关键接口 | Key Interfaces

- [`TIMER.c`](timer-pwm-buzzer/course/Library/TIMER.c)：Timer 时基与 Channel 配置。
- [`TIMER_config.h`](timer-pwm-buzzer/course/Library/TIMER_config.h)：Timer/Channel 选择。
- [`bsp_buzzer2.c`](timer-pwm-buzzer/course/Hardware/bsp_buzzer2.c)：蜂鸣器输出控制。
- [`main.c`](timer-pwm-buzzer/course/User/main.c)：应用更新流程。
