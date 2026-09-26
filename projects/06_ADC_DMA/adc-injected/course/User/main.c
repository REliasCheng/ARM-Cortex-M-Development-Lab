#include "gd32f4xx.h"
#include "systick.h"
#include <stdio.h>
#include <string.h>
#include "main.h"

#include "USART0.h"

/*******************************************
任务目标：

通道多，要开启扫描模式

1. 通道个数比较少 <= 4，使用inserted插入通道（方便，效率高）
2. 通道个数比较多 > 4, 使用routine常规通道+DMA
3. 希望省电：关闭ADC连续获取模式，每次获取前，软触发采集ADC数据
4. 希望精准：开启ADC连续获取模式，
	a. 使用Timer定时触发，把多轮数据缓存，使用前进行均值滤波
	b. 初始化时使用1次软触发

*******************************************/
void USART0_on_recv(uint8_t* buffer, uint32_t len) {
  printf("recv[%d]-> %s\n", len, buffer);

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
  adc_special_function_config(ADC0, ADC_CONTINUOUS_MODE, DISABLE);
  /* 扫描模式配置 ADC scan mode disable */
  adc_special_function_config(ADC0, ADC_SCAN_MODE, ENABLE);
	
	// ------------------- ADC 采样通道规则配置 --------------------
  /* 配置要采样的通道个数 ADC channel length config 
		常规通道（规则组） 1-16 ADC_ROUTINE_CHANNEL
		插入通道（注入组） 1-4  ADC_INSERTED_CHANNEL
	*/
  adc_channel_length_config(ADC0, ADC_INSERTED_CHANNEL, 2);				// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  /* 配置通道每轮采样的循环次数 ADC routine channel config 
		在 12 位分辨率的情况下，
		总转换时间=采样时间+12 个 CK_ADC 周期
		0.375us = (3 + 12) * (1Mus/40MHz) = 15 * 0.025us
		1.286us =(15 + 12) * (1Mus/21MHz) = 27 * 0.0476us
	*/
  adc_inserted_channel_config(ADC0, 0, ADC_CHANNEL_16, ADC_SAMPLETIME_15); 	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  adc_inserted_channel_config(ADC0, 1, ADC_CHANNEL_14, ADC_SAMPLETIME_15);  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//  adc_inserted_channel_config(ADC0, 2, ADC_CHANNEL_18, ADC_SAMPLETIME_15);	

  /* ADC触发配置 ADC trigger config */
//  adc_external_trigger_source_config(ADC0, ADC_ROUTINE_CHANNEL, ADC_EXTTRIG_ROUTINE_T0_CH0);
//  adc_external_trigger_config(ADC0, ADC_ROUTINE_CHANNEL, EXTERNAL_TRIGGER_DISABLE);

  /* 启用电池电压检测 ADC Vbat channel enable */
//  adc_channel_16_to_18(ADC_VBAT_CHANNEL_SWITCH,ENABLE);
  /* 启用温度和参考电压通道 ADC temperature and Vref enable */
  adc_channel_16_to_18(ADC_TEMP_VREF_CHANNEL_SWITCH,ENABLE);

  /* ADC DMA function enable */
//  adc_dma_request_after_last_enable(ADC0);
//  adc_dma_mode_enable(ADC0);

  /* enable ADC interface */
  adc_enable(ADC0);
  /* wait for ADC stability */
  delay_1ms(1);
  /* ADC calibration and reset calibration */
  adc_calibration_enable(ADC0);
}

// 使用ADC采样
void ADC_get() {
	  /* 软件触发采样 enable ADC software trigger */
  adc_software_trigger_enable(ADC0, ADC_INSERTED_CHANNEL); 	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	
	/* 等待采样完毕，再读取数据 */
	while(RESET == adc_flag_get(ADC0, ADC_FLAG_EOIC)); 	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	
	/* 清理标记 */
	adc_flag_clear(ADC0, ADC_FLAG_EOIC);
	
	uint16_t adc_val = adc_inserted_data_read(ADC0, ADC_INSERTED_CHANNEL_0);  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	 
	// ADC数值 -> 电压值 -> 温度值
	
	// 1. ADC数值 -> 电压值
	// Vtemp / 3.3 = adc_val / 4095
	float Vtemp = adc_val * 3.3 / 4095;
	
	// 2. 电压值 -> 温度值
	/* 温度 (°C) = {(V25 – Vtemperature) / Avg_Slope} + 25
		V25 = 1.45 V
		Avg_Slope = 4.1 mV
	*/
	float T_rst = (1.45 - Vtemp) * 1000 / 4.1 + 25;
	
	printf("adc_val: %d Vtemp: %.2fV T_rst: %.2f℃\n", adc_val, Vtemp, T_rst);

	adc_val = adc_inserted_data_read(ADC0, ADC_INSERTED_CHANNEL_1);
	// ADC数值 -> 电压值 -> 温度值
	
	// 1. ADC数值 -> 电压值
	// Vtemp / 3.3 = adc_val / 4095
	float V_rst = adc_val * 3.3 / 4095;
	
	printf("V_rst: %.2fV\n", V_rst);
}


int main(void)
{
  nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
  systick_config();

  USART0_init();

  ADC_config();

  while(1) {

    // 循环获取温度值
    ADC_get();

    delay_1ms(1000);

  }
}