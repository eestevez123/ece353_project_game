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

#include "adc.h"
#include "driver_defines.h"

//*****************************************************************************
// Verifies that the base address is a valid GPIO base address
//*****************************************************************************
static bool verify_base_addr(uint32_t baseAddr)
{
   switch( baseAddr )
   {
     case ADC0_BASE:
     case ADC1_BASE:
     {
       return true;
     }
     default:
     {
       return false;
     }
   }
}

/****************************************************************************
 * Return the GPIO IRQ Number
 ****************************************************************************/
IRQn_Type adc_get_irq_num(uint32_t base, uint8_t ss_num)
{
   switch(base)
   {
     case ADC0_BASE:
     {
       return ADC0SS0_IRQn + ss_num;
     }
     case ADC1_BASE:
     {
       return ADC1SS0_IRQn + ss_num;
     }
     default:
     {
       return 0;
     }
   }
}


/****************************************************************************
 * Return the RCGC Mask
 ****************************************************************************/
uint32_t adc_get_rcgc_mask(uint32_t base)
{
   switch(base)
   {
     case ADC0_BASE:
     {
       return SYSCTL_RCGCADC_R0;
     }
     case ADC1_BASE:
     {
       return SYSCTL_RCGCADC_R1;
     }
     default:
     {
       return 0;
     }
   }
}


/****************************************************************************
 * Return the RCGC Mask
 ****************************************************************************/
uint32_t adc_get_pr_mask(uint32_t base)
{
   switch(base)
   {
     case ADC0_BASE:
     {
       return SYSCTL_PRADC_R0;
     }
     case ADC1_BASE:
     {
       return SYSCTL_PRADC_R1;
     }
     default:
     {
       return 0;
     }
   }
}

/******************************************************************************
 * Initializes ADC to use Sample Sequencer #3, triggered by software, no IRQs
 *****************************************************************************/
bool initialize_adc(  uint32_t adc_base )
{
  ADC0_Type  *myADC;  
  uint32_t rcgc_adc_mask;
  uint32_t pr_mask;
	
  if( adc_base == 0)
  {
    return false;
  }

  // Set the rcgc_adc_mask and pr_mask  
  switch (adc_base) 
  {
    case ADC0_BASE :
    {
      // Turn on the clock for ADC0
      rcgc_adc_mask = SYSCTL_RCGCADC_R0;
      
      // Set pr_mask 
      pr_mask =  SYSCTL_PRADC_R0;
      break;
    }
    case ADC1_BASE :
    {
      // Turn on the clock for ADC1
      rcgc_adc_mask = SYSCTL_RCGCADC_R1;
      
      // Set pr_mask 
      pr_mask =  SYSCTL_PRADC_R1;
      break;
    }
    
    default:
      return false;
  }
  
  // Turn on the ADC Clock
  SYSCTL->RCGCADC |= rcgc_adc_mask;
  
  // Wait for ADCx to become ready
  while( (pr_mask & SYSCTL->PRADC) != pr_mask){}
  
  myADC = (ADC0_Type *)adc_base;
  
  // disable the sample sequencer by writing a 0 to the corresponding ASENn bit in the ADCACTSS register 
  myADC->ACTSS &= ~ADC_ACTSS_ASEN3;

  // Sequencer 3 is the lowest priority
  myADC->SSPRI = ADC_SSPRI_SS3_4TH | ADC_SSPRI_SS2_3RD | ADC_SSPRI_SS1_2ND | ADC_SSPRI_SS0_1ST;

  myADC->EMUX &= ~ADC_EMUX_EM3_ALWAYS;

  myADC->SSMUX3 &=  ~ADC_SSMUX3_MUX0_M;

  myADC->SSCTL3 = ADC_SSCTL3_IE0 | ADC_SSCTL3_END0;

  // Clear Averaging Bits
  //myADC->SAC &= ~ADC_SAC_AVG_M  ;
  
  // Average 64 samples
  //myADC->SAC |= ADC_SAC_AVG_64X;
  
  return true;
}

/******************************************************************************
 * Reads SSMUX3 for the given ADC.  Busy waits until completion
 *****************************************************************************/
uint32_t get_adc_value( uint32_t adc_base, uint8_t channel)
{
  ADC0_Type  *myADC;
  uint32_t result;
  
  if( adc_base == 0)
  {
    return false;
  }
  
  myADC = (ADC0_Type *)adc_base;
  
  myADC->SSMUX3 = channel;          // Set the Channel
  
  myADC->ACTSS |= ADC_ACTSS_ASEN3;  // Enable SS3
  
  myADC->PSSI =   ADC_PSSI_SS3;     // Start SS3
  
  while( (myADC->RIS & ADC_RIS_INR3)  == 0)
  {
    // wait
  }
  
  result = myADC->SSFIFO3 & 0xFFF;    // Read 12-bit data
  
  myADC->ISC  = ADC_ISC_IN3;          // Ack the conversion
  
  return result;
}

