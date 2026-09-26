#include "gd32f4xx.h"
#include "systick.h"
#include <stdio.h>
#include <string.h>
#include "main.h"

#include "USART0.h"

/*******************************************
任务目标： 使用独立看门狗自动重置芯片

1. 初始化独立看门狗，配置时钟和计数值
2. 启用看门狗
3. 在循环里在一定间隔时间内喂狗

*******************************************/
void USART0_on_recv(uint8_t* buffer, uint32_t len) {
  printf("recv[%d]-> %s\n", len, buffer);
	
	if(buffer[0] == 4){
		// 模拟卡死操作
		while(1);
	}

}

// Free WatchDog Timer
void FWDGT_init_config(){
	// 开启IRC32K晶振
	rcu_osci_on(RCU_IRC32K);
	// 等待晶振稳定运行
	while(SUCCESS != rcu_osci_stab_wait(RCU_IRC32K));
	
	//	1. 初始化独立看门狗，配置时钟和计数值
	/* configure counter reload value, and prescaler divider value
	uint16_t reload_value: 重载值(0x0000 - 0x0FFF)12位向下递减计数器 Max:4095
	uint8_t prescaler_div: 预分频系数，将32K分频（降频）
	
	32000Hz /  4 = 8000Hz Freq每秒数8000次，每次 0.125ms (1000ms/8000)
	32000Hz / 32 = 1000Hz Freq每秒数1000次，每次 1ms
	32000Hz / 64 =  500Hz Freq每秒数 500次，每次 2ms
	
	例如：目标的重启时间 target_ms = 1200ms 超过此时间不喂狗，触发重启
	结合分频系数计算计数器重装载值：
	reload_value = target_ms / 数一次数的时间
							 = target_ms / (1000ms / Freq)
							 = target_ms / (1000ms / (32000Hz / FWDGT_PSC))
							 = 1200ms / (1000ms / (32000 / 64))
							 = 1200ms / (1000ms / 500)
							 = 1200ms / 2ms
							 = 600
							 
  FWDGT_CTL 写入 0x5555：关闭FWDGT_PSC和FWDGT_RLD的写保护
	FWDGT_PSC 设置预分频系数
	FWDGT_RLD 设置重装载数值
	FWDGT_CTL = 0xAAAAU 将数值重新装载到看门狗定时器里
	***********************************************/
	
	fwdgt_config(1000, FWDGT_PSC_DIV32);			// 1000ms
//	fwdgt_config(500, FWDGT_PSC_DIV64);		  // 1000ms
	
	//	2. 启用看门狗
	// 向控制寄存器（FWDGT_CTL）中写0xCCCC可以开启独立看门狗定时器，计数器开始向下计数。
	fwdgt_enable();
}

int main(void)
{
	uint32_t feed = 0;
  nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
  systick_config();
  USART0_init();
	
	printf("Init Complete!\n");

	FWDGT_init_config();
	
  while(1) {
    delay_1ms(1160);
		/* 在循环喂狗 reload the counter of FWDGT 
		向控制寄存器（FWDGT_CTL）中写 0xAAAA：重装载计数器
		*/
		fwdgt_counter_reload();
		printf("喂狗! %d\n", ++feed);
  }
}