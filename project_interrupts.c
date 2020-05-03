// Copyright (c) 2015-19, Joe Krachey
// All rights reserved.
//
// Redistribution and use in source or binary form, with or without modification, 
// are permitted provided that the following conditions are met:
//
// 1. Redistributions in source form must reproduce the above copyright 
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

#include "main.h"
#include "project_interrupts.h"

static volatile uint16_t PS2_X_DATA;
static volatile uint16_t PS2_Y_DATA;
static volatile PS2_DIR_t PS2_DIR = PS2_DIR_CENTER;
static volatile PS2_DIR_t SHIP_DIR = PS2_DIR_CENTER;
static volatile uint16_t move_count = 0;


PS2_DIR_t ps2_get_direction(void)
{
  if(PS2_X_DATA > ADC_HIGH_THRESHOLD){
		// Joystick is pointed to the left
		return PS2_DIR = PS2_DIR_LEFT;
	}else if(PS2_X_DATA < ADC_LOW_THRESHOLD){
		// Joystick is pointed to the right
		return PS2_DIR = PS2_DIR_RIGHT;
	}else if(PS2_Y_DATA > ADC_HIGH_THRESHOLD){
		// Joystick is pointed up
		return PS2_DIR = PS2_DIR_UP;
	}else if(PS2_Y_DATA < ADC_LOW_THRESHOLD){
		// Joystick is pointed down
		return PS2_DIR = PS2_DIR_DOWN;
	}else{
		// Joystick is not being moved
		return PS2_DIR = PS2_DIR_CENTER;
	}
}

//*****************************************************************************
// TIMER2 ISR is used to determine when to move the Invader
//*****************************************************************************
void TIMER2A_Handler(void)
{	
	// let's check if it's hitting an edge
	if(!contact_edge(PS2_DIR, INVADER_X_COORD, INVADER_Y_COORD, invaderHeightPixels, invaderWidthPixels)){
		// no edge is being hit, it's safe to move in the direction
    move_image(PS2_DIR, &INVADER_X_COORD, &INVADER_Y_COORD, invaderHeightPixels, invaderWidthPixels);
		ALERT_INVADER = true;
	}
	// Clear the interrupt
	TIMER2->ICR |= TIMER_ICR_TATOCINT;
}

//*****************************************************************************
// TIMER3 ISR is used to determine when to move the spaceship
//*****************************************************************************
bool touch_edge;

void TIMER3A_Handler(void)
{	
	// Let's check if the ship has touched edge from where it is
	touch_edge = contact_edge(PS2_DIR_RIGHT, SHIP_X_COORD, SHIP_Y_COORD, car1HeightPixels, car1WidthPixels);
	
	// Let's check where move_count is at
		// can it move? Let's move it if we can
		if(!touch_edge) {
			move_image(PS2_DIR_RIGHT, &SHIP_X_COORD, &SHIP_Y_COORD, car1HeightPixels, car1WidthPixels);
			ALERT_CAR = true;
		} 
	// if we can't move since move_count is 0 or an edge was hit, let's give it a new direction and move count
	else{
		SHIP_X_COORD = car1WidthPixels/2;
	}
	// Clear the interrupt
	TIMER3->ICR |= TIMER_ICR_TATOCINT; 
}

//*****************************************************************************
// TIMER4 ISR is used to trigger the ADC
//*****************************************************************************
void TIMER4A_Handler(void)
{	
	// Clear the interrupt
	TIMER4->ICR |= TIMER_ICR_TATOCINT; 
	
	// Trigger ADC Sample Sequencer 2 conversion
	ADC0 ->PSSI |= ADC_PSSI_SS2;
}

//*****************************************************************************
// ADC0 SS2 ISR
//*****************************************************************************
void ADC0SS2_Handler(void)
{
	// get the data for the PS2 Joystick
	PS2_X_DATA = ADC0 -> SSFIFO2;
	PS2_Y_DATA = ADC0 -> SSFIFO2;
	
	// get the direction with this data
	PS2_DIR = ps2_get_direction();
	
	  // Clear the interrupt
  ADC0->ISC |= ADC_ISC_IN2;
}


