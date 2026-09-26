#ifndef __SPI_FLASH_H__
#define __SPI_FLASH_H__

#include "gd32f4xx.h"
#include "SPI0.h"

#define BSP_SPI_NSS_RCU		RCU_GPIOA
#define BSP_GPIO_PORT			GPIOA
#define BSP_SPI_NSS				GPIO_PIN_4

#define spi_read_write_byte(dat) SPI0_read_write(dat)

#define W25QXX_CS_ON(x)		(gpio_bit_write(BSP_GPIO_PORT, BSP_SPI_NSS, x?1:0))

void bsp_spi_init(void);
uint8_t spi_read_write_byte(uint8_t dat);
uint16_t GD25Q32_readID(void);
void GD25Q32_write_enable(void);
void GD25Q32_wait_busy(void);
void GD25Q32_erase_sector(uint32_t addr);
void GD25Q32_write(uint8_t* buffer, uint32_t addr, uint16_t numbyte);
void GD25Q32_read(uint8_t* buffer,uint32_t read_addr,uint16_t read_length);



#endif