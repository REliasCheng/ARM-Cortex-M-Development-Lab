#include "SPI0.h"

#if SPI0_USE_SOFT


static void GPIO_config(rcu_periph_enum rcu, uint32_t port, uint32_t pin) {
  // 1. 时钟初始化
  rcu_periph_clock_enable(rcu);
  // 2. 配置GPIO 输入输出模式
  gpio_mode_set(port, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, pin);
  // 3. 配置GPIO 输出选项
  gpio_output_options_set(port, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, pin);
}

void SPI0_init(){
	
	// SCL, MOSI
	GPIO_config(SPI0_SCL_RCU, SPI0_SCL_PORT, SPI0_SCL_PIN);
	GPIO_config(SPI0_MOSI_RCU, SPI0_MOSI_PORT, SPI0_MOSI_PIN);
	
	// MISO
	rcu_periph_clock_enable(SPI0_MISO_RCU);
	gpio_mode_set(SPI0_MISO_PORT, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, SPI0_MISO_PIN);
	
  // 4. 默认输出电平  // CPOL=1, CPHA=1
  gpio_bit_write(SPI0_SCL_PORT, SPI0_SCL_PIN, SET);
  gpio_bit_write(SPI0_MOSI_RCU, SPI0_MOSI_PORT, SET);
	

}

#define SCL(bit) 	gpio_bit_write(SPI0_SCL_PORT, SPI0_SCL_PIN, bit ? SET : RESET);

#define MOSI(bit) gpio_bit_write(SPI0_MOSI_PORT, SPI0_MOSI_PIN, bit ? SET : RESET);

#define MISO()    gpio_input_bit_get(SPI0_MISO_PORT,SPI0_MISO_PIN)//FS0

/**********************************************************
 * @brief 主机通过MOSI输出数据
 * @param data 要写出的数据
 **********************************************************/
void SPI0_write(uint8_t data){ // CPOL=1, SCL默认高电平
	// MSB大端模式
	// 1101 0011
	// 1000 0000
	for(uint8_t i=0; i<8; i++){
		// SCL拉低
		SCL(0);
		// 输出
		MOSI(data&0x80);
		// 左移1位
		data <<= 1;
		// SCL拉高
		SCL(1);
	}
}

/**********************************************************
 * @brief 主机通过MISO读取数据 (从机)
 * @return data 读到的数据
 **********************************************************/
uint8_t SPI0_read(){ // CPOL=1, MISO
	// 大端模式
	// 0000 0000 <- 1101 0101
	uint8_t i, read = 0x00 ;
	for(i = 0; i < 8; i++){
		// 拉低SCL
		SCL(0);
		
		read <<= 1;
		if(MISO()) read++;
		
		// 拉高SCL
		SCL(1);
	}
	
	return read;
}
uint8_t SPI0_read_write(uint8_t data){ // CPOL=1, MISO
	// 大端模式
	// 0000 0000 <- 1101 0101
	uint8_t i, read = 0x00 ;
	for(i = 0; i < 8; i++){
		// 拉低SCL
		SCL(0);
		
		// MOSI输出
		MOSI(data&0x80);
		// 左移1位
		data <<= 1;
		
		// MISO输入
		read <<= 1;
		if(MISO()) read++;
		
		// 拉高SCL
		SCL(1);
	}
	
	return read;
}


#else

void SPI0_init(){
	// GPIO -------------------------------------
	// rcu
	rcu_periph_clock_enable(SPI0_SCL_RCU);
	gpio_mode_set(SPI0_SCL_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, SPI0_SCL_PIN);
	gpio_output_options_set(SPI0_SCL_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, SPI0_SCL_PIN);
	gpio_af_set(SPI0_SCL_PORT, GPIO_AF_5, SPI0_SCL_PIN);
	
	rcu_periph_clock_enable(SPI0_MOSI_RCU);
	gpio_mode_set(SPI0_MOSI_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, SPI0_MOSI_PIN);
	gpio_output_options_set(SPI0_MOSI_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, SPI0_MOSI_PIN);
	gpio_af_set(SPI0_MOSI_PORT, GPIO_AF_5, SPI0_MOSI_PIN);
	
	rcu_periph_clock_enable(SPI0_MISO_RCU);
	gpio_mode_set(SPI0_MISO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, SPI0_MISO_PIN);
	gpio_af_set(SPI0_MISO_PORT, GPIO_AF_5, SPI0_MISO_PIN);
	
	// SPI  -------------------------------------
	// rcu
	rcu_periph_clock_enable(RCU_SPI0);
	
	/* deinitialize SPI and I2S */
	spi_i2s_deinit(SPI0);
	spi_parameter_struct spi_struct;
	/* 使用默认参数初始化结构体 initialize the parameters of SPI struct with default values */
	spi_struct_para_init(&spi_struct);
	
	spi_struct.device_mode          = SPI_MASTER;								// 设备模式
	spi_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;	// 传输模式
	spi_struct.frame_size           = SPI_FRAMESIZE_8BIT;				// 每一帧的数据位数
	spi_struct.nss                  = SPI_NSS_SOFT;							// 软片选
	spi_struct.clock_polarity_phase = SPI_CK_PL_HIGH_PH_2EDGE;	// CPOL=1, CPHA=1
	spi_struct.prescale             = SPI_PSC_16;								// 分频系数 84M / 16 = 5.25M
	spi_struct.endian               = SPI_ENDIAN_MSB;						// 大小端模式
	/* 初始化SPI initialize SPI parameter */
	spi_init(SPI0, &spi_struct);
	/* 使能SPI enable SPI */
	spi_enable(SPI0);

}

void SPI0_write(uint8_t data){
	/* 循环等待发送缓冲区，直到为空 */
	while(RESET == spi_i2s_flag_get(SPI0, SPI_FLAG_TBE));
	/* 通知外设电路发数据 SPI transmit data*/
	spi_i2s_data_transmit(SPI0, data);
	
	/* 循环等待接收缓冲区，直到不为空 */
	while(RESET == spi_i2s_flag_get(SPI0, SPI_FLAG_RBNE));
	/* 读取数据 SPI receive data */
	spi_i2s_data_receive(SPI0);
}

uint8_t SPI0_read(){
	/* 循环等待发送缓冲区，直到为空 */
	while(RESET == spi_i2s_flag_get(SPI0, SPI_FLAG_TBE));
	/* 通知外设电路发数据 SPI transmit data*/
	spi_i2s_data_transmit(SPI0, 0x00);
	
	/* 循环等待接收缓冲区，直到不为空 */
	while(RESET == spi_i2s_flag_get(SPI0, SPI_FLAG_RBNE));
	/* 读取数据 SPI receive data */
	uint8_t dat = spi_i2s_data_receive(SPI0);
	return dat;
}

uint8_t SPI0_read_write(uint8_t data){
	/* 循环等待发送缓冲区，直到为空 */
	while(RESET == spi_i2s_flag_get(SPI0, SPI_FLAG_TBE));
	/* 通知外设电路发数据 SPI transmit data*/
	spi_i2s_data_transmit(SPI0, data);
	
	/* 循环等待接收缓冲区，直到不为空 */
	while(RESET == spi_i2s_flag_get(SPI0, SPI_FLAG_RBNE));
	/* 读取数据 SPI receive data */
	return spi_i2s_data_receive(SPI0);
}

#endif
















