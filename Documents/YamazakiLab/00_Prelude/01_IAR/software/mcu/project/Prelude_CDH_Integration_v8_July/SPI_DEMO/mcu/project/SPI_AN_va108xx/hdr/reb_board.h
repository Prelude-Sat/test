/***************************************************************************************
 * @file     reb_board.h
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
#ifndef __REB_BOARD_H
#define __REB_BOARD_H

#include "va108xx.h"

#define PORTA_10_D2 				10 
#define PORTA_7_D3 					7 
#define PORTA_6_D4	 				6 
#define PORTA_11_SW_USER 		11

/**
	\brief IRQ0 services milli second timer  
*/ 
#define IRQ_TIM0_PRIORITY  0
#define IRQ_TIM0_INDEX     0
#define IRQ_TIM0_IRQn     OC0_IRQn

/**
	\brief IRQ1 services seconds timer 
*/ 
#define IRQ_TIM1_PRIORITY  0
#define IRQ_TIM1_INDEX     1
#define IRQ_TIM1_IRQn     OC1_IRQn

/**
	\brief IRQ2 not used
*/ 

/** 
	\brief IRQ3 services UART0 
*/ 
#define IRQ_UART0_PRIORITY  0
#define IRQ_UART0_INDEX     3
#define IRQ_UART0_IRQn     OC3_IRQn

/** 
	\brief IRQ4 services UART1 
*/ 
#define IRQ_UART1_PRIORITY  0
#define IRQ_UART1_INDEX     4
#define IRQ_UART1_IRQn     OC4_IRQn

/** 
	\brief IRQ5 services GPIO
*/ 
#define IRQ_GPIO_PRIORITY  0
#define IRQ_GPIO_INDEX     5
#define IRQ_GPIO_IRQn     OC6_IRQn

/** 
	\brief IRQ6 services SPI0 
*/  
#define IRQ_SPI0_PRIORITY  0
#define IRQ_SPI0_INDEX     6
#define IRQ_SPI0_IRQn     OC7_IRQn

/** 
	\brief IRQ7 services SPI1 
*/ 
#define IRQ_SPI1_PRIORITY  0
#define IRQ_SPI1_INDEX     7
#define IRQ_SPI1_IRQn     OC7_IRQn

/** 
	\brief IRQ8 services SPI2 
*/ 
#define IRQ_SPI2_PRIORITY  0
#define IRQ_SPI2_INDEX     8
#define IRQ_SPI2_IRQn     OC8_IRQn

/** 
	\brief IRQ9 services I2C0 in master mode  
*/ 
#define IRQ_I2C0_MS_PRIORITY  0
#define IRQ_I2C0_MS_INDEX     9
#define IRQ_I2C0_MS_IRQn     OC9_IRQn

/** 
	\brief IRQ10 services I2C0 in slave mode 
*/ 	
#define IRQ_I2C0_SL_PRIORITY  0
#define IRQ_I2C0_SL_INDEX     10
#define IRQ_I2C0_SL_IRQn     OC10_IRQn

/** 
	\brief IRQ11 services I2C1 in master mode 
*/ 
#define IRQ_I2C1_MS_PRIORITY  0
#define IRQ_I2C1_MS_INDEX     11
#define IRQ_I2C1_MS_IRQn     OC11_IRQn

/** 
	\brief IRQ12 services I2C1 in slave mode 
*/ 
#define IRQ_I2C1_SL_PRIORITY  0
#define IRQ_I2C1_SL_INDEX     12
#define IRQ_I2C1_SL_IRQn     OC12_IRQn

#endif /* __REB_BOARD_H */ 
