/***************************************************************************************
 * @file     reb_max11619.h
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
#ifndef __REB_MAX11619_H
#define __REB_MAX11619_H 

#define MAX11619_RESET					 0x10		/**< Reset all registers to default states */ 
#define MAX11619_CLKSEL10_INTREF 0x68		/**< Internal clock and reference */ 
#define MAX11619_CLKSEL10_EXTREF 0x64		/**< Internal clock and external reference */ 
#define MAX11619_AVG_OFF				 0x20   /**< Switch-off averaging */ 
#define MAX11619_CHANNEL_AN3 		 0x9E 	/**< Select Channel AN3 */ 
#define MAX11619_FIFO						 0x18		/**< Reset all registers and FIFO */ 

#define PORTA_SPIB_SSEL0	17	/**< Configure Port A[17] as SPIB SSEL0 */ 
#define PORTA_SPIB_MISO   18  /**< Configure Port A[18] as SPIB MISO */         
#define PORTA_SPIB_MOSI   19  /**< Configure Port A[19] as SPIB MOSI */        
#define PORTA_SPIB_SCLK   20  /**< Configure Port A[20] as SPIB SCLK */  

/**
	\fn          int32_t VOR_MAX11619_Initialize(void)
  \brief       Initialize Maxim ADC 11619.
  \return      \ref execution_status
*/ 
int32_t VOR_MAX11619_Initialize(void); 

/**
	\fn          int32_t VOR_MAX11619_UnInitialize(void)
  \brief       UnIntialize Maxim ADC 11619.
  \return      \ref execution_status
*/ 
int32_t VOR_MAX11619_UnInitialize(void); 

/**
	\fn          int32_t VOR_MAX11619_Configure(void)
  \brief       Configure Maxim ADC 11619. This call will reset all registers to default values, 
							 switch-off averaging, select internal reference and external reference, and input channel AN3. 
  \return      \ref execution_status
*/ 
int32_t VOR_MAX11619_Configure(void);

/**
	\fn          int32_t VOR_MAX11619_GetData(uint16_t *adc_data)
  \brief       Get current sampled data from MAX11619.
  \param[out]  adc_data  Pointer to uint16_t to receive current sampled data 
  \return      \ref execution_status
*/ 
int32_t VOR_MAX11619_GetData(uint16_t *adc_data);

#endif /* __REB_MAX11619_H */ 


