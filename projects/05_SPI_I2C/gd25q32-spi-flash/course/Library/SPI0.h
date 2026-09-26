#ifndef __SPI0_H__
#define __SPI0_H__

#include "gd32f4xx.h"

#define SPI0_USE_SOFT		1		// 0 硬实现，1 软实现

#define SPI0_SCL_RCU		RCU_GPIOA
#define SPI0_SCL_PORT		GPIOA
#define SPI0_SCL_PIN		GPIO_PIN_5

#define SPI0_MOSI_RCU		RCU_GPIOA
#define SPI0_MOSI_PORT	GPIOA
#define SPI0_MOSI_PIN		GPIO_PIN_7

#define SPI0_MISO_RCU		RCU_GPIOA
#define SPI0_MISO_PORT	GPIOA
#define SPI0_MISO_PIN		GPIO_PIN_6

void SPI0_init();

void SPI0_write(uint8_t data);

uint8_t SPI0_read();

uint8_t SPI0_read_write(uint8_t data);

#endif