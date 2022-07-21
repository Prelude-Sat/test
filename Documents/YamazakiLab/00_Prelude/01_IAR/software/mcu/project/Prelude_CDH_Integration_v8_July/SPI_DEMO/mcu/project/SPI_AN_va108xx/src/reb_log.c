/***************************************************************************************
 * @file     reb_log.c
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
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include "reb_log.h"
#include "driver_gpio.h"
#include "driver_uart.h"
#include "va108xx.h"
#include "segger_rtt.h"

#ifndef RTT_LOG
static VOR_DRIVER_UART *drv;
#endif 

static uint8_t log_buff[100];
static VOR_LOG_LEVEL_TYPE log_level; 
extern FILE __stdout;
VOR_LOG_PORT_TYPE port_type; 
	
void VOR_Log(VOR_LOG_LEVEL_TYPE level, const char *fmt, ...) {
	va_list arg;
	uint32_t len; 
	
	/* Check logger level */
	if( level > log_level )
		return;

	/* Log message to terminal */
	va_start(arg, fmt);
	if( level == LOG_ERROR )
		sprintf((char *)log_buff, ANSI_COLOR_RED); 
	else
		sprintf((char *)log_buff, ANSI_COLOR_BLUE); 
	len = strlen((char *)log_buff);
	vsnprintf((char *)(log_buff + len), sizeof(log_buff), fmt, arg);
	va_end(arg);

#ifdef RTT_LOG
		SEGGER_RTT_printf(0, (const char *)log_buff);
#else
		drv->Send(log_buff, len);
#endif 
}

void VOR_Log_Init(VOR_LOG_PORT_TYPE port, VOR_LOG_LEVEL_TYPE _log_level) {
	port_type = port; 
	log_level = _log_level; 
	
#ifdef RTT_LOG
	SEGGER_RTT_Init();
	SEGGER_RTT_ConfigUpBuffer(0, NULL, NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_SKIP);
	return; 
#else	
	switch( port_type ) {
		case PORTA_UARTA:
			VOR_GPIO_PinMux(GPORTA,LOG_PORTA_UARTA_TX_BIT,FUNSEL2);
			VOR_GPIO_PinMux(GPORTA,LOG_PORTA_UARTA_RX_BIT,FUNSEL2);
			VOR_GPIO_PinMux(GPORTA,LOG_PORTA_UARTA_RTS_BIT,FUNSEL2);
			VOR_GPIO_PinMux(GPORTA,LOG_PORTA_UARTA_CTS_BIT,FUNSEL2);
			break;
		case PORTA_UARTB:
			VOR_GPIO_PinMux(GPORTA,LOG_PORTA_UARTB_TX_BIT,FUNSEL2);
			VOR_GPIO_PinMux(GPORTA,LOG_PORTA_UARTB_RX_BIT,FUNSEL2);
			VOR_GPIO_PinMux(GPORTA,LOG_PORTA_UARTB_RTS_BIT,FUNSEL2);
			VOR_GPIO_PinMux(GPORTA,LOG_PORTA_UARTB_CTS_BIT,FUNSEL2);
			break;
		case PORTB_UARTA:
			VOR_GPIO_PinMux(GPORTB,LOG_PORTB_UARTA_TX_BIT,FUNSEL1);
			VOR_GPIO_PinMux(GPORTB,LOG_PORTB_UARTA_RX_BIT,FUNSEL1);
			VOR_GPIO_PinMux(GPORTB,LOG_PORTB_UARTA_RTS_BIT,FUNSEL2);
			VOR_GPIO_PinMux(GPORTB,LOG_PORTB_UARTA_CTS_BIT,FUNSEL2);
			break;
		case PORTB_UARTB:
			VOR_GPIO_PinMux(GPORTB,LOG_PORTB_UARTB_TX_BIT,FUNSEL2);
			VOR_GPIO_PinMux(GPORTB,LOG_PORTB_UARTB_RX_BIT,FUNSEL2);
			VOR_GPIO_PinMux(GPORTB,LOG_PORTB_UARTB_RTS_BIT,FUNSEL2);
			VOR_GPIO_PinMux(GPORTB,LOG_PORTB_UARTB_CTS_BIT,FUNSEL2);
			break;
		case RTT: 
			SEGGER_RTT_Init();
			SEGGER_RTT_ConfigUpBuffer(0, NULL, NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_SKIP);
			return; 
		default :
			break;
	};
	
	if( (port_type == PORTA_UARTA) || (port_type == PORTB_UARTA) )
		drv = &Driver_UART0;
	else if( (port_type == PORTA_UARTB) || (port_type == PORTB_UARTB) )
		drv = &Driver_UART1;
	
	drv->Initialize(NULL);
	drv->Control(VOR_UART_CONTROL_PARITY_ENABLED, 0);
	drv->Control(VOR_UART_CONTROL_POLL_MODE, 1);
	drv->Control(VOR_UART_CONTROL_STOP_BIT, VOR_UART_CONTROL_STOP_BIT_1);
	drv->Control(VOR_UART_CONTROL_DATA_BITS, VOR_UART_CONTROL_DATA_BITS_8);
	drv->Control(VOR_UART_CONTROL_LOOPBACK, 0);
	drv->Control(VOR_UART_CONTROL_AUTOCTS, 0);
	drv->Control(VOR_UART_CONTROL_POLL_TO, 300000);
	drv->Control(VOR_UART_CONTROL_BAUD, 9600);
	drv->Control(VOR_UART_CONTROL_TX, 1);
#endif 
}
