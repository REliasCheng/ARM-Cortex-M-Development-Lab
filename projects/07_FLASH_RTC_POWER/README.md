# RTC, watchdog and power management

## Selected projects

- [`rtc-alarm/course/`](rtc-alarm/course/) — RTC clock source, backup domain, calendar/alarm configuration and interrupt path.
- [`independent-watchdog/course/`](independent-watchdog/course/) — FWDGT prescaler/reload setup and feed sequence.
- [`pmu-modes/course/`](pmu-modes/course/) — GD32 power-management modes and wake-up handling.

## System relationship

```text
Low-speed clock → RTC / FWDGT
                     ↓
             alarm or timeout
                     ↓
                  NVIC/reset

Application → PMU mode → wake-up source → clock recovery
```

RTC backup-domain state and watchdog timeout survive different reset/power conditions. Changes should be checked against the selected clock source and vendor reference manual.

## Key interfaces

- [`RTC alarm main.c`](rtc-alarm/course/User/main.c) — backup-domain, calendar and alarm setup.
- [`Watchdog main.c`](independent-watchdog/course/User/main.c) — FWDGT timeout and feed sequence.
- [`PMU main.c`](pmu-modes/course/User/main.c) — low-power entry and wake-up flow.
- [`PMU interrupt handlers`](pmu-modes/course/User/gd32f4xx_it.c) — wake-up related interrupt path.
