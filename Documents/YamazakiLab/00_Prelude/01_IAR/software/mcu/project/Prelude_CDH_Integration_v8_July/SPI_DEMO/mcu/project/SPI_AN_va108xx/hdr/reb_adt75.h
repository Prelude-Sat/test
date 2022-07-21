/***************************************************************************************
 * @file     reb_adt75.h
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
#ifndef __REB_ADT75_H
#define __REB_ADT75_H

/**
	\brief ADT75 Definitions 
*/
#define ADT75_DEVICE_ADDRESS 0x48		/**< ADT75 Slave Device Address */ 
#define ADT75_TEMP_REG			 0x0		/**< ADT75 register to read current temperature */ 

/**
	\fn     int32_t VOR_ADT75_Initialize(void)
  \brief  Initialize ADC temperature sensor ADT75.
  \return \ref execution_status
*/ 
int32_t VOR_ADT75_Initialize(void); 

/**
	\fn     int32_t VOR_ADT75_UnInitialize(void)
  \brief  UnInitialize ADC temperature sensor ADT75.
  \return \ref execution_status
*/ 
int32_t VOR_ADT75_UnInitialize(void); 

/**
	\fn          int32_t VOR_ADT75_GetTemp(uint8_t *cur_temp)
  \brief       Get current temperature from ADT75.
  \param[out]  cur_temp  Pointer to uint8_t to receive temperature data
  \return      \ref execution_status
*/ 
int32_t VOR_ADT75_GetTemp(uint8_t *cur_temp); 

#endif /* __REB_ADT75_H */ 
