/***************************************************************************************
* @file     i2c_b.h
* @version  V1.0
* @date     22 March 28
*
* @note
* VORAGO Technologies / Reizy 
*
* @note
* Copyright (c) 2013-2016 VORAGO Technologies. 
*
* @par
* BY DOWNLOADING, INSTALLING OR USING THIS SOFTWARE, YOU AGREE TO BE BOUND BY 
* ALL THE TERMS AND CONDITIONS OF THE VORAGO TECHNOLOGIES END USER LICENSE AGREEMENT. 
* THIS SOFTWARE IS PROVIDED "AS IS". NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, 
* INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
* VORAGO TECHNOLOGIES SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL,
* INCIDENTAL, OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
*
****************************************************************************************/
#ifndef _pp_i2c_b_h
#define _pp_i2c_b_h

#include "pp_adc.h"

//**** global variable defs
#define PERIPH_CLK_ENAB_PORTA 0x00     
#define PERIPH_CLK_ENAB_PORTB 0x01     
#define PERIPH_CLK_ENAB_SPIA 0x04      
#define PERIPH_CLK_ENAB_SPIB 0x05      
#define PERIPH_CLK_ENAB_SPIC 0x06      
#define PERIPH_CLK_ENAB_UARTA 0x8      
#define PERIPH_CLK_ENAB_UARTB 0x9      
#define PERIPH_CLK_ENAB_I2CA 0x10      
#define PERIPH_CLK_ENAB_I2CB 0x11      
#define PERIPH_CLK_ENAB_IRQSEL 0x15    
#define PERIPH_CLK_ENAB_IOCONFIG 0x16  
#define PERIPH_CLK_ENAB_UTILITY 0x17   
#define PERIPH_CLK_ENAB_PORTIO 0x18    
#define FUNCSEL0 00
#define FUNCSEL1 01
#define FUNCSEL2 02
#define FUNCSEL3 03
//*** I2C
#define I2CB_CLKSCALE_VAL 0x0018    // default to normal mode, divide by 25 for 50 MHz SysClk 
#define I2C_START_STOP_CMD 0x03     // start with stop 

/* main function */
extern void I2CB_TC(uint8_t* I2C_DATA_TX, uint8_t I2C_WORDS_TX);
extern void I2CB_TLM(uint8_t* I2C_DATA_RX, uint8_t* I2C_DATA_TX, uint8_t I2C_WORDS_RX);
/* detail function */
extern uint32_t init_i2cb() ;
extern void i2cb_read(uint8_t* I2C_DATA, uint8_t I2C_WORDS);
extern void i2cb_write(uint8_t* I2C_DATA, uint8_t I2C_WORDS);
#endif