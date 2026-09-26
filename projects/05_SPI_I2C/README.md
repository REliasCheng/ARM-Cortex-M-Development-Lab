# SPI, I²C and display devices

## Selected projects

- [`i2c-pcf8563/course/`](i2c-pcf8563/course/) — software/hardware I²C access and a PCF8563 device layer.
- [`oled-i2c-buffered/course/`](oled-i2c-buffered/course/) — buffered OLED updates and batched I²C writes.
- [`gd25q32-spi-flash/course/`](gd25q32-spi-flash/course/) — SPI transport and GD25Q32 external Flash operations.

## Bus boundaries

```text
Application
    ↓
Device interface: PCF8563 / OLED / GD25Q32
    ↓
Bus interface: I²C or SPI
    ↓
GPIO alternate function, clock and interrupt resources
```

![OLED I2C update diagram](../../assets/images/architecture/oled-i2c-update-1.png)

Device addresses, chip-select polarity and bus timing remain defined by the original projects and attached hardware.

