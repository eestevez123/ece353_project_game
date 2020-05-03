// Copyright (c) 2014, Joe Krachey
// All rights reserved.
//
// Redistribution and use in binary form, with or without modification, 
// are permitted provided that the following conditions are met:
//
// 1. Redistributions in binary form must reproduce the above copyright 
//    notice, this list of conditions and the following disclaimer in 
//    the documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR 
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef __GPTIMER_H__
#define __GPTIMER_H__

#include <stdint.h>
#include <stdbool.h>
#include "TM4C123GH6PM.h"
#include "driver_defines.h"


typedef struct {
  uint32_t  base;
  uint32_t  mode;
  // Sub Timer A
  bool      enableA;
  uint32_t  countA;
  uint32_t  countModeA;
  uint8_t   priorityA;
  bool      intEnA;
  IRQn_Type  irqNumA;
  
  // Sub Timer B
  bool      enableB;
  uint32_t  countB;
  uint32_t  countModeB;
  uint8_t   priorityB;
  bool      intEnB;
  IRQn_Type  irqNumB;
} GPTIMER_CONFIG;



//*****************************************************************************
 //*****************************************************************************
void timer0Init(uint32_t clkCycles, bool periodic, bool enInts, uint8_t priority);
  
 //*****************************************************************************
 //*****************************************************************************
 int32_t gp_timer_PWM(
  uint32_t base, 
  uint16_t totalCycles_A,  
  uint16_t lowCycles_A,
  uint16_t totalCycles_B,  
  uint16_t lowCycles_B
);

/*****************************************************************************
 * Configures a timer to be in 32-bit mode and counts up
 * Maximum time to count is 85 seconds assuming 50MHz clock.
*****************************************************************************/
void gp_timer_start_count( uint32_t base);


/*****************************************************************************
 * Configures a timer to be in 32-bit mode and counts up
 * Maximum time to count is 85 seconds assuming 50MHz clock.
*****************************************************************************/
uint32_t gp_timer_stop_count( uint32_t base);

/*****************************************************************************
 * Enable a gp_timer
*****************************************************************************/
void gp_timer_enable(uint32_t base, bool timerA, bool timerB);


/*****************************************************************************
 * Disable a gp_timer
*****************************************************************************/
void gp_timer_disable(uint32_t base, bool timerA, bool timerB);

/*****************************************************************************
 * Returns the current count for a 32-bit timer
*****************************************************************************/
int32_t gp_timer_get_count(uint32_t base);

/*****************************************************************************
 * Configure a 32 bit timer to be either 16 or 32-bit mode.
*****************************************************************************/
int32_t gp_timer_config(GPTIMER_CONFIG *config);

/*****************************************************************************
 * Set the period and start a 32-bit timer
*****************************************************************************/
void gp_timer_set_32_bit_period(uint32_t base, uint32_t timerA);

#endif

