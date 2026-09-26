#include "PCF8563.h"


void PCF8563_init() {
  // 配置GPIO引脚为开漏模式(OD, Open Drain)

  // 配置I2C外设
}

#define NUMBER 	7

// 设置时间
void PCF8563_set_clock(Clock_t c) {
  u8 Cent;
  u8 p[NUMBER] = {0};			 // 用于存储时间数据数组
// 将数值转成1个字节的表达（BCD）  BCD格式介绍 BCD(Binary-Coded Decimal)

  // 秒：VL 1 1 1 - 0 0 0 0 十进制转成BCD
  p[0] = ((c.second / 10) << 4) | (c.second % 10);

  // 分： X 1 1 1 - 0 0 0 0 十进制转成BCD
  p[1] = ((c.minute / 10) << 4) | (c.minute % 10);

  // 时： X X 1 1 - 0 0 0 0 十进制转成BCD
  p[2] = ((c.hour / 10) << 4) | (c.hour % 10);

  // 天： X X 1 1 - 0 0 0 0 十进制转成BCD
  p[3] = ((c.day / 10) << 4) | (c.day % 10);

  // 周： X X X X - X 0 0 0
  p[4] = c.week;

  // 世纪
  Cent = (c.year >= 2100) ? 1 : 0;
  // 月:  C X X 1 - 0 0 0 0 十进制转成BCD
  p[5] = (Cent << 7) | ((c.month / 10) << 4) | (c.month % 10);

  // 年： 1 1 1 1 - 0 0 0 0 十进制转成BCD
  p[6] = ((c.year % 100 / 10) << 4) | (c.year % 10);

  I2C_WRITE(PCF8563_ADDR, PCF8563_REG, p, NUMBER);
}

// 读取时间
void PCF8563_get_clock(Clock_t * c) { 
	u8 p[NUMBER] = {0};			 // 用于存储时间数据数组
	u8 Cent;
	
//	printf("pcf_c: %p\n", c);
	
  I2C_READ(PCF8563_ADDR, PCF8563_REG, p, NUMBER);
  // 秒：VL 1 1 1 - 0 0 0 0 BCD转成十进制
  c->second = ((p[0] >> 4) & 0x07) * 10 + (p[0] & 0x0F);
  // 分： X 1 1 1 - 0 0 0 0 BCD转成十进制
  c->minute = ((p[1] >> 4) & 0x07) * 10 + (p[1] & 0x0F);
  // 时： X X 1 1 - 0 0 0 0 BCD转成十进制
  c->hour   = ((p[2] >> 4) & 0x03) * 10 + (p[2] & 0x0F);
  // 天： X X 1 1 - 0 0 0 0 BCD转成十进制
  c->day    = ((p[3] >> 4) & 0x03) * 10 + (p[3] & 0x0F);
  // 周： X X X X - X 0 0 0
  c->week   = p[4] & 0x07;

  // 世纪
  // 月:  C X X 1 - 0 0 0 0 BCD转成十进制
  c->month  = ((p[5] >> 4) & 0x01) * 10 + (p[5] & 0x0F);
  Cent			 = p[5] >> 7;		// 0->20xx年， 1->21xx年
  // 年： 1 1 1 1 - 0 0 0 0 BCD转成十进制
  c->year   = ((p[6] >> 4) & 0x0F) * 10 + (p[6] & 0x0F);
  c->year  += ((Cent == 0) ? 2000 : 2100);
}

// 设置闹铃
void PCF8563_set_alarm(Alarm_t alarm){
	u8 a[4];
	
//	a[0] |= ((alarm.minute >= 0) ? 0x00 : 0x80);
	// 分 M 1 1 1 - 0 0 0 0 		Enable -> 0, Disable -> 1 << 7 (0x80)
	if(alarm.minute >= 0){
		a[0] = ((alarm.minute / 10) << 4) + (alarm.minute % 10) + 0x00;
	}else {
		a[0] = 0x80; // 禁用
	}
	
	// 时 H X 1 1 - 0 0 0 0 		Enable -> 0, Disable -> 1 << 7 (0x80)
	if(alarm.hour >= 0){
		a[1] = ((alarm.hour / 10) << 4) + (alarm.hour % 10) + 0x00;
	}else {
		a[1] = 0x80; // 禁用
	}
	
	// 天 D X 1 1 - 0 0 0 0 		Enable -> 0, Disable -> 1 << 7 (0x80)
	if(alarm.day >= 0){
		a[2] = ((alarm.day / 10) << 4) + (alarm.day % 10) + 0x00;
	}else {
		a[2] = 0x80;  // 禁用
	}
	
	// 周 W X X X - X 0 0 0 		Enable -> 0, Disable -> 1 << 7 (0x80)
	if(alarm.week >= 0){
		a[3] = alarm.week + 0x00;	// 启用
	}else {
		a[3] = 0x80;	// 禁用
	}
	
	I2C_WRITE(PCF8563_ADDR, 0x09, a, 4);
}

void PCF8563_clear_alarm(){
	u8 cs2;
	// 必须清理AF标记，设置为0，Alarm才能触发下一次中断
	I2C_READ(PCF8563_ADDR, 0x01, &cs2, 1);
	// 清除Alarm标记，AF设置为0，下次闹钟到点才能触发INT
	cs2 &= ~(1 << 3); // 0x08
	// 写回cs2寄存器
	I2C_WRITE(PCF8563_ADDR, 0x01, &cs2, 1);
}
	

void PCF8563_enable_alarm(u8 enable){
	u8 cs2;
	// 启用闹铃 0x01 cs2
	I2C_READ(PCF8563_ADDR, 0x01, &cs2, 1);
	// 清除Alarm标记，AF设置为0，下次闹钟到点才能触发INT
	cs2 &= ~(1 << 3); // 0x08
	// 开启Alarm中断，AIE设置为1
	if(enable){
		cs2 |= (1 << 1);	// 0x02
	}else{
		cs2 &=~(1 << 1);	// 0x02	
	}
	// 写回cs2寄存器
	I2C_WRITE(PCF8563_ADDR, 0x01, &cs2, 1);
}

// 设置定时器Timer
void PCF8563_set_timer(TimerFreq freq, u8 countdown){
	u8 p = 0;
	
	// 设置Timer运行频率、启用Timer source clock
	p = (1 << 7) + freq;	// 4096Hz, 64Hz, 1Hz, 1/60Hz
	I2C_WRITE(PCF8563_ADDR, 0x0E, &p, 1);
	// 设置Timer计数值
	p = countdown;
	I2C_WRITE(PCF8563_ADDR, 0x0F, &p, 1);
}

// 启用定时器Timer
void PCF8563_enable_timer(u8 enable){
	u8 cs2;

	I2C_READ(PCF8563_ADDR, 0x01, &cs2, 1);
	// 清除Timer标记，TF设置为0，下次Timer到点才能触发INT
	cs2 &= ~(1 << 2); // 0x04
	// 开启Timer中断，TIE设置为1
	if(enable){
		cs2 |= (1 << 0);	// 0x01
	} else {
		cs2 &=~(1 << 0);	// 0x01
	}
	// 写回cs2寄存器
	I2C_WRITE(PCF8563_ADDR, 0x01, &cs2, 1);
}

// 清理定时器Timer标记，避免下次无法触发
void PCF8563_clear_timer(){
	u8 cs2;

	I2C_READ(PCF8563_ADDR, 0x01, &cs2, 1);
	// 清除Timer标记，TF设置为0，下次Timer到点才能触发INT
	cs2 &= ~(1 << 2); // 0x04
	// 写回cs2寄存器
	I2C_WRITE(PCF8563_ADDR, 0x01, &cs2, 1);
}


void ext_int3_call(void) {

// ALARM和TIMER任意功能开启了，才读取寄存器
#if USE_ALARM || USE_TIMER
	u8 cs2;
	// 读取并判断Flag标记
	I2C_READ(PCF8563_ADDR, 0x01, &cs2, 1);
	
#if USE_ALARM
	// Alarm Flag && AIE
	if( (cs2 & 0x08) && (cs2 & 0x02) ){
		// 清理Alarm的AF标记，避免下次无法触发
		PCF8563_clear_alarm();
		
		// 当中断触发时的实现逻辑
		PCF8563_on_alarm();
	}
#endif
	
#if USE_TIMER
	// Timer Flag && TIE
	if( (cs2 & 0x04) && (cs2 & 0x01) ){
		// 清理定时器Timer标记，避免下次无法触发
		PCF8563_clear_timer();
		
		// 当中断触发时的实现逻辑
		PCF8563_on_timer();
	}
#endif
#endif
}
