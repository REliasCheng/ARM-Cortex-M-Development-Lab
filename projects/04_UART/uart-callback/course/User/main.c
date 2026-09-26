#include "gd32f4xx.h"
#include "systick.h"
#include <stdio.h>
#include <string.h>
#include "main.h"

#include "USART0.h"

/*******************************************
任务目标：

通过USART0发送数据
TX: PA9
RX: P10

*******************************************/

uint8_t arr[] = {0x02, 0x03, 0x04};
char arr2[] = {'a', 'b', 'c'};
char* arr3 = "hello";

void USART0_on_recv(uint8_t* buffer, uint32_t len){
		printf("recv[%d]-> %s", len, buffer);
}

int main(void)
{
  systick_config();

  USART0_init();

  // 发送单个字节
//	USART0_send_data(0x23);
//	USART0_send_data(0x3E);
//	USART0_send_data(0xA5);

  // 发送字节数组
//	USART0_send_array(arr,3);
//	USART0_send_array(arr2,3);

  // 发送字符数组
//	USART0_send_string(arr2);
//	USART0_send_string(arr3);
//  USART0_send_string("Start!\n");

  uint8_t cnt = 0;
  while(1) {
//    printf("Hello %d\n", cnt++);
		
		USART0_send_data(cnt++);
    delay_1ms(500);
  }
}