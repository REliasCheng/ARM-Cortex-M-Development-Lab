# GPIO and board-level output

## Selected project

[`gpio-led-sequence/course/`](gpio-led-sequence/course/) contains the final optimized GD32 LED sequence project selected from three intermediate revisions.

The project separates board-facing LED operations from the sequence logic in `main.c`. GPIO clock enable, output mode, output type, speed and initial level are configured before application control begins.

## Signal path

```text
Application sequence
        ↓
LED interface
        ↓
GD32 GPIO configuration and bit operations
        ↓
SkyStar board LEDs
```

Open `course/Project/GD32F407.uvprojx` with Keil MDK after installing the GD32F4 device pack.

## Hardware relationship

The board LED interface is an output-only GPIO path. RCU enables the port clock before mode, output type, speed and initial level are applied. The key interface is configured separately as an input, keeping input sampling outside the LED driver.

## Key interfaces

- [`bsp_battery_led.h`](gpio-led-sequence/course/Hardware/bsp_battery_led.h) — board LED API.
- [`bsp_battery_led.c`](gpio-led-sequence/course/Hardware/bsp_battery_led.c) — GPIO initialization and output operations.
- [`bsp_keys.c`](gpio-led-sequence/course/Hardware/bsp_keys.c) — board key input.
- [`main.c`](gpio-led-sequence/course/User/main.c) — initialization and LED sequence.
