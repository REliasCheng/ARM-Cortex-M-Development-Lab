#include "gd32f4xx.h"
#include "systick.h"
#include <stdio.h>
#include <string.h>
#include "main.h"

#include "USART0.h"

/*******************************************
任务目标：使用ADC0结合DMA读取两个通道的数据

1. 电位器电压PC4->IN14通道
2. 芯片温度IN16通道

*******************************************/
void USART0_on_recv(uint8_t* buffer, uint32_t len) {
  printf("recv[%d]-> %s\n", len, buffer);

}

#define ADC_RECV_LEN		2

uint16_t g_recv_buff[ADC_RECV_LEN];

#define DMA_CH	DMA1, DMA_CH0
// ------------------ DMA初始化 ---------------------------------
void DMA_config(){
	// RCU ----------------
	rcu_periph_clock_enable(RCU_DMA1);
	
	// DMA ----------------
	/* deinitialize DMA a channel registers */
	dma_deinit(DMA_CH);
	dma_single_data_parameter_struct init_struct;
	
	/* 初始化默认参数 initialize the DMA single data mode parameters struct with the default values */
  dma_single_data_para_struct_init(&init_struct);
	
	/* set the DMA struct with the default values */
	// 方向：外设到内存
	init_struct.direction           = DMA_PERIPH_TO_MEMORY;
	// 外设：
	init_struct.periph_addr         = (uint32_t)(&ADC_RDATA(ADC0));
	init_struct.periph_inc          = DMA_PERIPH_INCREASE_DISABLE;
	// 内存
	init_struct.memory0_addr        = (uint32_t)g_recv_buff;
	init_struct.memory_inc          = DMA_MEMORY_INCREASE_ENABLE;
	// 数据的个数
	init_struct.number              = ADC_RECV_LEN;
	// 数据的宽度
	init_struct.periph_memory_width = DMA_PERIPH_WIDTH_16BIT;
	// 开启循环模式
	init_struct.circular_mode       = DMA_CIRCULAR_MODE_ENABLE;
	// 配置优先级
	init_struct.priority            = DMA_PRIORITY_ULTRA_HIGH;

	/* DMA single data mode initialize */
  dma_single_data_mode_init(DMA_CH, &init_struct);
	// 配置DMA子集
	dma_channel_subperipheral_select(DMA_CH, DMA_SUBPERI0);
	// 开启DMA循环模式
	dma_circulation_enable(DMA_CH);
	
	// 清理传输完成标记
	dma_flag_clear(DMA_CH, DMA_FLAG_FTF);
	
	// 开启接收
	dma_channel_enable(DMA_CH);

}

// 配置ADC
void ADC_config() {
	
	// 初始化PC4引脚模式
	rcu_periph_clock_enable(RCU_GPIOC);
	gpio_mode_set(GPIOC, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_4);
	
	/* RCU时钟初始化 */
	/* 启用ADC0 enable ADC clock */
	rcu_periph_clock_enable(RCU_ADC0);
	/* 配置分频系数 config ADC clock 84M / 4 = 21MHz */
	adc_clock_config(ADC_ADCCK_PCLK2_DIV4);

  /* 多个ADC运行时，避免相互干扰，运行模式配置 ADC mode config */
  adc_sync_mode_config(ADC_SYNC_MODE_INDEPENDENT);
	
	// ------------------ ADC 基本配置 ----------------------------
	/* 配置分辨率 */
	adc_resolution_config(ADC0, ADC_RESOLUTION_12B);
  /* 数据对齐方式 ADC data alignment config */
  adc_data_alignment_config(ADC0, ADC_DATAALIGN_RIGHT);
	
  /* 连续模式配置 ADC contineous function disable */
  adc_special_function_config(ADC0, ADC_CONTINUOUS_MODE, ENABLE);
  /* 扫描模式配置 ADC scan mode disable */
  adc_special_function_config(ADC0, ADC_SCAN_MODE, ENABLE);
	
	// ------------------- ADC 采样通道规则配置 --------------------
  /* 配置要采样的通道个数 ADC channel length config 
		常规通道（规则组） 1-16 ADC_ROUTINE_CHANNEL
		插入通道（注入组） 1-4  ADC_INSERTED_CHANNEL
	*/
  adc_channel_length_config(ADC0, ADC_ROUTINE_CHANNEL, 2);
  /* 配置通道每轮采样的循环次数 ADC routine channel config 
		在 12 位分辨率的情况下，
		总转换时间=采样时间+12 个 CK_ADC 周期
		0.375us = (3 + 12) * (1Mus/40MHz) = 15 * 0.025us
		1.286us =(15 + 12) * (1Mus/21MHz) = 27 * 0.0476us
	*/
  adc_routine_channel_config(ADC0, 0, ADC_CHANNEL_16, ADC_SAMPLETIME_15);
  adc_routine_channel_config(ADC0, 1, ADC_CHANNEL_14, ADC_SAMPLETIME_15);
//  adc_routine_channel_config(ADC0, 2, ADC_CHANNEL_18, ADC_SAMPLETIME_15);

  /* ADC触发配置 ADC trigger config */
//  adc_external_trigger_source_config(ADC0, ADC_ROUTINE_CHANNEL, ADC_EXTTRIG_ROUTINE_T0_CH0);
//  adc_external_trigger_config(ADC0, ADC_ROUTINE_CHANNEL, EXTERNAL_TRIGGER_DISABLE);

  /* 启用电池电压检测 ADC Vbat channel enable */
//  adc_channel_16_to_18(ADC_VBAT_CHANNEL_SWITCH,ENABLE);
  /* 启用温度和参考电压通道 ADC temperature and Vref enable */
  adc_channel_16_to_18(ADC_TEMP_VREF_CHANNEL_SWITCH,ENABLE);

  /* ADC DMA function enable */
  adc_dma_request_after_last_enable(ADC0);
  adc_dma_mode_enable(ADC0);

  /* enable ADC interface */
  adc_enable(ADC0);
  /* wait for ADC stability */
  delay_1ms(1);
  /* ADC calibration and reset calibration */
  adc_calibration_enable(ADC0);
	
  /* 软件触发采样 enable ADC software trigger */
  adc_software_trigger_enable(ADC0, ADC_ROUTINE_CHANNEL);
}

// 使用ADC采样        IN16通道芯片温度
// 使用ADC采样 PC4 -> IN14通道电位器电压
void ADC_get() {
	
	// ADC数值 -> 电压值 -> 温度值
	
	// 1. ADC数值 -> 电压值
	// Vtemp / 3.3 = adc_val / 4095
	float Vtemp = g_recv_buff[0] * 3.3 / 4095;
	
	// 2. 电压值 -> 温度值
	/* 温度 (°C) = {(V25 – Vtemperature) / Avg_Slope} + 25
		V25 = 1.45 V
		Avg_Slope = 4.1 mV
	*/
	float T_rst = (1.45 - Vtemp) * 1000 / 4.1 + 25;
	
	printf("adc_val: %d Vtemp: %.2fV T_rst: %.2f℃\n", g_recv_buff[0], Vtemp, T_rst);
	
		// 1. ADC数值 -> 电压值
	// Vtemp / 3.3 = adc_val / 4095
	float V_rst = g_recv_buff[1] * 3.3 / 4095;
	
	
	printf("V_rst: %.2fV\n", V_rst);
}


int main(void)
{
  nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
  systick_config();

  USART0_init();

	DMA_config();
  ADC_config();

  while(1) {

    // 循环获取温度值 和 电位器电压
    ADC_get();

    delay_1ms(1000);

  }
}