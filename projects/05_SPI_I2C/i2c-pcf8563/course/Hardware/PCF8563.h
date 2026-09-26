#ifndef __PCF8563_H__
#define __PCF8563_H__

#include "gd32f4xx.h"
//#include "I2C_soft.h"
#include "I2C0.h"

// 启用设置1，禁用设置0
#define USE_ALARM		0
#define USE_TIMER		0

typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t 	u8;

#define PCF8563_ADDR	0x51			// A2 设备写地址
#define PCF8563_REG 	0x02			// 存储地址：时间的存储地址开始位置

// I2C写操作
#define I2C_WRITE(a, r, p, n) 	I2C0_write(a, r, p, n)
// I2C读操作
#define I2C_READ(a, r, p, n) 		I2C0_read(a, r, p, n)

// 准备 秒、分、时、天、周、世纪&月、年
//	year = 2024, month = 8, day = 10, week = 6;
//	hour = 23, minute = 59, second = 55;
typedef struct Clock{
	u16 year;
	u8 month; 
	u8 day;
	u8 week;
	u8 hour;
	u8 minute;
	u8 second;
} Clock_t;

typedef struct Alarm{
	signed char minute;	// [-128, 127]
	signed char hour;
	signed char day;
	signed char week;
} Alarm_t;


typedef enum {
	HZ4096 = 0,
	HZ64	 = 1,
	HZ1		 = 2,
	HZ1_60 = 3,
} TimerFreq;

	
void PCF8563_init();

// 设置时间
void PCF8563_set_clock(Clock_t clock);

// 读取时间 (声明)
void PCF8563_get_clock(Clock_t * clock);

// 设置闹铃
void PCF8563_set_alarm(Alarm_t alarm);

// 启用闹铃
void PCF8563_enable_alarm(u8 enable);

// 清理闹铃标记，避免下次无法触发
void PCF8563_clear_alarm();

// 设置定时器Timer
void PCF8563_set_timer(TimerFreq freq, u8 countdown);

// 启用定时器Timer
void PCF8563_enable_timer(u8 enable);

// 清理定时器Timer标记，避免下次无法触发
void PCF8563_clear_timer();

extern void PCF8563_on_alarm();

extern void PCF8563_on_timer();

void ext_int3_call(void);

#endif