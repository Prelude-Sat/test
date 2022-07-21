/***************************************************************************************
* @file     pp_adc.h
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

//#ifndef PERIPH_CLK_ENAB_PORTA

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "va108xx.h"
#include "reb_log.h"
#include "reb_board.h"
#include "reb_adt75.h"
#include "driver_common.h"
#include "reb_max11619.h"
#include "reb_timer.h"
#include "gpio_va108xx.h"
#include "segger_rtt.h" 
#include "irq_va108xx.h"

//#endif

#ifndef _pp_adc_h
#define _pp_adc_h


//**** Macro Definitions
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

#define I2C_ADDR_TX 0xAE        // Define the I2C TX Address:0xAE
#define I2C_ADDR_RX 0xAF        // Define the I2C RX Address:0xAF

#define EM 0                    // Define the program for EM = 1/ BBM = 0
#define TRANCE 0
#define SWITCH 0
#define Detail 0


/* Type definition */
typedef struct{
  uint8_t TLM[64], TC[64], flag;
  int8_t  STATUS;
  int16_t HK_normal[128], HK_IniRate[32], HK_Detumbling[32], HK_Pitch[32], HK_Wheel[32];
  float   HK_detail[128];
} ADC_t;


/* Prototype declaration */
/* Sequence */
void    PowerON_ADC(ADC_t*);
void    HKInitial_ADC(ADC_t*);
void    EstAngRate_ADC(ADC_t*);
void    Detumbling_ADC(ADC_t*);
void    PitchEst_ADC(ADC_t*, int*);
void    Y_Wheel_ADC(ADC_t*);

/* Predefined Process */
void    adc_get_HK(ADC_t*);
void    HK_IniRateTele(ADC_t*);
void    HK_DetumblTele(ADC_t*);
void    HK_PitchEst(ADC_t*);
void    HK_Y_Wheel(ADC_t*);

/* HK */
void    HK_data(int, ADC_t*);
void    HK_trans(ADC_t*);
void    HK_Detumb(int, ADC_t*);
void    PitchEst_HK(int, ADC_t*);
void    Y_Wheel_HK(int, ADC_t*);

/* Function */
void TC_data(int,ADC_t*);
void TLM_print(int, ADC_t*);
void HK_print(int, ADC_t*);
uint8_t eclipse(ADC_t*);

/* I2C */
void    I2CA_CubeComputer(int, ADC_t*);
void    I2CB_CubeComputer(int, ADC_t*);
uint8_t TC_Byte(int);
uint8_t TLM_Byte(int);

/* Bit shift */
int32_t BS_32(int32_t,int32_t,int32_t,int32_t);
int16_t BS_16(int32_t,int32_t);
void    BS_4(int32_t,int8_t*);
void    BS_2(int32_t,int8_t*);
void    BS_1(int32_t,int8_t*);

#endif