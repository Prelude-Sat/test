/***************************************************************************************
 * @file     sleep.h
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
 
#ifndef __UTIL_SLEEP_H
#define __UTIL_SLEEP_H

/*****************************************************************************/ 
/* Include files                                                             */ 
/*****************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include "va108xx.h"

/*****************************************************************************/ 
/* Global pre-processor symbols/macros ('#define')                           */ 
/*****************************************************************************/

/*****************************************************************************/
/* Global variable declarations ('extern', definition in C source)           */
/*****************************************************************************/

/*****************************************************************************/ 
/* Global function prototypes ('extern', definition in C source)             */ 
/*****************************************************************************/

extern int SleepMs(uint32_t timerNum, uint32_t ms);

/*****************************************************************************/ 
/* End of file                                                               */ 
/*****************************************************************************/
#endif /* __UTIL_SLEEP_H */
