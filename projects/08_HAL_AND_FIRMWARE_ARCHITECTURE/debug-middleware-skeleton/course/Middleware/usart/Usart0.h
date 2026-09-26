#ifndef __USART0_H__
#define __USART0_H__

#include "gd32f4xx.h"

// ============================================================================================ ↓ 以下可以自行配置

// ---------------------------------------------------------------------------GPIO
/**
串口通信
TX	PA9, RX	PA10
*/
#define USART0_BAUDRATE		115200

#define USART0_TX_RCU			RCU_GPIOA
#define USART0_TX_PORT		GPIOA
#define USART0_TX_PIN			GPIO_PIN_9
#define USART0_TX_AF			GPIO_AF_7

#define USART0_RX_RCU			RCU_GPIOA
#define USART0_RX_PORT		GPIOA
#define USART0_RX_PIN			GPIO_PIN_10
#define USART0_RX_AF			GPIO_AF_7

// ---------------------------------------------------------------------------USART

#define USART0_RECV_CALLBACK		1

#define RX_BUFFER_LEN 		1024

// ----------------------------------------------------------------------------DMA
// 1 enable	0 disable
#define USART0_DMA_TX_ENABLE 1
#define USART0_DMA_RX_ENABLE 1

#if USART0_DMA_TX_ENABLE
#define USART0_TX_DMA_RCU					RCU_DMA1
#define USART0_TX_DMA_PERIPH_CH		DMA1, DMA_CH7
#define USART0_TX_DMA_CHANNEL_SUB	DMA_SUBPERI4
#endif

#if USART0_DMA_RX_ENABLE
#define USART0_RX_DMA_RCU					RCU_DMA1
#define USART0_RX_DMA_PERIPH_CH		DMA1, DMA_CH2
#define USART0_RX_DMA_CHANNEL_SUB	DMA_SUBPERI4
#endif

// ============================================================================================ ↑ 以上可以自行配置

void Usart0_init();
void Usart0_send_byte(uint8_t data);
void Usart0_send_data(uint8_t* data, uint32_t len);
void Usart0_send_string(uint8_t *data);

#if USART0_RECV_CALLBACK
extern void Usart0_on_recv(uint8_t* data, uint32_t len);
#endif

#endif