#include "gd32f4xx.h"
#include "systick.h"
#include <stdio.h>
#include <string.h>
#include "main.h"

#include "USART0.h"
#include "EXTI.h"


/*******************************************
任务目标：

按钮：PA0
● 按键按下时，打印日志
● 按键抬起时，打印日志
● 不通过键盘扫描实现，通过外部中断实现
*******************************************/

void USART0_on_recv(uint8_t* buffer, uint32_t len) {
  printf("recv[%d]-> %s", len, buffer);
	
	exti_software_interrupt_enable(EXTI_5);
}

void EXTI_0_on_trig() {
  printf("EXTI_0!\n");
}

uint32_t start_tick = 0;
void EXTI_1_on_trig() {

//		printf("IRQ\n");
  // 读取IO最新电平
  FlagStatus cur_state = gpio_input_bit_get(GPIOC, GPIO_PIN_1);

  if(cur_state == RESET) { // 按下
    printf("Down!\n");
    start_tick = systick_tick_us();
  } else {
    // (100 + 4,294,967,295)  - 4 294 000 000

    // 按下到抬起的时间间隔ms
    float duration = (systick_tick_us() - start_tick) * 1.0f / 1000;

    if(duration > 1000) {
      printf("Long Up: %.2f ms\n", duration);
    } else {
      printf("Up: %.2f ms\n", duration);

    }

  }
}

void EXTI_5_on_trig(){
  printf("EXTI_5!\n");
}

int main(void)
{
  systick_config();

  USART0_init();
  EXTI_init();

  uint8_t cnt = 0;
  while(1) {
//    printf("Hello %d\n", cnt++);

    delay_1ms(500);
  }
}