#include "I2C_soft.h"

#include "systick.h"

#define SCL_RCU			RCU_GPIOB
#define SCL_PORT		GPIOB
#define SCL_PIN			GPIO_PIN_6

#define SDA_RCU			RCU_GPIOB
#define SDA_PORT		GPIOB
#define SDA_PIN			GPIO_PIN_7


#define SDA(bit) 	gpio_bit_write(SDA_PORT, SDA_PIN, bit ? SET : RESET)
#define SCL(bit) 	gpio_bit_write(SCL_PORT, SCL_PIN, bit ? SET : RESET)
#define DELAY()  	delay_1us(5)
// 设置SDA为输入模式
#define SDA_IN()	gpio_mode_set(SDA_PORT, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, SDA_PIN);
// 设置SDA为输出模式
#define SDA_OUT()	gpio_mode_set(SDA_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, SDA_PIN);
// 读取SDA电平
#define SDA_STATE() gpio_input_bit_get(SDA_PORT, SDA_PIN)

// SCL-PB6时钟线,开漏
// SDA-PB7数据线,开漏
void I2C_soft_init(){
	// 软实现：CPU操作GPIO
	// 硬实现：操作寄存器，AF复用，硬件电路实现数据收发
	
	// 标准模式 100Kbits/s， 快速模式 400Kbits/s
	// 100Kbits/s -> 100 000 bit / 1 000 000 us  -> 1bit/10us => 5us
	// 400Kbits/s -> 400 000 bit / 1 000 000 us  -> 4bit/10us => 1.+us
//	__NOP();
	// SCL-PB6 (默认低电平)
	rcu_periph_clock_enable(SCL_RCU);
	gpio_mode_set(SCL_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, SCL_PIN);
	gpio_output_options_set(SCL_PORT, GPIO_OTYPE_OD, GPIO_OSPEED_MAX, SCL_PIN);
	
	// SDA-PB7 (默认低电平)
	rcu_periph_clock_enable(SDA_RCU);
	gpio_mode_set(SDA_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, SDA_PIN);
	gpio_output_options_set(SDA_PORT, GPIO_OTYPE_OD, GPIO_OSPEED_MAX, SDA_PIN);
	
	
	
}

static void start();
static void send(uint8_t data);
static uint8_t recv();
static void send_ack();
static void send_nack();

static int8_t wait_ack();
static void stop();

/**********************************************************
 * @brief 写数据
 * @param addr: 设备地址 7bit
 * @param reg:  寄存器地址
 * @param data: 字节数组
 * @param len: 数据个数
 * @return 是否写成功0成功，其他失败
 **********************************************************/
int8_t I2C_soft_write(uint8_t addr, uint8_t reg, uint8_t* data, uint32_t len){
	
	// 开始
	start();
	
	// 发送设备地址（设备写地址）
	send((addr << 1) | 0);
	// 等待响应
	if(wait_ack() != 0) return 1;
	
	// 发送寄存器地址
	send(reg);
	// 等待响应
	if(wait_ack() != 0) return 2;
	
	// 循环(长度len)
	for(uint32_t i = 0; i < len; i++){
		// 写出1字节数据
		send(data[i]);
		// 等待响应
		if(wait_ack()) return 3;
	}
	
	// 停止
	stop();
	
	return 0;
}

/**********************************************************
 * @brief 读数据
 * @param addr: 设备地址 7bit
				举例： 0x51 -> 写0xA2, 读0xA3
			0x51 -> 0b01010001
			0xA2 ->  0b10100010		左移1位，末尾补0
			0xA3 ->  0b10100011		左移1位，末尾补1
 * @param reg:  寄存器地址
 * @param data: 字节数组
 * @param len:  数据个数
 * @return 是否读成功0成功，其他失败
 **********************************************************/
int8_t I2C_soft_read(uint8_t addr, uint8_t reg, uint8_t* data, uint32_t len){
	uint8_t addr_write = (addr << 1) | 0;	// 8bit设备的写地址 
	uint8_t addr_read  = (addr << 1) | 1;	// 8bit设备的读地址 
	
	// 开始 ---------------------------------- 通用开头
	start();
	
	// 发送设备写地址
	send(addr_write);
	// 等待响应
	if(wait_ack()) return 1;
	
	// 发送寄存器地址（要读的寄存器首地址）
	send(reg);
	// 等待响应
	if(wait_ack()) return 2;
	
	// 开始 ---------------------------------- 读取开头
	start();
	
	// 发送设备读地址
	send(addr_read);
	// 等待响应
	if(wait_ack()) return 3;
	
	/************** 循环接收数据 **************/
	for(uint32_t i = 0;i < len; i++){ // 一个循环接收一个字节
		// 接收一个字节
		data[i] = recv();
		
		if (i != len - 1){
			// 发送响应 (非最后一条)
			send_ack();			
		}else {
			// 发送空响应
			send_nack();
		}
		
	}
	/*****************************************/
	
	// 停止
	stop();
	return 0;
}

static void start(){
	
	// 将SDA和SCL拉高
	SDA(1);
	DELAY();
	SCL(1);
	DELAY();
	
	// 将SDA和SCL依次拉低
	SDA(0);
	DELAY();
	SCL(0);
	DELAY();
}


static void send(uint8_t data){
	// 发送8bit，先发高位
	// 1010 1000
	// 0101 0000
	// 101 00000
	
	for(uint8_t i = 0; i < 8; i++){
		// 取出最高位
		if(data & 0x80){
			SDA(1); // 高电平
		}else {
			SDA(0); // 低电平
		}
		DELAY();
		
		// 数据有效期
		SCL(1);
		DELAY();
		SCL(0);
		DELAY();
		
		// 内容向左移动一位
		data <<= 1;
	}
}

uint8_t recv(){
	// 释放SDA控制权，进入输入模式
	SDA_IN();
	uint8_t cnt = 8; 			// 1byte字节， 8bit比特
	uint8_t data = 0x00;	// 空字节容器接收数据
	
	while(cnt--){					// 接收一个bit(先收到的是高位)
		// SCL拉低
		SCL(0); // 等待从设备准备数据
		DELAY();
		
		SCL(1); // 设置数据有效性
		
		// 0000 0000 -> 1110 1011
		// 0000 0010
		
		// 0000 0001  8
		// 0000 0011  7
		// 0000 0111  6
		// 0000 1110  5
		// 0001 1101  4
		// 0011 1010  3
		// 0111 0101  2
		// 1110 1011  1
		
		// 整体左移
		data <<= 1;
		// 将最低位设置为1
//		if(SDA_STATE()) data |= 0x01;
		if(SDA_STATE()) data++;
		
		DELAY();
	}
	// 最后一次低电平，不能忘
	SCL(0);
	
	return data;
}

static void send_ack(){
	// 主机获取SDA控制权
	SDA_OUT();
	
	// 拉低SDA
	SDA(0);
	DELAY();
	
	// 拉高SCL
	SCL(1);
	DELAY();
	
	// 拉低SCL (从设备读取SDA上的电平)
	SCL(0);
	DELAY();
}
static void send_nack(){
	// 主机获取SDA控制权
	SDA_OUT();
	
	// 拉低SDA
	SDA(1);
	DELAY();
	
	// 拉高SCL
	SCL(1);
	DELAY();
	
	// 拉低SCL (从设备读取SDA上的电平)
	SCL(0);
	DELAY();

}

static int8_t wait_ack(){
	// 拉高SDA, 等待从设备拉低
	SDA(1);
	DELAY();
	
	// 拉高SCL，同时释放SDA权限（变成输入模式）
	SCL(1);
	SDA_IN();
	DELAY();
	
	if(SDA_STATE() == RESET){ // 低电平
		// 从设备拉低了SDA，应答成功
		SCL(0);
		// 获取控制权，变成输出模式
		SDA_OUT();
	}else {
		// 获取控制权，变成输出模式
		SDA_OUT();
		// 无人应答
		stop();
		return 1;
	}
	
	return 0;
}

static void stop(){
	// 获取控制权，变成输出模式
	SDA_OUT();
	
	// 将SCL和SDA拉低
	SCL(0);
	SDA(0);
	DELAY();
	
	SCL(1);
	DELAY();
	SDA(1);
	DELAY();
}



