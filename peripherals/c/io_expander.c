#include "io_expander.h"

bool io_expander_init(void){
	gpio_config_falling_edge_irq(IO_EXPANDER_IRQ_GPIO_BASE, IO_EXPANDER_IRQ_PIN_NUM);
	io_expander_write_reg(MCP23017_IODIRA_R, 0x00);
	io_expander_write_reg(MCP23017_IODIRB_R, 0xFF);
	io_expander_write_reg(MCP23017_GPINTENB_R, 0xF);
	io_expander_write_reg(MCP23017_INTCONB_R, 0x0);
	io_expander_write_reg(MCP23017_GPPUB_R, 0xFF);
	return true;
};
void io_expander_write_reg(uint8_t reg, uint8_t data){
	i2cSetSlaveAddr(IO_EXPANDER_IRQ_GPIO_BASE, reg	 ,I2C_READ);
	i2cSendByte(IO_EXPANDER_IRQ_GPIO_BASE, data,  reg);
}
uint8_t io_expander_read_reg(uint8_t reg){
	uint8_t data;
	i2cSetSlaveAddr(IO_EXPANDER_IRQ_GPIO_BASE, reg, I2C_WRITE);
	i2cGetByte(IO_EXPANDER_IRQ_GPIO_BASE, &data,  reg);
	return data;
}