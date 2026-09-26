# Development environment

## Tooling found in the projects

- Keil MDK-ARM project files (`.uvprojx`)
- GD32F4 device pack and standard peripheral library
- STM32F4 CMSIS / standard peripheral library
- STM32CubeMX configuration files (`.ioc`)
- STM32 HAL sources generated for Keil MDK

No course-owned STM32CubeIDE project is included.

## Open a GD32 project

1. Install Keil MDK-ARM.
2. Install the GD32F4 device family pack required by the selected target.
3. Open the `.uvprojx` file under the project's `course/Project/` directory.
4. Confirm the selected device, startup file and include paths.
5. Select the intended probe and Flash algorithm before downloading.

## Open a CubeMX/HAL project

1. Open the `.ioc` file with a compatible STM32CubeMX version to inspect pin and clock configuration.
2. Open the Keil project under `course/MDK-ARM/` for compilation.
3. Confirm that the Keil device and pack match the STM32F407 target described by the `.ioc` file.

Several original STM32-named Keil projects select a GD32 device pack. The repository preserves those project files unchanged; correct the target only in a separate future `practice/` implementation.

## Build outputs

Generated `.o`, `.d`, `.crf`, `.map`, `.axf`, `.hex`, `.bin`, `.lst`, `.lnp`, `.dep` and IDE user-state files are excluded. Rebuilding creates them locally and `.gitignore` keeps them out of version control.

## Hardware checks

Before downloading firmware, verify the MCU marking, board power, BOOT setting, SWD wiring and serial port voltage level. The repository does not assign a fixed local COM port or probe configuration.

