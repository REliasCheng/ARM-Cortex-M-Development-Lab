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

