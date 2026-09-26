#include "gd32f4xx.h"
#include "systick.h"
#include <stdio.h>
#include <string.h>
#include "main.h"

#include "USART0.h"


/*******************************************
任务目标：通过DMA1进行数据拷贝
Memory to Peripheral 内存到外设数据拷贝

DMA实现USART数据发送
a. 配置DMA
b. USART启用DMA
c. 重写发送函数

DMA实现USART数据接收
a. 配置DMA
b. USART启用DMA
c. 重写接收函数（中断）

*******************************************/
void USART0_on_recv(uint8_t* buffer, uint32_t len) {
  printf("recv[%d]-> %s\n", len, buffer);
	
}

uint8_t arr[] = {0x61, 0x62, 0x63};

int main(void)
{
  nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
  systick_config();
	
  USART0_init();
	
	USART0_send_data('#');
	USART0_send_string("Hello:\n");
	USART0_send_array(arr, 3);
	
	printf("\n");
	
	USART0_dma_send_data('$');
	USART0_dma_send_string("Hello:\n");
	USART0_dma_send_array(arr, 3);
	
  while(1) {
		delay_1ms(500);
		
  }
}