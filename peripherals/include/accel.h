#ifndef __WIRELESS_H__
#define __WIRELESS_H__

#include <stdint.h>
#include <stdio.h>
#include "TM4C123GH6PM.h"

#include "gpio_port.h"
#include "spi.h"
#include "spi_select.h"


//*****************************************************************************
// Fill out the #defines below to configure which pins are connected to
// LSM6DS3HTR
//*****************************************************************************
#define   ACCEL_GPIO_BASE       	GPIOA_BASE
#define   ACCEL_SPI_BASE        	SSI0_BASE

#define   ACCEL_CLK_PIN         	PA2
#define   ACCEL_MISO_PIN        	PA4
#define   ACCEL_MOSI_PIN        	PA5

#define   ACCEL_SPI_CLK_PCTL_M  	GPIO_PCTL_PA2_M
#define   ACCEL_SPI_MISO_PCTL_M  	GPIO_PCTL_PA4_M
#define   ACCEL_SPI_MOSI_PCTL_M  	GPIO_PCTL_PA5_M

#define   ACCEL_CLK_PIN_PCTL    	GPIO_PCTL_PA2_SSI0CLK
#define   ACCEL_MISO_PIN_PCTL   	GPIO_PCTL_PA4_SSI0RX
#define   ACCEL_MOSI_PIN_PCTL   	GPIO_PCTL_PA5_SSI0TX

#define   ACCEL_CS_BASE        		GPIOA_BASE
#define   ACCEL_CS_PIN          	PA3
#define   ACCEL_CS_PORT         	GPIOA

#define   ACCEL_IRQ_GPIO_BASE    GPIOD_BASE
#define   ACCEL_IRQ_PIN          PD3


#define ACCEL_INT1_CTRL_R                     0x0D
#define ACCEL_WHO_AM_I_R											0x0F
#define ACCEL_CTRL1_XL_R											0x10
#define ACCEL_CTRL2_G_R												0x11
#define ACCEL_CTRL5_C_R												0x14
#define ACCEL_OUTX_L_XL												0x28
#define ACCEL_OUTX_H_XL												0x29
#define ACCEL_OUTY_L_XL												0x2A
#define ACCEL_OUTY_H_XL												0x2B
#define ACCEL_OUTZ_L_XL												0x2C
#define ACCEL_OUTZ_H_XL												0x2D

#define ACCEL_SPI_MODE												0

#define ACCEL_SPI_READ												( 1 << 7)
#define ACCEL_SPI_WRITE_N											( 0 << 7)


#define ACCEL_INT1_CTRL_INT1_STEP_DETECTOR		(1 << 7)
#define ACCEL_INT1_CTRL_INT1_SIGN_MOT					(1 << 6)
#define ACCEL_INT1_CTRL_INT1_FULL_FLAG				(1 << 5)
#define ACCEL_INT1_CTRL_INT1_FIFO_OVR					(1 << 4)
#define ACCEL_INT1_CTRL_INT1_FTH							(1 << 3)
#define ACCEL_INT1_CTRL_INT1_BOOT							(1 << 2)
#define ACCEL_INT1_CTRL_INT1_DRDY_G						(1 << 1)
#define ACCEL_INT1_CTRL_INT1_DRDY_XL					(1 << 0)

#define ACCEL_CTRL1_XL_ODR_208HZ							(0x5 << 4)
#define ACCEL_CTRL1_XL_2G											(0x0 << 2)
#define ACCEL_CTRL1_XL_ANTI_ALIAS_50HZ				(0x3 << 0)

#define ACCEL_CTRL2_G_ODR_416HZ								(0x6 << 4)
#define ACCEL_CTRL2_G_FS_245_DPS							(0x0 << 2)
#define ACCEL_CTRL2_G_FS_125									(0x2 << 0)

#define ACCEL_CTRL5_SLEEP_G										(0x1 << 6)
#define ACCEL_CTRL5_INT2_ON_INT1							(0x1 << 5)


//*****************************************************************************
//*****************************************************************************
int16_t accel_read_x(void);

//*****************************************************************************
//*****************************************************************************
int16_t accel_read_y(void);

//*****************************************************************************
//*****************************************************************************
int16_t accel_read_z(void);

//*****************************************************************************
// Used to initialize the ST Micro LSM6DS3H Accelerometer.  The GPIO pins and SPI 
// interface are both configured.
//*****************************************************************************
void accel_initialize(void);


#endif
