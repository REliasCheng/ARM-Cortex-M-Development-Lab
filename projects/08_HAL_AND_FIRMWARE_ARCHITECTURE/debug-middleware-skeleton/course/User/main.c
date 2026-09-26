#include "gd32f4xx.h"
#include "systick.h"
#include <stdio.h>
#include "main.h"
#include "Usart0.h"

int8_t arr[] = { 1,2,3,2,1 };
uint8_t global_value = 0xA3;

int32_t calc(int32_t a, int32_t b){
	return a / b;
}

void Usart0_on_recv(uint8_t* data, uint32_t len) {
  printf("buffer: %s len: %d \n", data, len);

  if (data[0] == 0x02) {
    gpio_bit_toggle(GPIOG, GPIO_PIN_3);
    printf("toggle\n");
  }else if (data[0] == 0x03){
		int8_t rst = arr[2];
		printf("rst = %d \n", rst);
	}
}

// PE3, PG3
static void GPIO_config() {
  // rcu
  rcu_periph_clock_enable(RCU_GPIOE);
  gpio_mode_set(GPIOE, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_3);
  gpio_output_options_set(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_3);
	
  rcu_periph_clock_enable(RCU_GPIOG);
  gpio_mode_set(GPIOG, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_3);
  gpio_output_options_set(GPIOG, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_3);
}


int main(void)
{
  nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);

	systick_config();

	Usart0_init();

	GPIO_config();

  printf("Init Complete!\n");

  uint32_t cnt = 0;
  while(1) {
		
    cnt++;
    printf("0x%02X %d -> ", cnt, cnt);
		
		printf("global_value: %d\n", global_value++);

		//		arr[0] = cnt;
		
    if (cnt % 3 == 0) {
      gpio_bit_set(GPIOE, GPIO_PIN_3);   // PE3 -> 1
      printf("set\n");
    } else {
      gpio_bit_reset(GPIOE, GPIO_PIN_3); // PE3 -> 0
      printf("reset\n");
    }

    delay_1ms(1000);
  }
}
