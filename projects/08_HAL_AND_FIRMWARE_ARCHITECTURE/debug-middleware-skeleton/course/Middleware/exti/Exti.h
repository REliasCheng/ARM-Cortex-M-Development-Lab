#ifndef __EXTI_H__
#define __EXTI_H__

#include "gd32f4xx.h"

// 0禁用 1硬触发 2软触发 3都可触发
#define USE_EXTI_0	0
#define USE_EXTI_1	1
#define USE_EXTI_2	0
#define USE_EXTI_3	0
#define USE_EXTI_4	0
#define USE_EXTI_5	0
#define USE_EXTI_6	0
#define USE_EXTI_7	0
#define USE_EXTI_8	0
#define USE_EXTI_9	0
#define USE_EXTI_10	0
#define USE_EXTI_11	0
#define USE_EXTI_12	0
#define USE_EXTI_13	0
#define USE_EXTI_14	0
#define USE_EXTI_15	0

/**************************** EXTI0 *****************************/
#if USE_EXTI_0
// GPIO 
#define EXTI0_RCU_PERIPH			RCU_GPIOD
#define EXTI0_GPIO_PORT				GPIOD
#define EXTI0_PULL_UP_DOWN			GPIO_PUPD_PULLUP
// EXTI & NVIC
#define EXTI0_SOURCE_PORT			EXTI_SOURCE_GPIOD
#define	EXTI0_TRIG_TYPE				EXTI_TRIG_BOTH
#define EXTI0_IRQ_PRIORITY			2, 2
#endif

/**************************** EXTI1 *****************************/
#if USE_EXTI_1
// GPIO & EXTI & NVIC
#define EXTI1_RCU_PERIPH			RCU_GPIOD
#define EXTI1_GPIO_PORT				GPIOD
#define EXTI1_PULL_UP_DOWN			GPIO_PUPD_PULLUP
#define EXTI1_SOURCE_PORT			EXTI_SOURCE_GPIOD
#define	EXTI1_TRIG_TYPE				EXTI_TRIG_BOTH
#define EXTI1_IRQ_PRIORITY			1, 2
#endif

/**************************** EXTI5 *****************************/
#if USE_EXTI_5
// GPIO & EXTI & NVIC
#define EXTI5_RCU_PERIPH			RCU_GPIOD
#define EXTI5_GPIO_PORT				GPIOD
#define EXTI5_PULL_UP_DOWN			GPIO_PUPD_PULLUP
#define EXTI5_SOURCE_PORT			EXTI_SOURCE_GPIOD
#define	EXTI5_TRIG_TYPE				EXTI_TRIG_BOTH
#define EXTI5_IRQ_PRIORITY			2, 2
#endif


void Exti_init();

/*!
    \brief    	外部中断触发函数
    \param[in]  linex: EXTI line number, refer to exti_line_enum
                only one parameter can be selected which is shown as below:
    \arg        EXTI_x (x=0..22): EXTI line x
*/			

#if USE_EXTI_0 || USE_EXTI_1 || USE_EXTI_2 || USE_EXTI_3 || \
		USE_EXTI_4 || USE_EXTI_5 || USE_EXTI_6 || USE_EXTI_7 ||	\
		USE_EXTI_8 || USE_EXTI_9 || USE_EXTI_10|| USE_EXTI_11|| \
		USE_EXTI_12|| USE_EXTI_13|| USE_EXTI_14|| USE_EXTI_15 
		
extern void Exti_on_trig(exti_line_enum linex);

#endif

#endif