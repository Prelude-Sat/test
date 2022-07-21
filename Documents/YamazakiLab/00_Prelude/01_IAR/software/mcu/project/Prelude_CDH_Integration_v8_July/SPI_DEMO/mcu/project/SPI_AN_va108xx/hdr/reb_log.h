/***************************************************************************************
 * @file     reb_log.h
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
#ifndef RUB_LOG_H
#define RUB_LOG_H
#include <stdint.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

typedef enum {
	LOG_ERROR,
	LOG_WARN,
	LOG_INFO,
	LOG_DBG,
	LOG_TRACE
} VOR_LOG_LEVEL_TYPE;

typedef enum {
	PORTA_UARTA = 1,
	PORTA_UARTB,
	PORTB_UARTA,
	PORTB_UARTB,
	RTT
} VOR_LOG_PORT_TYPE;

#define LOG_PORTA_UARTA_TX_BIT    9         
#define LOG_PORTA_UARTA_RX_BIT    8         
#define LOG_PORTA_UARTA_RTS_BIT   7         
#define LOG_PORTA_UARTA_CTS_BIT   6   

#define LOG_PORTA_UARTB_TX_BIT    5         
#define LOG_PORTA_UARTB_RX_BIT    4         
#define LOG_PORTA_UARTB_RTS_BIT   3        
#define LOG_PORTA_UARTB_CTS_BIT   2 

#define LOG_PORTB_UARTA_TX_BIT    23         
#define LOG_PORTB_UARTA_RX_BIT    22         
#define LOG_PORTB_UARTA_RTS_BIT   21         
#define LOG_PORTB_UARTA_CTS_BIT   20    

#define LOG_PORTB_UARTB_TX_BIT    19         
#define LOG_PORTB_UARTB_RX_BIT    18         
#define LOG_PORTB_UARTB_RTS_BIT   17         
#define LOG_PORTB_UARTB_CTS_BIT   16  

void VOR_Log(VOR_LOG_LEVEL_TYPE log_level, const char *fmt, ...);
void VOR_Log_Init(VOR_LOG_PORT_TYPE port, VOR_LOG_LEVEL_TYPE log_level);
#endif
//------------------------------------------------------------------------------
