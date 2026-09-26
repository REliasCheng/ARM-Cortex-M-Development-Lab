#ifndef __I2C_SOFT_H__
#define __I2C_SOFT_H__

#include "gd32f4xx.h"

void I2C_soft_init();

// 写数据
// addr: 设备地址
// 
int8_t I2C_soft_write(uint8_t addr, uint8_t reg, uint8_t* data, uint32_t len);

int8_t I2C_soft_read(uint8_t addr, uint8_t reg, uint8_t* data, uint32_t len);


#endif