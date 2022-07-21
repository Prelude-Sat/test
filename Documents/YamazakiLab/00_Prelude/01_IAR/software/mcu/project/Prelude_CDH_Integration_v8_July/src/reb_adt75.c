/***************************************************************************************
 * @file     reb_adt75.c
 * @version  V1.0
 * @date     31. March 2016
 *
 * @note
 * VORAGO Technologies 
 *
 * @note
 * Copyright (c) 2013-2016 VORAGO Technologies. 
 *
 * @par
 * BY DOWNLOADING, INSTALLING OR USING THIS SOFTWARE, YOU AGREE TO BE BOUND BY 
 * ALL THE TERMS AND CONDITIONS OF THE VORAGO TECHNOLOGIES END USER LICENSE AGREEMENT. 
 * THIS SOFTWARE IS PROVIDED "AS IS". NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY 
 * AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. VORAGO TECHNOLOGIES 
 * SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL 
 * DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 ****************************************************************************************/
#include "driver_i2c.h"
#include "reb_adt75.h"

int32_t VOR_ADT75_Initialize(void) 
{
	int32_t retCode = 0; 
	VOR_DRIVER_I2C *pI2C = &Driver_I2C0;
	
	pI2C->Initialize(NULL); 
	pI2C->Control(VOR_I2C_DRV_POLL, VOR_I2C_CTRL_ENABLE); 					/* Select poll mode driver */  
	pI2C->Control(VOR_I2C_POLL_TOUT, 100000); 										  /* Set poll time-out time to 100 msec */ 
	pI2C->Control(VOR_I2C_BUS_SPEED, VOR_I2C_BUS_SPEED_STANDARD); 	/* Select standard speed */ 
	pI2C->Control(VOR_I2C_ADDRESS_10BIT, VOR_I2C_CTRL_DISABLE); 		/* 7 bit address mode */ 
	pI2C->Control(VOR_I2C_OWN_ADDRESS, ADT75_DEVICE_ADDRESS);
	
	return retCode; 
}

int32_t VOR_ADT75_UnInitialize(void) 
{
	int32_t retCode = 0; 
	VOR_DRIVER_I2C *pI2C = &Driver_I2C0;
	
	pI2C->Uninitialize(); 
	
	return retCode; 
}

int32_t VOR_ADT75_GetTemp(uint8_t *cur_temp) 
{
	int32_t retCode = 0; 
	uint8_t rx_data[2], reg_addrs; 
	VOR_DRIVER_I2C *pI2C = &Driver_I2C0;
	reg_addrs = ADT75_TEMP_REG; 

	retCode = pI2C->MasterTransmit(ADT75_DEVICE_ADDRESS, &reg_addrs, sizeof(uint8_t), false);	
	if( retCode == 0 )
		retCode = pI2C->MasterReceive(ADT75_DEVICE_ADDRESS, rx_data, sizeof(uint8_t)*2, false);	
	*cur_temp = (rx_data[0]<<4 | rx_data[1]>>4)>>4; 

	return retCode; 
}
