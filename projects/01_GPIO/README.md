# GPIO 与板级输出 | GPIO and Board Output

## 代表工程 | Selected Project

[`gpio-led-sequence/course/`](gpio-led-sequence/course/) 保存 GD32 LED 序列的最终优化版本。工程将板级 LED 操作与 `main.c` 中的序列逻辑分开，并在应用控制前完成 GPIO 时钟、输出模式、输出类型、速度和初始电平配置。

## 信号路径 | Signal Path

```text
Application sequence
        ↓
LED interface
        ↓
GD32 GPIO configuration and bit operations
        ↓
SkyStar board LEDs
```

安装 GD32F4 Device Pack 后，可使用 Keil MDK 打开 `course/Project/GD32F407.uvprojx`。

## 硬件关系 | Hardware Relationship

板载 LED 是 GPIO 输出路径。RCU 先启用端口时钟，再配置模式、输出类型、速度和初始电平；按键接口单独配置为输入，不与 LED 驱动混用。

## 关键接口 | Key Interfaces

- [`bsp_battery_led.h`](gpio-led-sequence/course/Hardware/bsp_battery_led.h)：板级 LED 接口。
- [`bsp_battery_led.c`](gpio-led-sequence/course/Hardware/bsp_battery_led.c)：GPIO 初始化与输出操作。
- [`bsp_keys.c`](gpio-led-sequence/course/Hardware/bsp_keys.c)：板级按键输入。
- [`main.c`](gpio-led-sequence/course/User/main.c)：初始化与 LED 序列。
