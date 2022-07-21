/***************************************************************************************
 * @file     reb_timer.h
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
#ifndef __REB_TIMER_H
#define __REB_TIMER_H

/**
	\fn          int32_t VOR_TIM0_MSecInitialize(uint32_t time_msec)
  \brief       Initialize milli seconds timer with specified time(in msec).  
  \param[in]   time_msec  Specifies time in milli seconds  
  \return      \ref execution_status
*/ 
int32_t VOR_TIM0_MSecInitialize(uint32_t time_msec); 

/**
	\fn          int32_t VOR_TIM1_SecInitialize(uint32_t time_sec)
  \brief       Initialize seconds timer with specified time(in sec).  
  \param[in]   time_sec  Specifies time in seconds  
  \return      \ref execution_status
*/ 
int32_t VOR_TIM1_SecInitialize(uint32_t time_sec); 

/**
	\fn       bool VOR_TIM0_MSecExpired(void)
  \brief    Returns true when milli second timer expires.  
  \return   return true when milli second timer expires otherwise return false 
*/ 
bool VOR_TIM0_MSecExpired(void); 

/**
	\fn       bool VOR_TIM0_MSecClear(void)
  \brief    Clear milli-seconds timer flag, which will be set again when milli second timer expires.  
*/ 
void VOR_TIM0_MSecClear(void); 

/**
	\fn       bool VOR_TIM1_SecExpired(void)
  \brief    Returns true when seconds timer expires.  
  \return   return true when seconds timer expires otherwise return false 
*/ 
bool VOR_TIM1_SecExpired(void); 

/**
	\fn       bool VOR_TIM1_SecClear(void)
  \brief    Clear seconds timer falg, which will be set again when seconds timer expires.  
*/ 
void VOR_TIM1_SecClear(void); 

#endif /* __REB_TIMER_H */ 



