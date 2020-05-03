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

#include "gptimer.h"

/*****************************************************************************
 * Configures Timer 0
 * Parameters
 *          clkCycles
 *                      Number of clock cycles for Timer 0
 *          periodic
 *                      true = periodic, false = one-shot
 *           enInts
 *                      true = enable interrupts, false = disable interrupts
 *****************************************************************************/
void timer0Init(uint32_t clkCycles, bool periodic, bool enInts, uint8_t priority)
{
  // Turn on the clock to timer0
  SYSCTL->RCGCTIMER   |=   SYSCTL_RCGCTIMER_R0;        
  while ((SYSCTL->PRTIMER & SYSCTL_PRTIMER_R0) == 0);
  
  // Disable the timer while we are configuring it.
  TIMER0->CTL      &=   ~TIMER_CTL_TAEN;
   
  // Configure for 32-bit Mode
  TIMER0->CFG      =    TIMER_CFG_32_BIT_TIMER;    
  
  // Check to see if we are in periodic or one-shot mode
  if ( periodic)
    TIMER0->TAMR     =   TIMER_TAMR_TAMR_PERIOD;
  else
    TIMER0->TAMR     =   TIMER_TAMR_TAMR_1_SHOT;       
  
  
  // Set the number of clock cycles
  TIMER0->TAILR    =   clkCycles;                     
  
  // Clear the status flag so the timer is ready the next time it is run. 
  TIMER0->ICR|= TIMER_ICR_TATOCINT;
  
  
  // Check to see if we will enable interrupts
  if ( enInts )
  {
    TIMER0->IMR    |= TIMER_IMR_TATOIM;
    NVIC_SetPriority(TIMER0A_IRQn ,priority);
    NVIC_EnableIRQ(TIMER0A_IRQn);
  }
  
  // Turn the timer on
  TIMER0->CTL      |=   TIMER_CTL_TAEN ;  
}

/*****************************************************************************
 * 
 *****************************************************************************/
int32_t gp_timer_enable_clock( uint32_t base)
{
  switch (base)
   {
     case TIMER0_BASE: 
      {
         SYSCTL->RCGCTIMER   |=   SYSCTL_RCGCTIMER_R0;
        while ((SYSCTL->PRTIMER & SYSCTL_PRTIMER_R0) == 0);
        return 0;
      }
     case TIMER1_BASE: 
      {
         SYSCTL->RCGCTIMER   |=   SYSCTL_RCGCTIMER_R1;
        while ((SYSCTL->PRTIMER & SYSCTL_PRTIMER_R1) == 0);
        return 0;
      }
     case TIMER2_BASE: 
      {
         SYSCTL->RCGCTIMER   |=   SYSCTL_RCGCTIMER_R2;
        while ((SYSCTL->PRTIMER & SYSCTL_PRTIMER_R2) == 0);
        return 0;
      }
     case TIMER3_BASE: 
      {
         SYSCTL->RCGCTIMER   |=   SYSCTL_RCGCTIMER_R3;
        while ((SYSCTL->PRTIMER & SYSCTL_PRTIMER_R3) == 0);
        return 0;
      }
	   case TIMER4_BASE: 
      {
         SYSCTL->RCGCTIMER   |=   SYSCTL_RCGCTIMER_R4;
        while ((SYSCTL->PRTIMER & SYSCTL_PRTIMER_R4) == 0);
        return 0;
      }
	   case TIMER5_BASE: 
      {
         SYSCTL->RCGCTIMER   |=   SYSCTL_RCGCTIMER_R5;
        while ((SYSCTL->PRTIMER & SYSCTL_PRTIMER_R5) == 0);
        return 0;
      }
      default:
      {
        return -1;
      }

    }
}


/*****************************************************************************
 * Configures Timer 0
 *****************************************************************************/
int32_t gp_timer_PWM(
  uint32_t base, 
  uint16_t totalCycles_A,  
  uint16_t lowCycles_A,
  uint16_t totalCycles_B,  
  uint16_t lowCycles_B
)
{
  
   TIMER0_Type *myTimer;
  
   gp_timer_enable_clock(base);
   

    myTimer = (TIMER0_Type *)base;
   
    // Disable the timer while we are configuring it.
    myTimer->CTL      &=   ~(TIMER_CTL_TAEN | TIMER_CTL_TBEN);
   
   // Put the timer in 16 bit mode
   myTimer->CFG       = TIMER_CFG_16_BIT;
   
   // Both Timer A and Timer B into PWM Mode
   myTimer->TAMR =  TIMER_TAMR_TAAMS | TIMER_TAMR_TAMR_PERIOD;
   myTimer->TBMR =  TIMER_TBMR_TBAMS | TIMER_TBMR_TBMR_PERIOD;
   
   // Set the Duty Cycle
   myTimer->TAILR = totalCycles_A;
   myTimer->TAMATCHR = lowCycles_A;
    
   myTimer->TBILR = totalCycles_B;
   myTimer->TBMATCHR = lowCycles_B;
   
    // Turn the timer on
    myTimer->CTL      |=   TIMER_CTL_TAEN | TIMER_CTL_TBEN;
  
    return 0;

}


/*****************************************************************************
 * Configures a timer to be in 32-bit mode and counts up
 * Maximum time to count is 85 seconds assuming 50MHz clock.
*****************************************************************************/
void gp_timer_start_count( uint32_t base)
{
  
  TIMER0_Type *myTimer;

  gp_timer_enable_clock(base);

  myTimer = (TIMER0_Type *)base;

  // Disable the timer while we are configuring it.
  myTimer->CTL      &=   ~(TIMER_CTL_TAEN | TIMER_CTL_TBEN);

  // Put the timer in 32 bit mode
  myTimer->CFG       = TIMER_CFG_32_BIT_TIMER;

  // One Shot, count up
  myTimer->TAMR     =   TIMER_TAMR_TAMR_1_SHOT | TIMER_TAMR_TACDIR;
  
  // Set the max upper bounds
  myTimer->TAILR = 0xFFFFFFFF;

  // Clear the status flag so the timer is ready the next time it is run. 
  myTimer->ICR|= TIMER_ICR_TATOCINT;

  // Turn the timer on
  myTimer->CTL      |=   TIMER_CTL_TAEN | TIMER_CTL_TBEN;

}


/*****************************************************************************
 * Configures a timer to be in 32-bit mode and counts up
 * Maximum time to count is 85 seconds assuming 50MHz clock.
*****************************************************************************/
uint32_t gp_timer_stop_count( uint32_t base)
{
  
  TIMER0_Type *myTimer;


  myTimer = (TIMER0_Type *)base;

  // Disable the timer while we are configuring it.
  myTimer->CTL      &=   ~(TIMER_CTL_TAEN | TIMER_CTL_TBEN);

  // Return the count
  return myTimer->TAR;

}

/*****************************************************************************
 *  
*****************************************************************************/
void gp_timer_configA(GPTIMER_CONFIG *config)
{
   TIMER0_Type *myTimer = (TIMER0_Type *)config->base;
  
   myTimer->TAMR     =   config->countModeA;   
  
  // Set the number of clock cycles
  myTimer->TAILR    =   config->countA;
  
  // Clear the status flag so the timer is ready the next time it is run. 
  myTimer->ICR|= TIMER_ICR_TATOCINT;
  
  
  // Check to see if we will enable interrupts
  if ( config->intEnA )
  {
    myTimer->IMR    |= TIMER_IMR_TATOIM;
    NVIC_SetPriority(config->irqNumA ,config->priorityA);
    NVIC_EnableIRQ(config->irqNumA);
  }
  // Turn the timer on
  myTimer->CTL      |=   TIMER_CTL_TAEN ;
  
  // Clear the status flag so the timer is ready the next time it is run. 
  myTimer->ICR|= TIMER_ICR_TATOCINT;
  
}


/*****************************************************************************
 *  
*****************************************************************************/
void gp_timer_configB(GPTIMER_CONFIG *config)
{
   TIMER0_Type *myTimer = (TIMER0_Type *)config->base;
  
   myTimer->TBMR     =   config->countModeB;
  
  // Set the number of clock cycles
  myTimer->TBILR    =   config->countB;
  
  // Clear the status flag so the timer is ready the next time it is run. 
  myTimer->ICR|= TIMER_ICR_TBTOCINT;
  
  
  // Check to see if we will enable interrupts
  if ( config->intEnB )
  {
    myTimer->IMR    |= TIMER_IMR_TBTOIM;
    NVIC_SetPriority(config->irqNumB ,config->priorityB);
    NVIC_EnableIRQ(config->irqNumB);
  }
  // Turn the timer on
  myTimer->CTL      |=   TIMER_CTL_TBEN ;
}


/*****************************************************************************
 * Configure a 32 bit timer to be either 16 or 32-bit mode.
*****************************************************************************/
int32_t gp_timer_config(GPTIMER_CONFIG *config)
{
   TIMER0_Type *myTimer;
  
  if( config == 0)
  {
    return -1;
  }
  
  myTimer = (TIMER0_Type *)config->base;
  
  gp_timer_enable_clock(config->base);
  
  // Disable the timer while we are configuring it.
  myTimer->CTL      &=   ~(TIMER_CTL_TAEN | TIMER_CTL_TBEN);
  
  
  // Check for configuring the timer as 16 or 32-bit mode
  if( config->mode == TIMER_CFG_16_BIT)
  {    
    myTimer->CFG  = TIMER_CFG_16_BIT;
    
    // Timer B is only configured in 16-bit mode
    if( config->enableB)
    {
      gp_timer_configB(config);
    }
  }
  else
  {
    myTimer->CFG  = TIMER_CFG_32_BIT_TIMER;
  }

  // Check to see if we are turning on timerA
  if( config->enableA)
  {
    gp_timer_configA(config);
  }
  
  return 0;
}

/*****************************************************************************
 * Enable a gp_timer
*****************************************************************************/
void gp_timer_enable(uint32_t base, bool timerA, bool timerB)
{
   TIMER0_Type *myTimer;
   uint32_t mask = 0;
  
   if( timerA)
   {
     mask |= TIMER_CTL_TAEN;
   }
   if( timerB)
   {
     mask |= TIMER_CTL_TBEN;
   }
   
   myTimer = (TIMER0_Type *)base;
   myTimer->CTL      |=   mask;
}

/*****************************************************************************
 * Disable a gp_timer
*****************************************************************************/
void gp_timer_disable(uint32_t base, bool timerA, bool timerB)
{
   TIMER0_Type *myTimer;
   uint32_t mask = 0;
  
   if( timerA)
   {
     mask |= TIMER_CTL_TAEN;
   }
   if( timerB)
   {
     mask |= TIMER_CTL_TBEN;
   }
   
   myTimer = (TIMER0_Type *)base;
   myTimer->CTL      &=   ~mask;
}

/*****************************************************************************
 * Get current count of the timer.
*****************************************************************************/
int32_t gp_timer_get_count(uint32_t base)
{
   TIMER0_Type *myTimer;
   myTimer = (TIMER0_Type *)base;
   return myTimer->TAR;
}

/*****************************************************************************
 * Set the period and start a 32-bit timer
*****************************************************************************/
void gp_timer_set_32_bit_period(uint32_t base, uint32_t timerA)
{
  TIMER0_Type *myTimer;
  myTimer = (TIMER0_Type *)base;
  
  gp_timer_disable(base, true, false);
  myTimer->TAILR = timerA;
  myTimer->TAR = timerA;
  gp_timer_enable(base, true, false);
}

