# RTC、看门狗与电源管理 | RTC, Watchdog and Power

## 代表工程 | Selected Projects

- [`rtc-alarm/course/`](rtc-alarm/course/)：RTC 时钟源、备份域、Calendar/Alarm 与中断路径。
- [`independent-watchdog/course/`](independent-watchdog/course/)：FWDGT Prescaler/Reload 与喂狗流程。
- [`pmu-modes/course/`](pmu-modes/course/)：GD32 电源模式与唤醒处理。

## 系统关系 | System Relationship

```text
Low-speed clock → RTC / FWDGT
                     ↓
             alarm or timeout
                     ↓
                  NVIC/reset

Application → PMU mode → wake-up source → clock recovery
```

RTC Backup Domain 与 Watchdog Timeout 在不同复位和供电条件下具有不同保持行为，修改配置时需要同时核对时钟源和厂商参考手册。

## 关键接口 | Key Interfaces

- [`RTC alarm main.c`](rtc-alarm/course/User/main.c)：Backup Domain、Calendar 与 Alarm 配置。
- [`Watchdog main.c`](independent-watchdog/course/User/main.c)：FWDGT Timeout 与 Feed Sequence。
- [`PMU main.c`](pmu-modes/course/User/main.c)：低功耗进入与唤醒流程。
- [`PMU interrupt handlers`](pmu-modes/course/User/gd32f4xx_it.c)：唤醒相关中断路径。
