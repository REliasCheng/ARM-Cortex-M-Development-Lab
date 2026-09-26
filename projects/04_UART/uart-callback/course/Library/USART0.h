#ifndef __USART0_H__
#define __USART0_H__

#include "gd32f4xx.h"

// 功能开关配置 1打开，0关闭
#define USART0_RECV_CALLBACK    1

#define USART0_TX_RCU		RCU_GPIOB
#define USART0_TX_PORT	GPIOB
#define USART0_TX_PIN		GPIO_PIN_6

#define USART0_RX_RCU		RCU_GPIOB
#define USART0_RX_PORT	GPIOB
#define USART0_RX_PIN		GPIO_PIN_7

#define USART0_BAUDRATE 115200

void USART0_init();
// 发送一个字节数据
void USART0_send_data(uint8_t data);
// 发送多个字节
void USART0_send_array(uint8_t* data, uint32_t len);
// 发送字符数组 （\0是字符串结束标记）
void USART0_send_string(char* data);

extern void USART0_on_recv(uint8_t* buffer, uint32_t len);

#endif