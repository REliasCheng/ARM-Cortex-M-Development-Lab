#include "gd32f4xx.h"
#include "systick.h"
#include <stdio.h>
#include <string.h>
#include "main.h"

#include "USART0.h"
//#include "I2C_soft.h"
#include "I2C0.h"

#include "PCF8563.h"


/*******************************************
任务目标：

自己根据I2C协议定义，实现I2C设备的数据读写

PCF8563: 设备地址0x51, 写0xA2 读0xA3
I2C屏幕: 设备地址0x3C, 写0x78

*******************************************/
void USART0_on_recv(uint8_t* buffer, uint32_t len) {
  printf("recv[%d]-> %s\n", len, buffer);

}

int main(void)
{
  nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
  systick_config();

  USART0_init();

//  I2C_soft_init();
	I2C0_init();

//	uint8_t addr = 0x51;
//	uint8_t reg  = 0x02;
//	uint8_t data[] = {0x03, 0x04, 0xAF};
//	int8_t rst = I2C_soft_write(addr, reg, data, 3);
//	printf("rst: %d\n", rst);
  // 提前准备时间，写1次时间 -------------------------------------
  // 准备 秒、分、时、天、周、世纪&月、年
  Clock_t c = {
    .week = 1,
    .year = 2024,
    .month = 9,
    .day = 30,
    .hour = 23,
    .minute = 59,
    .second = 55,
  };

  PCF8563_set_clock(c);
  while(1) {
    // 循环读取数据: 秒、分、时、天、周、月、年、世纪
    PCF8563_get_clock(&c);

    // 打印数据
    printf("%02d-%02d-%02d ", (int)c.year, (int)c.month, (int)c.day);
    printf("%02d:%02d:%02d ", (int)c.hour, (int)c.minute, (int)c.second);
    printf("week->%d\n", (int)c.week);

    delay_1ms(900);
  }
}