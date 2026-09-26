

#include "gd32f4xx.h"
#include "systick.h"
#include "bsp_battery_led.h"
#include "bsp_keys.h"
#include <stdio.h>
#include "main.h"
/*******************************************
任务目标：模拟充电宝指示灯

默认：LED都熄灭
充电中： 需要呈现出流水灯闪烁
充电结束：根据电量闪烁3次

测试案例设计：
1. 准备工作，4个灯，3个按钮
2. 按钮0按下时，模拟开始充电
3. 按钮1按下时，模拟停止充电
4. 按钮2按下时，模拟电量增加。

*******************************************/

void Keys_on_keyup(uint8_t index){
	
		// 熄灭
//		Battery_led_turn_off(1);
//		Battery_led_turn_off(2);
//		Battery_led_turn_off(3);
//		Battery_led_turn_off(4);
}

uint8_t power = 1;
void Keys_on_keydown(uint8_t index){

		// 点亮
//		if(index == 0) Leds_turn_on(1);
//		if(index == 1) Leds_turn_on(2);
//		if(index == 2) Leds_turn_on(3);
//		if(index == 3) Leds_turn_on(4);
		switch(index){
			case 0: Battery_led_start(power); break; // 0,1,2,3,4 开始充电
			case 1: Battery_led_stop(); break;   // 结束充电
			case 2: Battery_led_update(++power); break; 		 // 电量增加
			default:  break; 
		}
		
}

extern uint16_t period_1ms_num;
extern uint16_t period_10ms_num ;
extern uint16_t period_battery_num ;
extern uint16_t period_1000ms_num ;
	
int main(void)
{
  systick_config();
	
	Keys_init();
	Battery_led_init();
  
	uint32_t cnt = 0;
	while(1) {
		
		// 在循环内不断判断时间num，如果为0，需要执行了
		// 1ms
		if (period_1ms_num == 0){
			period_1ms_num = 1;
			
		}
		
		// 10ms
		if (period_10ms_num == 0){
			period_10ms_num = 10;
			Keys_scan();
			
			// 30ms
//			LED_refresh();
		}
		
		// 100ms
		if (period_battery_num == 0){
			period_battery_num = 100;
			Battery_led_loop();
		}
		
  }
}