#include "gd32f4xx.h"
#include "systick.h"
#include <stdio.h>
#include <string.h>
#include "main.h"

#include "USART0.h"

/*******************************************
任务目标： 初始化并配置RTC时钟

- 设置时钟
	1. 解除备份域寄存器的写保护
	2. 设置时钟
	3. 开启RTC时钟
- 读取时钟

*******************************************/
void USART0_on_recv(uint8_t* buffer, uint32_t len) {
  printf("recv[%d]-> %s\n", len, buffer);

}

void RTC_config() {
  // 初始化RTC时钟，依赖PMU
	// 1. 解除备份域寄存器的写保护
	rcu_periph_clock_enable(RCU_PMU);
	/* enable the access of the RTC registers */
	pmu_backup_write_enable();
	
	/* 重置备份域（不重置可能会无法设置晶振，出现RTC时钟不走字） */
	/* reset backup domain */
//	rcu_bkp_reset_enable();
//	rcu_bkp_reset_disable();
	
	// 2. 设置时钟
	rcu_osci_on(RCU_HXTAL);
	/* 等待晶振稳定 */
	rcu_osci_stab_wait(RCU_HXTAL);
	
//	/* 选择内部32K作为RTC晶振 select RCU_RTCSRC_IRC32K as RTC clock source */
//	rcu_rtc_clock_config(RCU_RTCSRC_IRC32K);
	
	/* 选择外部HXTAL作为RTC晶振 select RCU_RTCSRC_HXTAL as RTC clock source */
	rcu_rtc_clock_config(RCU_RTCSRC_HXTAL_DIV_RTCDIV);
	/* 配置分频系数 HXTAL时需要配置*/
	rcu_rtc_div_config(RCU_RTC_HXTAL_DIV25);
	
	/* 3. 开启RTC时钟 enable RTC Clock */
	rcu_periph_clock_enable(RCU_RTC);
	/* wait until RTC_TIME and RTC_DATE registers are synchronized with APB clock */
	rtc_register_sync_wait();
}

// 十位取出左移4位 + 个位 (得到BCD数)
#define WRITE_BCD(val) 	((val / 10) << 4) + (val % 10)
// 将高4位乘以10 + 低四位 (得到10进制数)
#define READ_BCD(val) 	(val >> 4) * 10 + (val & 0x0F)

void RTC_write(){
	// 不可以在RTC_config之后调用rtc_deinit,会导致时钟不走字
	rtc_parameter_struct rps; // 数据格式BCD格式
	rps.year				= WRITE_BCD(24);
	rps.month 			= WRITE_BCD(9); // [1, 12]RTC_SEP
	rps.date  			= WRITE_BCD(30); // [1, 31]
	rps.day_of_week = WRITE_BCD(1); // [1, 7]
	rps.hour				= WRITE_BCD(23);
	rps.minute			= WRITE_BCD(59);
	rps.second		  = WRITE_BCD(55);
	rps.am_pm				= RTC_PM;
	rps.display_format = RTC_24HOUR;
	
  rps.factor_asyn = 0x7F;             // 7bit 异步分频器 0x00 - 0x7F   
  rps.factor_syn  = 0x09C3;  					  //15bit 同步分频器 0x00 - 0x7FFF
	// ck_spre == 1 --> 1个时钟1秒 1Hz
	// ck_spre = rtc_clk / ((factor_asyn + 1) * (factor_syn + 1))
	// HXTAL / 25：
	// ck_spre：1, rtc_clk: 8M / 25 = 320k, factor_asyn: 0x7F
	// 1 = 320k / (0x80 * (factor_syn + 1))
	// factor_syn = 320k / 128 - 1
	//						= 2499		(0x09C3)
	
	/* initialize RTC registers */
	rtc_init(&rps);
}

void RTC_read(){
	rtc_parameter_struct rps; 
	/* 获取当前的日期时间 */
	rtc_current_time_get(&rps);
	
	uint16_t year 			= READ_BCD(rps.year) + 2000;
	uint8_t month 			= READ_BCD(rps.month);
	uint8_t date 				= READ_BCD(rps.date);
	
	uint8_t hour 				= READ_BCD(rps.hour);
	uint8_t minute 			= READ_BCD(rps.minute);
	uint8_t second 			= READ_BCD(rps.second);
	uint8_t day_of_week = READ_BCD(rps.day_of_week);
	
	printf("-> %04d-%02d-%02d %02d:%02d:%02d week: %d\n",
		year, month, date, hour, minute, second, day_of_week);
	
//	printf("%02X-%02X-%02X %02X:%02X:%02X week: %X\n",
//		rps.year, rps.month, rps.date,
//		rps.hour, rps.minute, rps.second, rps.day_of_week);
}

#define RTC_EXTI_LINE		EXTI_17

void RTC_Alarm_IRQHandler(){
	
	if(SET == rtc_flag_get(RTC_FLAG_ALRM0)){
		// 重置标记
		exti_interrupt_flag_clear(RTC_EXTI_LINE);
		rtc_flag_clear(RTC_FLAG_ALRM0);
		
		printf("Alarm2!\n");
	}
	
//	if(SET == exti_interrupt_flag_get(RTC_EXTI_LINE)){
//		// 重置标记
//		exti_interrupt_flag_clear(RTC_EXTI_LINE);
//		rtc_flag_clear(RTC_FLAG_ALRM0);
//		
//		printf("Alarm1!\n");
//	}
	
}

void RTC_config_alarm(){
	// 重置标记
	exti_interrupt_flag_clear(RTC_EXTI_LINE);
	rtc_flag_clear(RTC_FLAG_ALRM0);
	
	rtc_alarm_struct rat;
	
	// mask设置忽略的选项 (让闹钟在每分钟的58秒都执行)
	rat.alarm_mask = RTC_ALARM_DATE_MASK | RTC_ALARM_HOUR_MASK | RTC_ALARM_MINUTE_MASK;
//	// mask设置忽略的选项 (让闹钟在每小时的59分钟的58秒都执行)
//	rat.alarm_mask = RTC_ALARM_DATE_MASK | RTC_ALARM_HOUR_MASK;
//	// mask设置忽略的选项 (让闹钟在每天的23小时的59分钟的58秒都执行)
//	rat.alarm_mask = RTC_ALARM_DATE_MASK;
	
	// 28日 RTC_ALARM_DATE_SELECTED | 周三 RTC_ALARM_WEEKDAY_SELECTED
	rat.weekday_or_date = RTC_ALARM_DATE_SELECTED;
	rat.alarm_day 			= 0x30; // 这个值是什么，取决于weekday_or_date
	rat.alarm_hour 			= 0x23; // 小时
	rat.alarm_minute		= 0x59; // 分钟
	rat.alarm_second		= 0x58; // 秒
	rat.am_pm= RTC_PM; 
	
	/* configure RTC alarm */
	rtc_alarm_config(RTC_ALARM0, &rat);
	
	// 配置外部中断
	exti_init(RTC_EXTI_LINE, EXTI_INTERRUPT, EXTI_TRIG_RISING);
	
	// NVIC-----------------------------
	// 配置中断优先级
	nvic_irq_enable(RTC_Alarm_IRQn, 2, 2);
	// 启用中断
	rtc_interrupt_enable(RTC_INT_ALARM0);
	
	/* enable RTC alarm */
	rtc_alarm_enable(RTC_ALARM0);
	
	// 清理标记
//	exti_interrupt_flag_clear(RTC_EXTI_LINE);

}

int main(void)
{
  nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
  systick_config();
  USART0_init();

  RTC_config();
	
//	printf("RTC_BKP0: 0x%X\n", RTC_BKP0);
//	
//	if(RTC_BKP0 == 0x1234){
//		printf("不再重复初始化时钟\n");
//		//说明不是第一次初始化，可以不用重新设置时间
//	}else {
//		// 说明首次进入，需要初始化RTC，将此值初始化
//		RTC_BKP0 = 0x1234;
//		printf("首次进入，初始化时钟\n");
//		// 首次开机，或者在同步时间后，才写入时间数据
//		RTC_write();
//	}
	
	RTC_write();
	
	RTC_config_alarm();
	
  while(1) {
		RTC_read();
    delay_1ms(1000);
  }
}