#include "gd32f4xx.h"
#include "systick.h"
#include <stdio.h>
#include <string.h>
#include "main.h"

#include "USART0.h"
#include "TIMER.h"
#include "bsp_buzzer2.h"


/*******************************************
任务目标：让LED1(PD8), LED2 (PD9)呼吸

- 通过杜邦线连接
	- PD8和PE8
	- PD9和PE9
	
- 让TIMER0CH0的
	- OP极输出PWM PE9
	- ON极输出PWM PE8
	
- 进行呼吸灯输出操作
- 蜂鸣器播放声音(PB9, TM1_CH1)

*******************************************/


void USART0_on_recv(uint8_t* buffer, uint32_t len) {
  printf("recv[%d]-> %s", len, buffer);
}


#define	L1	1
#define	L2	2
#define	L3	3
#define	L4	4
#define	L5	5
#define	L6	6
#define	L7	7

#define N0 0

#define	N1	L1 + 7
#define	N2	L2 + 7
#define	N3	L3 + 7
#define	N4	L4 + 7
#define	N5	L5 + 7
#define	N6	L6 + 7
#define	N7	L7 + 7

#define	H1	N1 + 7
#define	H2	N2 + 7
#define	H3	N3 + 7
#define	H4	N4 + 7
#define	H5	N5 + 7
#define	H6	N6 + 7
#define	H7	N7 + 7


u8 notes[] = {
  L5,N1,N1,N3, N6,N3,N5, N5,N6,N5,N3, N4,N3,N2,
  L6,N2,N2,N4, N7,N7,N6,N5, N4,N0,N4,N3, L6,L7,N1,
  N2, N2,N0, L5,N1,N1,N3, N6,N3,N5,
  N5,N6,N5,N3, N4,N3,N2, L6,N2,N2,N4, N7,N7,N6,N5,
  N4,N4,N3, L7,N2, N1, N1,N0,L5,

};

u8 durations[] = {
  3,1,3,1, 3,1,3, 3,1,3,1, 3,1,3,
  3,1,3,1, 3,1,3,1, 2,2,3,1, 2,4,2,
  8, 4,4, 3,1,3,1, 3,1,3,
  3,1,3,1, 3,1,3, 3,1,3,1, 3,1,3,1,
  4,3,1, 4,4, 8, 4,3,1,
};
int main(void)
{
  systick_config();

  USART0_init();

  TIMER_init();

  uint32_t len = sizeof(notes) / sizeof(notes[0]);

  while(1) {

    for(uint32_t i = 0; i < len; i++) {
      // 按照指定音调输出
      Buzzer_beep(notes[i]);

      // 根据每个音调进行休眠
      delay_1ms(durations[i] * 100);

      // 短暂的间隔
      Buzzer_stop();
      delay_1ms(20);
    }

    // 停止
    Buzzer_stop();
  }
}