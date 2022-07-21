/***************************************************************************************
 * @file     reb_timer.c
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
#include <stdbool.h>
#include "va108xx.h"
#include "reb_timer.h"
#include "reb_board.h"
#include "irq_va108xx.h"

static volatile bool msec_tim_timeout = false, sec_tim_timeout = false; 

int32_t VOR_TIM0_MSecInitialize(uint32_t time_msec) {
	int32_t ret_code = 0;
	VOR_SYSCONFIG->TIM_CLK_ENABLE |= 0x1;  
	VOR_TIM->BANK[0].CTRL = 0x0; 
	
	/* Disable IRQ for TIM0 module */ 
	VOR_Disable_Irq(VOR_IRQ_TIM, 0); 

  /* Initialize TIM w/ specified period */ 	
	VOR_TIM->BANK[0].RST_VALUE = time_msec*(SystemCoreClock/1000);  
		
	/* Enable TIM and interrupt */ 
	VOR_TIM->BANK[0].CTRL  = TIM0_CTRL_ENABLE_Msk | TIM0_CTRL_IRQ_ENB_Msk;
	
	/* Enable IRQ for TIM0 module */ 
	VOR_Enable_Irq(VOR_IRQ_TIM, 0);
		
	return ret_code;
}

int32_t VOR_TIM1_SecInitialize(uint32_t time_sec) {
	int32_t ret_code = 0;
	VOR_SYSCONFIG->TIM_CLK_ENABLE |= 0x2;  
	VOR_TIM->BANK[1].CTRL = 0x0;

	/* Enable IRQ for TIM1 module */ 
	VOR_Disable_Irq(VOR_IRQ_TIM, 1); 

  /* Initialize TIM w/ specified period */ 	
	VOR_TIM->BANK[1].RST_VALUE = time_sec*SystemCoreClock;  
		
	/* Enable TIM and interrupt */ 
	VOR_TIM->BANK[1].CTRL  = TIM1_CTRL_ENABLE_Msk | TIM1_CTRL_IRQ_ENB_Msk;
	
	/* Enable IRQ for TIM1 module */ 
	VOR_Enable_Irq(VOR_IRQ_TIM, 1);
		
	return ret_code;
}

void OC0_IRQHandler(void) {
	/* Disable IRQ for TIM0 module */ 
  VOR_Disable_Irq(VOR_IRQ_TIM, 0); 
	
	msec_tim_timeout = true; 

	/* Enable IRQ for TIM0 module */ 
	VOR_Enable_Irq(VOR_IRQ_TIM, 0); 
}

void VOR_TIM1_IRQHandler(void) {
	/* Disable IRQ for TIM1 module */ 
  VOR_Disable_Irq(VOR_IRQ_TIM, 1); 
	
	sec_tim_timeout = true; 
	
	/* Enable IRQ for TIM1 module */ 
	VOR_Enable_Irq(VOR_IRQ_TIM, 1); 
}

bool VOR_TIM0_MSecExpired(void) {
	return msec_tim_timeout ? true: false; 
}

void VOR_TIM0_MSecClear(void) {
	msec_tim_timeout = false; 
}

bool VOR_TIM1_SecExpired(void) {
	return sec_tim_timeout ? true: false; 
}

void VOR_TIM1_SecClear(void) {
	sec_tim_timeout = false; 
}
