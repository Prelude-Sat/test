/***************************************************************************************
 * @file     sleep.c
 * @version  V0.1
 * @date     01 July 2020
 *
 * @note
 * VORAGO Technologies
 *
 * @note
 * Copyright (c) 2013-2020 VORAGO Technologies.
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
 
/*****************************************************************************/ 
/* Include files                                                             */ 
/*****************************************************************************/

#include "sleep.h"

/*****************************************************************************/ 
/* Local pre-processor symbols/macros ('#define')                           */ 
/*****************************************************************************/

#define NUM_TIMERS           (24)

// Choose an available IRQ number for the sleep timer to use to wake up the core
// Default: 2 (OC2)
#define SleepTim_IRQn		     ((IRQn_Type)2)
#define SleepTim_Priority    (0)

/*****************************************************************************/ 
/* Function implementation - global ('extern') and local ('static')          */ 
/*****************************************************************************/

/*******************************************************************************
 **
 ** @brief  Put the core in a low power state for a specified milliseconds
 **
 ** @note   Maximum sleep time is 85899 ms at SystemCoreClock = 50MHz
 **
 ******************************************************************************/
int SleepMs(uint32_t timerNum, uint32_t ms){
  uint32_t sys1k = SystemCoreClock / 1000; // VA10820 clock speed / 1000 (clocks per millisecond)
  if(timerNum > NUM_TIMERS) return -1; // invalid timer num
  if(ms > (0xFFFFFFFF / sys1k)) return -2; // ms delay too long
  VOR_SYSCONFIG->TIM_CLK_ENABLE |= (1 << timerNum);
  VOR_TIM->BANK[timerNum].CSD_CTRL = 0;
  VOR_TIM->BANK[timerNum].CTRL = TIM_PERIPHERAL_CTRL_AUTO_DISABLE_Msk | TIM_PERIPHERAL_CTRL_IRQ_ENB_Msk;
  VOR_TIM->BANK[timerNum].CNT_VALUE = sys1k * ms;
  VOR_TIM->BANK[timerNum].RST_VALUE = sys1k * ms;
  VOR_IRQSEL->TIM[timerNum] = SleepTim_IRQn; // For Torch Only
  NVIC_SetPriority(SleepTim_IRQn, SleepTim_Priority);
  NVIC_EnableIRQ(SleepTim_IRQn);
  VOR_TIM->BANK[timerNum].ENABLE = 1;
  while((VOR_TIM->BANK[timerNum].CTRL & TIM_PERIPHERAL_CTRL_ACTIVE_Msk) > 0){
    __WFI(); // halt core, wait for interrupt
  }
  NVIC_DisableIRQ(SleepTim_IRQn);
  return 0;
}

/*******************************************************************************
 **
 ** @brief  Sleep timer irq handler - index of this ISR must match SleepTim_IRQn
 **         Default: 2 (OC2)
 **
 ******************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif
void OC2_IRQHandler(void)
{
}
#ifdef __cplusplus
}
#endif

/*****************************************************************************/ 
/* End of file                                                               */ 
/*****************************************************************************/
