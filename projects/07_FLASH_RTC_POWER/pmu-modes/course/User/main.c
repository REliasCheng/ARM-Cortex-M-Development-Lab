#include "gd32f4xx.h"
#include "systick.h"
#include <stdio.h>
#include <string.h>
#include "main.h"

#include "USART0.h"
#include "EXTI.h"


/*******************************************
任务目标：

最小颗粒的单元功能测试

让LED1-PB2闪烁

通过接收串口消息进入不同的省电模式：

0x00. 睡眠模式			（任意中断可唤醒）
	

0x01. 深度睡眠模式（EXTI外部中断可唤醒）


0x02. 待机模式			（4个特殊唤醒方式）

*******************************************/

// 睡眠模式
void sleep_mode(){
	// RCU -------------------
	rcu_periph_clock_enable(RCU_PMU);
	
	printf("sleep_mode:1\n");
	
	// 进入睡眠模式
	pmu_to_sleepmode(WFI_CMD);
	// 唤醒中断的抢占优先级要比串口高才能唤醒
	// 因为此代码目前阻塞在串口的中断函数里
	
	printf("sleep_mode:2\n");
}

// 深度睡眠模式
void deep_sleep_mode(){
	
	rcu_periph_clock_enable(RCU_PMU);
	
	printf("deep_sleep_mode:1\n");
	// 进入深度睡眠模式
	pmu_to_deepsleepmode(PMU_LDO_LOWPOWER, PMU_LOWDRIVER_ENABLE, WFI_CMD);
	
	// 把主频设置回来
	SystemInit();
	
	printf("deep_sleep_mode:2\n");
}

// 待机模式
void standby_mode(){
	rcu_periph_clock_enable(RCU_PMU);
	
	/* 清理待机模式标记 */
	pmu_flag_clear(PMU_FLAG_RESET_STANDBY);
	
	/* 启用唤醒按钮 */
	pmu_wakeup_pin_enable();
	
	printf("standby mode:1\n");
	
	/* 进入待机模式 */
	pmu_to_standbymode();
	
	printf("standby mode:2\n");
	
}

void EXTI_1_on_trig(){
	printf("EXTI_1\n");
}

void USART0_on_recv(uint8_t* buffer, uint32_t len) {
  printf("recv[%d]-> %s\n", len, buffer);
	
	switch (buffer[0])
  {
  	case 0x00:
			sleep_mode();
  		break;
  	case 0x01:
			deep_sleep_mode();
  		break;
  	case 0x02:
			standby_mode();
  		break;
  	default:
  		break;
  }
}

static void GPIO_config(rcu_periph_enum rcu, uint32_t port, uint32_t pin) {
  // 1. 时钟初始化
  rcu_periph_clock_enable(rcu);
  // 2. 配置GPIO 输入输出模式
  gpio_mode_set(port, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, pin);
  // 3. 配置GPIO 输出选项
  gpio_output_options_set(port, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, pin);
  // 4. 默认输出电平
  gpio_bit_write(port, pin, RESET);
}

void delay(){
	uint32_t i = 20000000;
	while(i--) __NOP();
}

int main(void)
{
	nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
//  systick_config();
	EXTI_init();
  USART0_init();
	
	printf("init\n");
	
	GPIO_config(RCU_GPIOB, GPIOB, GPIO_PIN_2);
	
  while(1) {
		gpio_bit_toggle(GPIOB, GPIO_PIN_2);
//		delay_1ms(500);
		delay();
  }
}