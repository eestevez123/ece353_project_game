#ifndef __HW3_STAFF_H__
#define __HW3_STAFF_H__

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "TM4C123.h"
#include "gpio_port.h"
#include "ps2.h"
#include "lcd.h"
#include "timers.h"
#include "project_images.h"
#include "hw3.h"

//*****************************************************************************
// Generates a random number
// https://en.wikipedia.org/wiki/Linear-feedback_shift_register  -- DO NOT MODIFY
//*****************************************************************************
uint16_t generate_random_number(void);

//*****************************************************************************
// Generates the the new direction and number of pixels  -- DO NOT MODIFY
//*****************************************************************************
PS2_DIR_t get_new_direction(PS2_DIR_t curr_direction);

//*****************************************************************************
// Generates the the new direction and number of pixels  -- DO NOT MODIFY
//*****************************************************************************
uint16_t get_new_move_count(void);

//*****************************************************************************
// Validates HW3 Hardware -- DO NOT MODIFY
//*****************************************************************************
bool hw3_hardware_validate(void);

#endif
