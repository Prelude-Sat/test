/***************************************************************************************
 * @file     reb_max11619.c
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
#include "va108xx.h"
#include "driver_spi.h"
#include "reb_max11619.h"

static VOR_DRIVER_SPI *pSPI; 

int32_t VOR_MAX11619_Initialize(void) 
{
	VOR_GPIO->BANK[0].DIR &= ~(0x01UL << 14); ///< Set-UP PORTA[14] GPIO as input to monitor EOC
	
	VOR_GPIO_PinMux(GPORTA, PORTA_SPIB_SSEL0, FUNSEL2); ///< Configure Port A[17] as SPIB SSEL0 
	VOR_GPIO_PinMux(GPORTA, PORTA_SPIB_MISO, FUNSEL2);	///< Configure Port A[18] as SPIB MISO 
	VOR_GPIO_PinMux(GPORTA, PORTA_SPIB_MOSI, FUNSEL2);	///< Configure Port A[19] as SPIB MOSI 
	VOR_GPIO_PinMux(GPORTA, PORTA_SPIB_SCLK, FUNSEL2); 	///< Configure Port A[20] as SPIB sclk 

	pSPI = &Driver_SPI1;
	pSPI->Initialize(NULL); 
	pSPI->Control(VOR_SPI_DRV_POLL, VOR_SPI_CTRL_ENABLE); 					///< Select Poll Mode Driver
	pSPI->Control(VOR_SPI_POLL_TOUT, 1000000); 											///< Set polling time-out to 1 sec
	pSPI->Control(VOR_SPI_CTRL_DATA_WIDTH, 8); 											///< Set data-width to 8 bit
	pSPI->Control(VOR_SPI_CTRL_CLKMODE, VOR_SPI_CLKMODE_SPH0_SPO0); ///< Select SPI mode 00
	pSPI->Control(VOR_SPI_CTRL_CS, 0); 															///< Slave select 0 
	pSPI->Control(VOR_SPI_CTRL_CLKRATE, 3000000); 								  ///< Set SPI clock rate to 3 MHz
	pSPI->Control(VOR_SPI_CTRL_MS, VOR_SPI_MASTER);									///< Set CPU SPI master
	pSPI->Control(VOR_SPI_CTRL_BLOCKMODE, VOR_SPI_CTRL_ENABLE); 		///< Enable block mode 
	pSPI->Control(VOR_SPI_CTRL_INTERFACE, VOR_SPI_CTRL_ENABLE); 		///< Enable SPI interface
	
	return 0; 
}

int32_t VOR_MAX11619_UnInitialize(void) 
{
	pSPI->Uninitialize(); 
	
	return 0; 
}

int32_t VOR_MAX11619_Configure(void) 
{
	int32_t ret_code = 0; 
	uint8_t cmd; 
	
	/* Reset ADC */ 
	cmd = MAX11619_RESET; 
	if( (ret_code = pSPI->Send(&cmd, sizeof(uint8_t))) != 0 )
		return ret_code; 
	
	/* Use internal clock and external reference */ 
	cmd = MAX11619_CLKSEL10_EXTREF; 
	if( (ret_code = pSPI->Send(&cmd, sizeof(uint8_t))) != 0 )
		return ret_code; 
	
	/* Switch off averaging */ 
	cmd = MAX11619_AVG_OFF; 
	if( (ret_code = pSPI->Send(&cmd, sizeof(uint8_t))) != 0 )
		return ret_code; 
	
	/* Select input channel AN3 */ 
	cmd = MAX11619_CHANNEL_AN3; 
	ret_code = pSPI->Send(&cmd, sizeof(uint8_t)); 

	return ret_code; 
}

int32_t VOR_MAX11619_GetData(uint16_t *adc_data) 
{
	int32_t ret_code = 0; 
	uint8_t tmp_val[2] = {0, 0}; 
	
	/* Wait till ADC conversion is complete i.e till EOC goes low. */ 
	while( VOR_GPIO->BANK[0].DATAIN & (0x01UL << 14) ); 

	/* Read new sample data from ADC FIFO */   
	if( (ret_code = pSPI->Receive(tmp_val, sizeof(uint16_t))) != 0 )
		return ret_code; 
	
	*adc_data = 0;
	*adc_data |= tmp_val[1]>>2; 
	*adc_data |= ((uint16_t)tmp_val[0])<<6; 

	return 0; 
}
