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
#include "hw3.h"
#include "project_interrupts.h"

static volatile uint16_t PS2_X_DATA;
static volatile uint16_t PS2_Y_DATA;
static volatile PS2_DIR_t PS2_DIR = PS2_DIR_CENTER;
static volatile PS2_DIR_t VIRUS1_DIR = PS2_DIR_RIGHT;
static volatile PS2_DIR_t VIRUS2_DIR = PS2_DIR_LEFT;
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
// TIMER0 ISR is used to move car2
//*****************************************************************************
bool touch_edge21;
bool touch_edge22;

void TIMER0A_Handler(void)
{	
	touch_edge21 = contact_edge(PS2_DIR_RIGHT, CAR21_X_COORD , CAR21_Y_COORD, car2HeightPixels, car2WidthPixels);
	touch_edge22 = contact_edge(PS2_DIR_RIGHT, CAR22_X_COORD , CAR22_Y_COORD, car2HeightPixels, car2WidthPixels);
	
	// Let's check where move_count is at
		// can it move? Let's move it if we can
		if(!touch_edge21) {
			CAR21_X_COORD++;
			ALERT_TRUCK1 = true;
		} 
		else{
			ERASE_TRUCK = true;
			CAR21_X_COORD = car2WidthPixels/2;
	}
		if(!touch_edge22) {
			CAR22_X_COORD++;
			ALERT_TRUCK2 = true;
		} else{
			ERASE_TRUCK = true;
		CAR22_X_COORD = car2WidthPixels/2;
	}
	// Clear the interrupt
	TIMER0->ICR |= TIMER_ICR_TATOCINT; 
}
//*****************************************************************************
// TIMER1 is used to blink a LED
//*****************************************************************************
int LED = 0;
void TIMER1A_Handler(void)
{	if(LED == 0){
lp_io_set_pin(RED_PIN);
	LED = 1;
}else{
	lp_io_clear_pin(RED_PIN);
	LED = 0;
}
	
	// Clear the interrupt
	TIMER1->ICR |= TIMER_ICR_TATOCINT;
}
//*****************************************************************************
// TIMER2 ISR is used to determine when to move the Invader
//*****************************************************************************
bool target_boundary;
bool user_boundary;

void TIMER2A_Handler(void)
{
	 target_boundary = contact_boundary(PS2_DIR,120,25,50,240,PLAYER_X_COORD,PLAYER_Y_COORD,invaderHeightPixels,invaderWidthPixels);
	 user_boundary = 	contact_boundary(PS2_DIR ,75,300,30,149,PLAYER_X_COORD,PLAYER_Y_COORD,invaderHeightPixels,invaderWidthPixels);
	
	// let's check if it's hitting an edge
	if((!contact_edge(PS2_DIR, PLAYER_X_COORD, PLAYER_Y_COORD, invaderHeightPixels, invaderWidthPixels))&(!target_boundary)&(!user_boundary)){
		// no edge is being hit, it's safe to move in the direction
    move_image(PS2_DIR, &PLAYER_X_COORD, &PLAYER_Y_COORD, invaderHeightPixels, invaderWidthPixels);
		ALERT_PLAYER = true;
	}
	
	if(target_boundary) {
	lcd_draw_rectangle_centered(PLAYER_X_COORD, invaderWidthPixels, PLAYER_Y_COORD, invaderHeightPixels, LCD_COLOR_BLACK);
	ALERT_RAISE_SCORE = true;
	PLAYER_X_COORD = 200;
	PLAYER_Y_COORD = 300;
	
	}
	// Clear the interrupt
	TIMER2->ICR |= TIMER_ICR_TATOCINT;
}

//*****************************************************************************
// TIMER3 ISR is used to determine when to move the spaceship
//*****************************************************************************
bool touch_edge11;
bool touch_edge12;

void TIMER3A_Handler(void)
{	
	// Let's check if the ship has touched edge from where it is
	touch_edge11 = contact_edge(PS2_DIR_RIGHT, CAR11_X_COORD -2 , CAR11_Y_COORD, car1HeightPixels, car1WidthPixels);
	touch_edge12 = contact_edge(PS2_DIR_RIGHT, CAR12_X_COORD -2, CAR12_Y_COORD, car1HeightPixels, car1WidthPixels);
	
	// Let's check where move_count is at
		// can it move? Let's move it if we can
		if(!touch_edge11) {
			CAR11_X_COORD++;
			ALERT_CAR1 = true;
		} 
		else{
			ERASE_CAR = true;
		CAR11_X_COORD = car1WidthPixels/2;
	}
		if(!touch_edge12) {
			CAR12_X_COORD++;
			ALERT_CAR2 = true;
		} else{
			ERASE_CAR = true;
		CAR12_X_COORD = car1WidthPixels/2;
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
// TIMER3 ISR is used to determine when to move the spaceship
//*****************************************************************************
bool touch_edge31;
bool touch_edge32;

void TIMER5A_Handler(void)
{	
	// Let's check if the ship has touched edge from where it is
	touch_edge31 = contact_edge(VIRUS1_DIR, VIRUS1_X_COORD -1, VIRUS1_Y_COORD, invaderHeightPixels, invaderWidthPixels);
	touch_edge32 = contact_edge(VIRUS2_DIR, VIRUS2_X_COORD -1, VIRUS2_Y_COORD, invaderHeightPixels, invaderWidthPixels);
	
	// Let's check where move_count is at
		// can it move? Let's move it if we can
		if(!touch_edge31) {
			if(VIRUS1_DIR == PS2_DIR_RIGHT){
			VIRUS1_X_COORD++;
			}else{
				VIRUS1_X_COORD--;
			}
			ALERT_VIRUS1 = true;
		} 
		else{
		if(VIRUS1_DIR == PS2_DIR_RIGHT){
			VIRUS1_DIR = PS2_DIR_LEFT;
			}else{
				VIRUS1_DIR = PS2_DIR_RIGHT;
			}
	}
		if(!touch_edge32) {
			if(VIRUS2_DIR == PS2_DIR_RIGHT){
			VIRUS2_X_COORD++;
			}else{
				VIRUS2_X_COORD--;
			}
			ALERT_VIRUS2 = true;
		} 
		else{
		if(VIRUS2_DIR == PS2_DIR_RIGHT){
			VIRUS2_DIR = PS2_DIR_LEFT;
			}else{
				VIRUS2_DIR = PS2_DIR_RIGHT;
			}
	}
	// Clear the interrupt
	TIMER5->ICR |= TIMER_ICR_TATOCINT; 
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




