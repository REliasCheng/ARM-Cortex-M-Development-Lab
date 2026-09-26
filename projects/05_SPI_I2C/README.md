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

## I²C path

PCF8563 and OLED operations sit above a shared I²C interface. Device code owns register addresses and display commands; the bus layer owns START/STOP, address transfer, ACK handling and byte movement.

Key files: [`I2C0.c`](i2c-pcf8563/course/Library/I2C0.c), [`PCF8563.c`](i2c-pcf8563/course/Hardware/PCF8563.c), [`I2C.c`](oled-i2c-buffered/course/Library/I2C/I2C.c) and [`oled.c`](oled-i2c-buffered/course/Hardware/OLED/oled.c).

<a id="spi-flash-path"></a>

## SPI / Flash path

The Flash project separates SPI transport from GD25Q32 commands. The bus layer configures clock polarity/phase and byte transfers; the device layer owns chip select, command bytes, addresses and data operations.

Key files: [`SPI0.c`](gd25q32-spi-flash/course/Library/SPI0.c), [`spi_flash.h`](gd25q32-spi-flash/course/Hardware/spi_flash.h) and [`spi_flash.c`](gd25q32-spi-flash/course/Hardware/spi_flash.c).
