#include "main.h"
#include "hw3.h"


volatile uint16_t SHIP_X_COORD = 16;
volatile uint16_t SHIP_Y_COORD = 200;
volatile uint16_t INVADER_X_COORD = 50;
volatile uint16_t INVADER_Y_COORD = 100;
volatile bool ALERT_CAR = false;
volatile bool ALERT_INVADER = true;
char STUDENT_NAME[] = "Eddie Estevez and Quinn Kleinschmidt";

uint8_t ft6x06_read_td_status(void);
uint16_t ft6x06_read_x(void);
uint16_t ft6x06_read_y(void);

typedef struct
{
    int top;
    int bottom;
    int left;
    int right;
} Rectangle;

//*****************************************************************************
// Determines if any part of the image would be off the screen if the image
// is moved in the specified direction.
//*****************************************************************************

int screen_height, screen_width; 			// to be used in order to accomodate for odd
																			// number of pixels for image dimensions

bool contact_edge(
    volatile PS2_DIR_t direction,			// direction in question
    volatile uint16_t x_coord, 				// X-coord of image
    volatile uint16_t y_coord, 				// Y-coord of image
    uint8_t image_height, 						// height of image
    uint8_t image_width								// width of image
)
{
		// the math gets weird if either the height or width are odd, so let's account for that
	screen_height = ROWS;
	screen_width = COLS;
	// if either numbers of odd, subtract 1 from the original values
		if(image_height%2==1) {
			screen_height--;
		}
		if(image_width%2==1) {
			screen_width--;
		}
		// So, where does it want to go?
		switch(direction)
		{
			// True -> Edge has been hit, can't move anymore
			// False -> Edge has not been hit, movement is possible
			case PS2_DIR_UP:		// Trying go up?
			// Let's look at the Y coordinate
			// check if at the very top edge of the screen
			if(y_coord == (image_height/2)) {
				// nope, you can't go more up
				return true;
			} else {
				// yeah, there's at least one pixel of room
			return false;
			}
			case PS2_DIR_DOWN:	// Trying go down?
			// Let's look at the Y coordinate
			// check if at the very bottom edge of the screen
			if(y_coord == (screen_height - image_height/2)) {
				// nope, you can't go more down
				return true;
			} else {
				// yeah, there's at least one pixel of room
			return false;
			}
			case PS2_DIR_LEFT:	// Trying to go left?
			// Let's look at the X coordinate
			// check if at the very left edge of the screen
			if(x_coord == (image_width/2)) {
				// nope, you can't go more left
				return true;
			} else {
				// yeah, there's at least one pixel of room
			return false;
			}
			case PS2_DIR_RIGHT:	// Trying to go right?
			// Let's look at the X coordinate
			// check if at the very right edge of the screen
			if(x_coord == (screen_width - image_width/2)) {
				// nope, you can't go more right
				return true;
			} else {
				// yeah, there's at least one pixel of room
			return false;
			}
			case PS2_DIR_CENTER:
			// You don't need to move anywhere if you're already at the center
			// Technically, yes, you can stay where you are, so TRUE
			return false;
			case PS2_DIR_INIT:
			// Not a direction, hopefully this value is not sent
			return false;
			default:
			// No valid input was given, something went wrong if it reaches
			// the default case
			return false;
		}
}

//*****************************************************************************
// Moves the image by one pixel in the provided direction.  The second and 
// third parameter should modify the current location of the image (pass by
// reference)
//*****************************************************************************
void move_image(
        volatile PS2_DIR_t direction,
        volatile uint16_t *x_coord, 
        volatile uint16_t *y_coord, 
        uint8_t image_height, 
        uint8_t image_width
)
{
		// What direction do you want to move?
		switch(direction)
		{
			case PS2_DIR_UP:
				// move up in the y-axis
				*y_coord = *y_coord - 1;
			break;
			case PS2_DIR_DOWN:
				// move down in the y-axis
				*y_coord = *y_coord + 1;
			break;
			case PS2_DIR_LEFT:
				// move left in the x-axis
				*x_coord = *x_coord - 1;
			break;
			case PS2_DIR_RIGHT:
				// move right in the x-axis
				*x_coord = *x_coord + 1;
			break;
			case PS2_DIR_CENTER:
				// don't do anything
			break;
			case PS2_DIR_INIT:
				// don't do anything
			break;
			default:
				// don't do anything
				break;
		}
		return;
}

//*****************************************************************************
// Determines is any portion of the two images are overlapping.  An image is
// considered to be overlapping if the two rectangles determined by the image
// height and widths are overlapping.  An overlap occurs even if the area that
// overlaps are portions of the images where the pixels do not display on the
// screen.
//*****************************************************************************

Rectangle ship;				// Rectangle to help with boundary check of ship
Rectangle invader;		// Rectangle to help with boundary check of invader

bool check_game_over(
        volatile uint16_t ship_x_coord, 
        volatile uint16_t ship_y_coord, 
        uint8_t ship_height, 
        uint8_t ship_width,
        volatile uint16_t invader_x_coord, 
        volatile uint16_t invader_y_coord, 
        uint8_t invader_height, 
        uint8_t invader_width
)
{
	// Let's make some rectangles to help us figure out the boundaries
	
	// First, the space ship
	ship.top = ship_y_coord - (ship_height/2);
	ship.bottom = ship_y_coord + (ship_height/2);
	ship.left = ship_x_coord - (ship_width/2);
	ship.right = ship_x_coord + (ship_width/2);
	
	// Second, the space invader
	invader.top = invader_y_coord - (invader_height/2);
	invader.bottom = invader_y_coord + (invader_height/2);
	invader.left = invader_x_coord - (invader_width/2);
	invader.right = invader_x_coord + (invader_width/2);
	
	// To check for an overlap, let's check conditions for when there is not an overlap
	// If the conditions for no overlap are not met, that means there is an overlap
	
	// Let's check the y axis first
	if (invader.top > ship.bottom || ship.top > invader.bottom)
		return false;
	// Now the x axis
	if (invader.left > ship.right || ship.left > invader.right)
		return false;
	// If neither statemetns were true, than there is an overlap
	return true;
}

//*****************************************************************************
// Initializes all of the peripherls used in HW3
//*****************************************************************************
void init_hardware(void)
{
  lcd_config_gpio();
  lcd_config_screen();
  lcd_clear_screen(LCD_COLOR_BLACK);
  ps2_initialize();
  
  // Update the Space Shipt 60 times per second.
  gp_timer_config_32(TIMER2_BASE,TIMER_TAMR_TAMR_PERIOD, 1000000, false, true);
  gp_timer_config_32(TIMER3_BASE,TIMER_TAMR_TAMR_PERIOD, 500000, false, true);
  gp_timer_config_32(TIMER4_BASE,TIMER_TAMR_TAMR_PERIOD, 50000, false, true);
}

uint8_t game_state;

//*****************************************************************************
// Main application for HW3
//*****************************************************************************
void hw3_main(void)
{
    bool game_over = false;
		bool game_pause = false;
	
		uint16_t x,y;
		uint8_t touch_event;
		i2c_status_t td_status;
		i2c_status_t x_status;
		i2c_status_t y_status;
	
    init_hardware();
		
		// Have game start at the Welcome Screen
		 game_state = 0;
	
		/* States for game_state
					Welcome Screen = 0x0;
					Game Screen = 0x1;
		*/
		
		while(true)
		{
		switch(game_state) {
			
			//*****************************************************************************
			// Welcome Screen
			//*****************************************************************************
			case 0:
				
			
			touch_event = ft6x06_read_td_status();
			
			if(touch_event == 1 | touch_event == 2) {
				x = ft6x06_read_x();
				y = ft6x06_read_y();
				game_state = 1;
				if(game_state) {
				lcd_clear_screen(LCD_COLOR_BLACK);
				break;
				};
			}
			gp_timer_wait(TIMER0_BASE, 5000000);
			
				lcd_draw_image(
							120,            									// X Center Point
							welcomeScreenTitleWidthPixels,   // Image Horizontal Width
							80,            									// Y Center Point
							welcomeScreenTitleHeightPixels, // Image Vertical Height
							welcomeScreenTitleBitmaps,      // Image
							LCD_COLOR_WHITE,         				// Foreground Color
							LCD_COLOR_BLACK        					// Background Color  
				);
			
							lcd_draw_image(
							120,            									// X Center Point
							touchAnywhereWidthPixels,   // Image Horizontal Width
							240,            									// Y Center Point
							touchAnywhereHeightPixels, // Image Vertical Height
							touchAnywhereBitmaps,      // Image
							LCD_COLOR_WHITE,         				// Foreground Color
							LCD_COLOR_BLACK        					// Background Color  
				);
			
			
			break;
			//*****************************************************************************
			
			
			
			//*****************************************************************************
			// GameScreen
			//*****************************************************************************
			case 1: 
			// Render out Target
				lcd_draw_image(
							98,            				// X Center Point
							target1WidthPixels,   // Image Horizontal Width
							25,            				// Y Center Point
							target1HeightPixels,  // Image Vertical Height
							target1Bitmaps,       // Image
							LCD_COLOR_RED,         // Foreground Color
							LCD_COLOR_WHITE        	// Background Color  
					);
							lcd_draw_image(
							218,            			// X Center Point
							target2WidthPixels,   // Image Horizontal Width
							25,           				// Y Center Point
							target2HeightPixels,  // Image Vertical Height
							target2Bitmaps,       // Image
							LCD_COLOR_RED,        // Foreground Color
							LCD_COLOR_WHITE       // Background Color  
							);
			
			// Render out high score text
							lcd_draw_image(
							59,            									// X Center Point
							highScoreWidthPixels,   // Image Horizontal Width
							300,            									// Y Center Point
							highScoreHeightPixels, // Image Vertical Height
							highScoreBitmaps,      // Image
							LCD_COLOR_WHITE,         				// Foreground Color
							LCD_COLOR_BLACK        					// Background Color  
				);
								
					while(!game_over)
							{	
								
							if(ALERT_CAR)
							{
								ALERT_CAR = false;
								
								lcd_draw_image(
									SHIP_X_COORD,            // X Center Point
									car1WidthPixels,   // Image Horizontal Width
									SHIP_Y_COORD,            // Y Center Point
									car1HeightPixels,  // Image Vertical Height
									car1Bitmaps,       // Image
									LCD_COLOR_BLACK,          // Foreground Color
									LCD_COLOR_ORANGE          // Background Color
								);


							game_over = check_game_over(
												SHIP_X_COORD,
												SHIP_Y_COORD,
												space_shipHeightPixels,
												space_shipWidthPixels,
												INVADER_X_COORD,
												INVADER_Y_COORD,
												invaderHeightPixels,
												invaderWidthPixels
											);
							}

							if(ALERT_INVADER)
							{
								ALERT_INVADER = false;
								
							lcd_draw_image(
								INVADER_X_COORD,          // X Center Point
								invaderWidthPixels,       // Image Horizontal Width
								INVADER_Y_COORD,          // Y Center Point
								invaderHeightPixels,      // Image Vertical Height
								invaderBitmaps,           // Image
								LCD_COLOR_RED,            // Foreground Color
								LCD_COLOR_BLACK           // Background Color
								);


							game_over = check_game_over(
													SHIP_X_COORD,
													SHIP_Y_COORD,
													space_shipHeightPixels,
													space_shipWidthPixels,
													INVADER_X_COORD,
													INVADER_Y_COORD,
													invaderHeightPixels,
													invaderWidthPixels
												);
										}
							}
							game_state = 2;
							lcd_clear_screen(LCD_COLOR_BLACK);
							break;
			case 2:
							lcd_draw_image(
							120,            									// X Center Point
							gameOverWidthPixels,   // Image Horizontal Width
							160,            									// Y Center Point
							gameOverHeightPixels, // Image Vertical Height
							gameOverBitmaps,      // Image
							LCD_COLOR_WHITE,         				// Foreground Color
							LCD_COLOR_BLACK        					// Background Color  
				);
						
			break;
			//*****************************************************************************
			// Default Case
			//*****************************************************************************
			default: 						game_state = 0;
													break;
		}
	}

}
