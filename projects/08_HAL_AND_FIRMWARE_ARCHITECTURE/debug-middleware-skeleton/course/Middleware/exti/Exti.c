#include "Exti.h"


static void Exti_config(
			rcu_periph_enum rcu_periph,
			uint32_t gpio_port, uint32_t pull_up_down, uint32_t gpio_pin,
			uint8_t exti_port, uint8_t exti_pin,
			exti_line_enum linex, exti_trig_type_enum trig_type,
			uint8_t nvic_irq,
			uint8_t nvic_irq_pre_priority, uint8_t nvic_irq_sub_priority
		) {
    // GPIO init ------------------------------------
    // rcu
    rcu_periph_clock_enable(rcu_periph);
    // gpio 按钮输入
    gpio_mode_set(gpio_port, GPIO_MODE_INPUT, pull_up_down, gpio_pin);

    // Exti init ------------------------------------
    /* 配置时钟 */
    rcu_periph_clock_enable(RCU_SYSCFG);
    /* connect key EXTI line to key GPIO pin */
    syscfg_exti_line_config(exti_port, exti_pin);
    /* configure key EXTI line */
    exti_init(linex, EXTI_INTERRUPT, trig_type);

    // NVIC enbale-----------------------------------
    /* 启用并配置中断优先级 */
    nvic_irq_enable(nvic_irq, nvic_irq_pre_priority, nvic_irq_sub_priority);
    /* 启用外部中断 */
    exti_interrupt_enable(linex);
    /* 清理一次，可选 */
    exti_interrupt_flag_clear(linex);
}

void Exti_init(){
	

#if USE_EXTI_0
		Exti_config(
			EXTI0_RCU_PERIPH,
			EXTI0_GPIO_PORT, EXTI0_PULL_UP_DOWN, GPIO_PIN_0,
			EXTI0_SOURCE_PORT, EXTI_SOURCE_PIN0,
			EXTI_0, EXTI0_TRIG_TYPE,
			EXTI0_IRQn, EXTI0_IRQ_PRIORITY
		);
#endif
	
#if USE_EXTI_1
		Exti_config(
			EXTI1_RCU_PERIPH,
			EXTI1_GPIO_PORT, EXTI1_PULL_UP_DOWN, GPIO_PIN_1,
			EXTI1_SOURCE_PORT, EXTI_SOURCE_PIN1,
			EXTI_1, EXTI1_TRIG_TYPE,
			EXTI1_IRQn, EXTI1_IRQ_PRIORITY
		);
#endif
	
#if USE_EXTI_5
		Exti_config(
			EXTI5_RCU_PERIPH,
			EXTI5_GPIO_PORT, EXTI5_PULL_UP_DOWN, GPIO_PIN_5,
			EXTI5_SOURCE_PORT, EXTI_SOURCE_PIN5,
			EXTI_5, EXTI5_TRIG_TYPE,
			EXTI5_9_IRQn, EXTI5_IRQ_PRIORITY
		);
#endif
	
}
#define INTERRUPT_FLAG_TRIG(linex)								\
    if(exti_interrupt_flag_get(linex) == SET) {	\
        exti_interrupt_flag_clear(linex);				\
				Exti_on_trig(linex);											\
    }																							\

#if USE_EXTI_0
void EXTI0_IRQHandler(){
		INTERRUPT_FLAG_TRIG(EXTI_0)
}
#endif
#if USE_EXTI_1
void EXTI1_IRQHandler(){
		INTERRUPT_FLAG_TRIG(EXTI_1)
}
#endif

void EXTI5_9_IRQHandler() {

#if USE_EXTI_5
		INTERRUPT_FLAG_TRIG(EXTI_5)
#endif
	
#if USE_EXTI_6
		INTERRUPT_FLAG_TRIG(EXTI_6)
#endif
}