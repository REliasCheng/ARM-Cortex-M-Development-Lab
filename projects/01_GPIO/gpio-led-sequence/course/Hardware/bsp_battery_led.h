#ifndef __BSP_BATTERY_LEDS_H__
#define __BSP_BATTERY_LEDS_H__

#include "gd32f4xx.h"

#define LED1 1
#define LED2 2
#define LED3 3
#define LED4 4

void Battery_led_init();

void Battery_led_turn_on(uint8_t index);

void Battery_led_turn_off(uint8_t index);

// 开始充电（进入充电状态）
void Battery_led_start(uint8_t power);
// 停止充电
void Battery_led_stop();

// 轮询
void Battery_led_loop();

// 更新当前电量
void Battery_led_update(uint8_t power);
	
#endif