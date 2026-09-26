#include "bsp_battery_led.h"
#include "systick.h"

// 声明LED灯结构体
typedef struct {
  rcu_periph_enum rcu;
  uint32_t port;
  uint32_t pin;
} Led_GPIO_t;

// 声明单个结构体变量
//Led_GPIO_t g_sw_gpio = {
//	RCU_GPIOC, GPIOC, GPIO_PIN_6
//};

// 声明所有LED灯数组 global
Led_GPIO_t g_gpio_list[] = {
  {RCU_GPIOC, GPIOC, GPIO_PIN_6 },// LED_SW
  {RCU_GPIOD, GPIOD, GPIO_PIN_8 },// LED1
  {RCU_GPIOD, GPIOD, GPIO_PIN_10},// LED3
  {RCU_GPIOD, GPIOD, GPIO_PIN_12},// LED5
  {RCU_GPIOD, GPIOD, GPIO_PIN_14},// LED7
};

uint8_t g_list_cnt = sizeof(g_gpio_list) / sizeof(Led_GPIO_t);

static void GPIO_config(rcu_periph_enum rcu, uint32_t port, uint32_t pin ) {
  // 初始化LED, 推挽输出
  // 时钟初始化
  rcu_periph_clock_enable(rcu);
  // 配置GPIO模式：输出模式
  gpio_mode_set(port, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, pin);
  // 配置输出选项：PP推挽，最大速度
  gpio_output_options_set(port, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, pin);
}

void Battery_led_init() {

  for(uint8_t i = 0; i < g_list_cnt; i++) {
    Led_GPIO_t gpio = g_gpio_list[i];
    // 初始化GPIO
    GPIO_config(gpio.rcu, gpio.port, gpio.pin);
    // 默认关闭(拉高)
    gpio_bit_write(gpio.port, gpio.pin, SET);
  }
  // 拉低总开关
  gpio_bit_write(g_gpio_list[0].port, g_gpio_list[0].pin, RESET);
}

void Battery_led_turn_on(uint8_t pos) { // 拉低开灯
  Led_GPIO_t gpio = g_gpio_list[pos];
  gpio_bit_write(gpio.port, gpio.pin, RESET);
}

void Battery_led_turn_off(uint8_t pos) { // 拉高关灯
  Led_GPIO_t gpio = g_gpio_list[pos];
  gpio_bit_write(gpio.port, gpio.pin, SET);
}


void Battery_led_turn(uint8_t pos, uint8_t on) { // 拉高关灯
  Led_GPIO_t gpio = g_gpio_list[pos];
  gpio_bit_write(gpio.port, gpio.pin, on ? RESET : SET);
}

// 使用状态机记录当前模块运行状态

// 0. 默认：LED都熄灭
// 1. 充电中： 需要呈现出流水灯闪烁
// 2. 充电结束中：根据电量闪烁3次
uint8_t state = 0;	// status

uint8_t show_power = 0;
uint8_t curr_power = 0;

// 开始充电流水灯
// power当前电量 [0,1,2,3,4]
void Battery_led_start(uint8_t power){
	// 根据当前点亮，依次增加到4
	show_power = power;
	curr_power = power;
	
	// 切换成充电中状态
	state = 1;
}

void Battery_led_update(uint8_t power){
	curr_power = power;
}

uint8_t charging_stop_cnt = 0;
uint8_t charging_cnt = 0;

// 在主循环调用（根据需要的间隔时间）状态机
// 100ms
void Battery_led_loop(){
	if (state == 0){
		// 默认，停止
		Battery_led_turn_off(LED1);
		Battery_led_turn_off(LED2);
		Battery_led_turn_off(LED3);
		Battery_led_turn_off(LED4);
		
	}else if(state == 1){ // 500ms
		if(++charging_cnt < 5){
			return;
		}
		charging_cnt = 0;
		
		// 充电中
		// 0,1,2,3,4
		Battery_led_turn(LED1, show_power >= 1);
		Battery_led_turn(LED2, show_power >= 2);
		Battery_led_turn(LED3, show_power >= 3);
		Battery_led_turn(LED4, show_power >= 4);
		
		// 让show_power重置为power
		if(++show_power > 4) show_power = curr_power;
		
	}else if(state == 2){ // 200ms
		
		
		// 根据当前当前电量值，闪烁3次 (600ms)
		if(charging_stop_cnt % 2 == 0){ // 0, 2, 4
			Battery_led_turn_off(LED1);
			Battery_led_turn_off(LED2);
			Battery_led_turn_off(LED3);
			Battery_led_turn_off(LED4);
		}else {	// 1, 3, 5
			Battery_led_turn(LED1, curr_power >= 1);
			Battery_led_turn(LED2, curr_power >= 2);
			Battery_led_turn(LED3, curr_power >= 3);
			Battery_led_turn(LED4, curr_power >= 4);
		}
		
		if(++charging_stop_cnt == 6){
			charging_stop_cnt = 0;
			
			// 闪完之后自动切换回默认状态
			state = 0;
		}
		
	}
}

void Battery_led_stop(){
	
	// 切换成停止中状态
	state = 2;
}






