/***************************************************************************************
* @file     pp_spi.h
* @version  V1.0
* @date     22 January 13
*
* @note
* VORAGO Technologies / TOMMY 
*
* @note
* Copyright (c) 2013-2016 VORAGO Technologies. 
*
* @par
* BY DOWNLOADING, INSTALLING OR USING THIS SOFTWARE, YOU AGREE TO BE BOUND BY 
ALL THE TERMS AND CONDITIONS OF THE VORAGO TECHNOLOGIES END USER LICENSE AGREEMENT. 
* THIS SOFTWARE IS PROVIDED "AS IS". NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, 
INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
* VORAGO TECHNOLOGIES SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL,
 INCIDENTAL, OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
*
****************************************************************************************/

#include <stdio.h>
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
#include "sleep.h"

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
#define FUNCSEL0 00                                                             // 1つのピンに3つの役割が存在するため選択する必要がある
#define FUNCSEL1 01                                                             
#define FUNCSEL2 02
#define FUNCSEL3 03

#define SPIA_DATA_BMSTOP_Pos                31                                                       /*!< SPIC DATA: BMSTOP Position            */
#define SPIA_DATA_BMSTOP_Msk                (0x01UL << SPIA_DATA_BMSTOP_Pos)                         /*!< SPIC DATA: BMSTOP Mask                */

#define SPI_PERIPHERAL_CTRL1_BLOCKMODE_Pos     7                                                       /*!< SPI_PERIPHERAL CTRL1: BYTEMODE Position */
#define SPI_PERIPHERAL_CTRL1_BLOCKMODE_Msk     (0x01UL << SPI_PERIPHERAL_CTRL1_BLOCKMODE_Pos)          //
#define SPI_PERIPHERAL_DATA_BMSTOP_Msk 0x80000000

/*** Start Duration of read (ms) ***/
extern void start_time(void);

/*** End Duration of read (ms) ***/
extern void stop_time(void);

/*** Set Up Port A and B ***/
extern void REB1_SPIB_setup(int CSB);
/*
CSB is SPIB Slave Select Pin Define
SPI_SSELBn[0] = PORTA[17]/SPI_SSELBn[7] = PORTA[10] etc.
->10～17 Pin is CSB Pin
*/ 

extern void REB1_SPIA_setup(int CSA);
/*
CSA is SPIA Slave Select Pin Define
SPI_SSELAn[0] = PORTA[28]/SPI_SSELAn[7] = PORTA[21] etc.
->21～28 Pin is CSA Pin
*/

/*** Set Up SPI Frequency ***/
extern void spi_frequency(int SELECT_PORT,uint8_t PRESCALE,uint8_t SCRDV,int slave);
//SELECT_PORT;0->A,1->B,2->C

/*
SPIA -> To MIS MPU/CAM/SD/XLink -> 12.5MHz (Initialize sd -> 0.1MHz~0.4MHz)
SPIB -> from CDH ->
Initialize sd -> PRESCALE = 0x32 , SCRDV = 0x09 -> 0.1MHz
12.5MHz -> PRESCALE = 0x02 , SCRDV = 0x01
*/

//SPI.WRITE(Input uint8_t DATA);
extern uint8_t spi_write(int SELECT_PORT,volatile uint8_t data);
extern uint8_t spi_read(int SELECT_PORT,unsigned char *rbp,int bytes);

extern void SPI_DATASET(int SELECT_PORT);
extern void SPI_CTRL(int SELECT_PORT,int SS,int Slave);
//extern void spi_cs_ctrl(int SELECT_PORT,int SS,int DS,int Slave,unsigned char *ibp);
//SS はSlave Number（0,1,2。。。）のことを指す,CSはHigh（1）かLow（0）

void spi_transfer(int SELECT_PORT, uint8_t* tx_buf, uint16_t length);
void spi_read_data(int SELECT_PORT, uint8_t* rx_buf, uint16_t length);


// デバック用の関数
void data_debug_print(const uint8_t* data, uint16_t bytes);
