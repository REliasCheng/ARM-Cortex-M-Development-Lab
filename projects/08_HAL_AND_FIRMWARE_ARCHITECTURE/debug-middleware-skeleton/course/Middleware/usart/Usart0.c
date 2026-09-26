#include "Usart0.h"
#include <stdio.h>
#include <string.h>

#define DMA_PERIPH_ADDR			((uint32_t)&USART_DATA(USART0))

static uint8_t 	g_rx_buffer[RX_BUFFER_LEN];
static uint32_t g_rx_cnt = 0;

#if USART0_DMA_TX_ENABLE

static void USART0_TX_DMA_config() {

  // 内存到外设： DMA1 CH7 USART0_TX 子集100

  // 传输模式：SRC内存->DST串口
  // SRC内存: memory0_addr
  // DST串口: periph_addr


  // 初始化RCU ------------------------------------------------
  rcu_periph_clock_enable(USART0_TX_DMA_RCU);

  // 初始化DMA ------------------------------------------------
  dma_deinit(USART0_TX_DMA_PERIPH_CH);
  dma_single_data_parameter_struct init_struct;
  /* initialize the DMA single data mode parameters struct with the default values */
  dma_single_data_para_struct_init(&init_struct);
  // 方向
  init_struct.direction           = DMA_MEMORY_TO_PERIPH;

  // SRC内存源头
  // init_struct.memory0_addr        = (uint32_t)src;
  init_struct.memory_inc          = DMA_MEMORY_INCREASE_ENABLE;	// DISABLE
  // init_struct.number              = sizeof(src) / sizeof(src[0]);
  // 每个数据的大小（BIT数）
  init_struct.periph_memory_width = DMA_MEMORY_WIDTH_8BIT;

  // DST串口目标, 必须取地址
  // uint32_t a = USART_DATA(USART0); // 不能这样赋值，这是将寄存器里的值取出来
  init_struct.periph_addr         = DMA_PERIPH_ADDR;
  init_struct.periph_inc          = DMA_PERIPH_INCREASE_DISABLE;	// DISABLE

  // 循环模式，禁用(默认)
  init_struct.circular_mode       = DMA_CIRCULAR_MODE_DISABLE;
  // DMA优先级
  init_struct.priority            = DMA_PRIORITY_ULTRA_HIGH;

  /* DMA single data mode initialize */
  dma_single_data_mode_init(USART0_TX_DMA_PERIPH_CH, &init_struct);

  /* DMA通道子集选择 */
  dma_channel_subperipheral_select(USART0_TX_DMA_PERIPH_CH, USART0_TX_DMA_CHANNEL_SUB);

  // 开启中断(可选) 传输完成中断
//	nvic_irq_enable(DMA1_Channel7_IRQn, 0, 0);
//	dma_interrupt_enable(DMA_PERIPH_CH, DMA_CHXCTL_FTFIE);

}

#endif

#if USART0_DMA_RX_ENABLE


static void USART0_RX_DMA_config() {

  // 外设到内存： DMA1 CH2 USART0_RX 子集100

  // 传输模式：SRC串口->DST内存
  // SRC串口: periph_addr
  // DST内存: memory0_addr

  // 初始化RCU ------------------------------------------------
  rcu_periph_clock_enable(USART0_RX_DMA_RCU);

  // 初始化DMA ------------------------------------------------
  dma_deinit(USART0_RX_DMA_PERIPH_CH);
  dma_single_data_parameter_struct init_struct;
  /* initialize the DMA single data mode parameters struct with the default values */
  dma_single_data_para_struct_init(&init_struct);
  // 方向
  init_struct.direction           = DMA_PERIPH_TO_MEMORY;

  // SRC源头
  // uint32_t a = USART_DATA(USART0); // 不能这样赋值，这是将寄存器里的值取出来
  init_struct.periph_addr         = DMA_PERIPH_ADDR;
  init_struct.periph_inc          = DMA_PERIPH_INCREASE_DISABLE;	// DISABLE
  // 每个数据的大小（BIT数）
  init_struct.periph_memory_width = DMA_MEMORY_WIDTH_8BIT;
  init_struct.number              = RX_BUFFER_LEN;

  // DST目标, 必须取地址
  init_struct.memory0_addr        = (uint32_t)g_rx_buffer;
  init_struct.memory_inc          = DMA_MEMORY_INCREASE_ENABLE;	// ENABLE

  // 循环模式，禁用(默认)
  init_struct.circular_mode       = DMA_CIRCULAR_MODE_DISABLE;
  // DMA优先级
  init_struct.priority            = DMA_PRIORITY_ULTRA_HIGH;

  /* DMA single data mode initialize */
  dma_single_data_mode_init(USART0_RX_DMA_PERIPH_CH, &init_struct);

  /* DMA通道子集选择 */
  dma_channel_subperipheral_select(USART0_RX_DMA_PERIPH_CH, USART0_RX_DMA_CHANNEL_SUB);

  // 通知DMA去搬运数据
  dma_channel_enable(USART0_RX_DMA_PERIPH_CH);
}

#endif


void Usart0_init() {
#if USART0_DMA_TX_ENABLE
  // DMA初始化 TX
  USART0_TX_DMA_config();
#endif

#if USART0_DMA_RX_ENABLE
  // DMA初始化 RX
  USART0_RX_DMA_config();
#endif

  // GPIO初始化
  // TX:PA9 	RX:PA10 AF ------------------------------------------
  // 启用GPIO时钟 TX
  rcu_periph_clock_enable(USART0_TX_RCU);
  // 启用GPIO时钟 RX
  rcu_periph_clock_enable(USART0_RX_RCU);

  /* configure the USART0 TX pin and USART0 RX pin */
  gpio_af_set(USART0_TX_PORT, USART0_TX_AF, USART0_TX_PIN);
  gpio_af_set(USART0_RX_PORT, USART0_RX_AF, USART0_RX_PIN);

  // gpio配置
  gpio_mode_set(USART0_TX_PORT,GPIO_MODE_AF, GPIO_PUPD_NONE, USART0_TX_PIN);
//  gpio_output_options_set(USART0_TX_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, USART0_TX_PIN);

  // gpio配置
  gpio_mode_set(USART0_RX_PORT,GPIO_MODE_AF, GPIO_PUPD_NONE, USART0_RX_PIN);
//  gpio_output_options_set(USART0_RX_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, USART0_RX_PIN);

  // 串口初始化 ---------------------------------------------------
  // 启用时钟
  rcu_periph_clock_enable(RCU_USART0);
  // 重置
  usart_deinit(USART0);
  // 配置串口参数：波特率*，数据位，校验位，停止位
  usart_baudrate_set(USART0, USART0_BAUDRATE);								 // 波特率：	必填
  usart_word_length_set(USART0, USART_WL_8BIT);		 // 数据位，	默认8bit
  usart_parity_config(USART0, USART_PM_NONE);			 // 校验位：	默认无校验位
  usart_stop_bit_set(USART0, USART_STB_1BIT);			 // 停止位：	默认1bit
  usart_data_first_config(USART0, USART_MSBF_LSB); // 数据模式：默认小端模式

#if USART0_DMA_TX_ENABLE
  // DMA发送启用
  usart_dma_transmit_config(USART0, USART_TRANSMIT_DMA_ENABLE);
#endif

#if USART0_DMA_RX_ENABLE
  // DMA接收启用
  usart_dma_receive_config(USART0, USART_RECEIVE_DMA_ENABLE);
#endif

  // -----------------无论DMA开不开，这里都要启用发送和接收
  // 发送启用
  usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);
  // 接收启用
  usart_receive_config(USART0, USART_RECEIVE_ENABLE);

  // 接收中断配置
  nvic_irq_enable(USART0_IRQn, 2, 0);

#if !USART0_DMA_RX_ENABLE
  usart_interrupt_enable(USART0, USART_INT_RBNE);
#endif

  usart_interrupt_enable(USART0, USART_INT_IDLE);
  // 启用usart
  usart_enable(USART0);

}

void Usart0_send_data(uint8_t* data, uint32_t len) {

#if USART0_DMA_TX_ENABLE

  // 动态发送数据（内存到外设）
  dma_memory_address_config(USART0_TX_DMA_PERIPH_CH, DMA_MEMORY_0, (uint32_t)data);
  dma_transfer_number_config(USART0_TX_DMA_PERIPH_CH, len);

  // 通知DMA去搬运数据（不阻塞）
  dma_channel_enable(USART0_TX_DMA_PERIPH_CH);
  // 等待数据完成 full transfer finish
  while(dma_flag_get(USART0_TX_DMA_PERIPH_CH, DMA_FLAG_FTF) == RESET);
  // 清理标记，避免下次无法发送
  dma_flag_clear(USART0_TX_DMA_PERIPH_CH, DMA_FLAG_FTF);

#else

  while(data && len) {
    Usart0_send_byte((uint8_t)(*data));
    data++;
    len--;
  }

#endif

}

//发送一byte数据
void Usart0_send_byte(uint8_t data) {

#if USART0_DMA_TX_ENABLE

  Usart0_send_data(&data, 1);

#else

  //通过USART发送
  usart_data_transmit(USART0, data);
  //判断缓冲区是否已经空了 TBE = Transmit data Buffer Empty
  //FlagStatus state = usart_flag_get(USART_NUM,USART_FLAG_TBE);
  while(RESET == usart_flag_get(USART0, USART_FLAG_TBE));

#endif

}

// 配置printf数据打印
int fputc(int ch, FILE *f) {
  Usart0_send_byte((uint8_t)ch);
  return ch;
}

//发送字符串
void Usart0_send_string(uint8_t *data) {

#if USART0_DMA_TX_ENABLE

  Usart0_send_data(data, strlen((const char *)data));

#else
  //满足: 1.data指针不为空  2.发送的数据不是\0结束标记
  while(data && *data) {
    Usart0_send_byte((uint8_t)(*data));
    data++;
  }
#endif

}

/*
1. USART0的中断函数只有一个
2. 触发中断的原因(标记)有多个
3. 需要区分是那个标记触发的中断
*/
void USART0_IRQHandler() {

//	printf("USART0_IRQHandler\n");

#if USART0_DMA_RX_ENABLE

  if(usart_interrupt_flag_get(USART0, USART_INT_FLAG_IDLE) == SET) {
    // printf(">IDLE<\n");
    // 闲置了 clear IDLE flag
    usart_data_receive(USART0); 	 // 必须要读，读出来的值不能要

    // 通过DMA获取接收数据的长度
    /* get the number of remaining data to be transferred by the DMA */
    g_rx_cnt = RX_BUFFER_LEN - dma_transfer_number_get(USART0_RX_DMA_PERIPH_CH);

    // 添加字符串结束标记, 避免打印时出错
    g_rx_buffer[g_rx_cnt] = '\0';

    Usart0_on_recv(g_rx_buffer, g_rx_cnt);

    // 清理缓冲区
    g_rx_cnt = 0;

    // 停止DMA搬运
    dma_channel_disable(USART0_RX_DMA_PERIPH_CH);
    // 清除标记
    dma_flag_clear(USART0_RX_DMA_PERIPH_CH, DMA_FLAG_FTF);
    // 重置计数器
    dma_transfer_number_config(USART0_RX_DMA_PERIPH_CH, RX_BUFFER_LEN);
    // 启用DMA搬运
    dma_channel_enable(USART0_RX_DMA_PERIPH_CH);
  }

#else

  if(usart_interrupt_flag_get(USART0, USART_INT_FLAG_RBNE) == SET) {
    // 接收到数据了
    // printf(">RBNE<\n");
    usart_interrupt_flag_clear(USART0, USART_INT_FLAG_RBNE);

    // 获取寄存器里的数据, 缓存到buffer
    g_rx_buffer[g_rx_cnt++] = usart_data_receive(USART0);

    // 原样返回
//		send_data(data);
  }

  if(usart_interrupt_flag_get(USART0, USART_INT_FLAG_IDLE) == SET) {
    // printf(">IDLE<\n");
    // 闲置了 clear IDLE flag
    usart_data_receive(USART0); 	 // 必须要读，读出来的值不能要
    // 处理缓冲区的内容

    // 添加字符串结束标记, 避免打印时出错
    g_rx_buffer[g_rx_cnt] = '\0';

#if USART0_RECV_CALLBACK
    Usart0_on_recv(g_rx_buffer, g_rx_cnt);
#endif

    // 清理缓冲区
    g_rx_cnt = 0;
  }

#endif

}