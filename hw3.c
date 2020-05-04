#include "main.h"
#include "hw3.h"


volatile uint16_t CAR11_X_COORD = 16;
volatile uint16_t CAR11_Y_COORD = 240;
volatile uint16_t CAR12_X_COORD = 116;
volatile uint16_t CAR12_Y_COORD = 240;
volatile uint16_t CAR21_X_COORD = 26;
volatile uint16_t CAR21_Y_COORD = 100;
volatile uint16_t CAR22_X_COORD = 126;
volatile uint16_t CAR22_Y_COORD = 100;
volatile uint16_t VIRUS1_X_COORD = 14;
volatile uint16_t VIRUS1_Y_COORD = 140;
volatile uint16_t VIRUS2_X_COORD = 226;
volatile uint16_t VIRUS2_Y_COORD = 190;
volatile uint16_t PLAYER_X_COORD = 200;
volatile uint16_t PLAYER_Y_COORD = 300;


uint32_t highScore = 000;
uint32_t userScore = 000;


uint8_t virusFrameCounter = 0;
volatile bool ALERT_VIRUS1 = false;
volatile bool ALERT_CAR1 = false;
volatile bool ALERT_TRUCK1 = false;
volatile bool ALERT_VIRUS2 = false;
volatile bool ALERT_CAR2 = false;
volatile bool ALERT_TRUCK2 = false;
volatile bool ALERT_PLAYER = true;
volatile bool ALERT_RAISE_SCORE = false;
volatile bool SWITCH_IMAGE = true;
volatile bool ERASE_CAR = false;
volatile bool ERASE_TRUCK = false;
bool gameOverFirstTime = true;
const uint8_t *virusBitMapVar;
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

// to be used for boundaries
bool contact_boundary(
    volatile PS2_DIR_t direction,			// direction in question
		volatile uint16_t firstObject_X_COORD, 
		volatile uint16_t firstObject_Y_COORD, 
		uint8_t first_height, 
		uint8_t first_width,
		volatile uint16_t secondObject_X_COORD, 
		volatile uint16_t secondObject_Y_COORD, 
		uint8_t second_height, 
		uint8_t second_width
)
{	
		// So, where does it want to go?
		switch(direction)
		{
			// True -> Edge has been hit, can't move anymore
			// False -> Edge has not been hit, movement is possible
			case PS2_DIR_UP:		// Trying go up?
			// Let's look at the Y coordinate
			if(check_game_over(firstObject_X_COORD,firstObject_Y_COORD,
				first_height,first_width,PLAYER_X_COORD,PLAYER_Y_COORD-2,invaderHeightPixels,
				invaderWidthPixels)) {
				// nope, you can't go more up
				return true;
			} else {
				// yeah, there's at least one pixel of room
			return false;
			}
			case PS2_DIR_DOWN:	// Trying go down?
			// Let's look at the Y coordinate
						if(check_game_over(firstObject_X_COORD,firstObject_Y_COORD,
				first_height,first_width,PLAYER_X_COORD,PLAYER_Y_COORD+2,invaderHeightPixels,
			invaderWidthPixels)) {
				// nope, you can't go more down
				return true;
			} else {
				// yeah, there's at least one pixel of room
			return false;
			}
			case PS2_DIR_LEFT:	// Trying to go left?
			// Let's look at the X coordinate
			// check if at the very left edge of the screen
						if(check_game_over(firstObject_X_COORD-1,firstObject_Y_COORD,
				first_height,first_width,PLAYER_X_COORD-2,PLAYER_Y_COORD,invaderHeightPixels,
			invaderWidthPixels)) {
				// nope, you can't go more left
				return true;
			} else {
				// yeah, there's at least one pixel of room
			return false;
			}
			case PS2_DIR_RIGHT:	// Trying to go right?
			// Let's look at the X coordinate
			// check if at the very right edge of the screen
			if(check_game_over(firstObject_X_COORD+1,firstObject_Y_COORD,
				first_height,first_width,PLAYER_X_COORD+2,PLAYER_Y_COORD,invaderHeightPixels,
			invaderWidthPixels)){
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

Rectangle first;				// Rectangle to help with boundary check of first
Rectangle second;		// Rectangle to help with boundary check of second

bool check_game_over (
        volatile uint16_t firstObject_X_COORD, 
        volatile uint16_t firstObject_Y_COORD, 
        uint8_t first_height, 
        uint8_t first_width,
        volatile uint16_t secondObject_X_COORD, 
        volatile uint16_t secondObject_Y_COORD, 
        uint8_t second_height, 
        uint8_t second_width
)
{
	// Let's make some rectangles to help us figure out the boundaries
	
	// First, the space first
	first.top = firstObject_Y_COORD - (first_height/2);
	first.bottom = firstObject_Y_COORD + (first_height/2);
	first.left = firstObject_X_COORD - (first_width/2);
	first.right = firstObject_X_COORD + (first_width/2);
	
	// Second, the space second
	second.top = secondObject_Y_COORD - (second_height/2);
	second.bottom = secondObject_Y_COORD + (second_height/2);
	second.left = secondObject_X_COORD - (second_width/2);
	second.right = secondObject_X_COORD + (second_width/2);
	
	// To check for an overlap, let's check conditions for when there is not an overlap
	// If the conditions for no overlap are not met, that means there is an overlap
	
	// Let's check the y axis first
	if (second.top > first.bottom || first.top > second.bottom)
		return false;
	// Now the x axis
	if (second.left > first.right || first.left > second.right)
		return false;
	// If neither statemetns were true, than there is an overlap
	return true;
}


//*****************************************************************************
// Renders out the high score from the high score variable
//*****************************************************************************
const uint8_t* getTheRightNumberHelper(uint8_t number) {
switch(number) {
	case 0: return zeroBitmap;
	break;
	case 1: return oneBitmap;
	break;
	case 2: return twoBitmap;
	break;
	case 3: return threeBitmap;
	break;
	case 4: return fourBitmap;
	break;
	case 5: return fiveBitmap;
	break;
	case 6: return sixBitmap;
	break;
	case 7: return sevenBitmap;
	break;
	case 8: return eightBitmap;
	break;
	case 9: return nineBitmap;
	break;
		default: return zeroBitmap;
	break;


}
}

//*****************************************************************************
// Renders out the high score from the high score variable
//*****************************************************************************
const uint8_t* getTheRightNumber(uint32_t number, uint8_t pos) { 
	uint8_t numberInThatPlace;
			switch(pos) {
				case 0:
							numberInThatPlace = number % 10;
				break;
				case 1:
							numberInThatPlace = ((number % 100) - ((number % 100) % 10))/10;
				break;
				case 2:
						numberInThatPlace = (number - (number % 100))/100;
				break;
				default:
						numberInThatPlace = 0;
					break;
			}
			return getTheRightNumberHelper(numberInThatPlace);
}



//*****************************************************************************
// Renders out the high score from the high score variable
//*****************************************************************************
void renderHighScore(void) {
	
				// Render out high score text
							lcd_draw_image(
							127,            									// X Center Point
							zeroWidthPixels,   // Image Horizontal Width
							300,            									// Y Center Point
							zeroHeightPixels, // Image Vertical Height
							getTheRightNumber(highScore, 2),      // Image
							LCD_COLOR_WHITE,         				// Foreground Color
							LCD_COLOR_BLACK        					// Background Color  
				);
								lcd_draw_image(
							149,            									// X Center Point
							zeroWidthPixels,   // Image Horizontal Width
							300,            									// Y Center Point
							zeroHeightPixels, // Image Vertical Height
							getTheRightNumber(highScore, 1),      // Image
							LCD_COLOR_WHITE,         				// Foreground Color
							LCD_COLOR_BLACK        					// Background Color  
				);
											lcd_draw_image(
							171,            									// X Center Point
							zeroWidthPixels,   // Image Horizontal Width
							300,            									// Y Center Point
							zeroHeightPixels, // Image Vertical Height
							getTheRightNumber(highScore, 0),      // Image
							LCD_COLOR_WHITE,         				// Foreground Color
							LCD_COLOR_BLACK        					// Background Color  
				);
}

//*****************************************************************************
// Renders out the user score from the user score variable onto the game screen
//*****************************************************************************
void renderUserScore(void) {
	
				// Render out high score text
							lcd_draw_image(
							94,            									// X Center Point
							zeroWidthPixels,   // Image Horizontal Width
							300,            									// Y Center Point
							zeroHeightPixels, // Image Vertical Height
							getTheRightNumber(userScore, 2),      // Image
							LCD_COLOR_WHITE,         				// Foreground Color
							LCD_COLOR_BLACK        					// Background Color  
				);
								lcd_draw_image(
							116,            									// X Center Point
							zeroWidthPixels,   // Image Horizontal Width
							300,            									// Y Center Point
							zeroHeightPixels, // Image Vertical Height
							getTheRightNumber(userScore, 1),      // Image
							LCD_COLOR_WHITE,         				// Foreground Color
							LCD_COLOR_BLACK        					// Background Color  
				);
											lcd_draw_image(
							138,            									// X Center Point
							zeroWidthPixels,   // Image Horizontal Width
							300,            									// Y Center Point
							zeroHeightPixels, // Image Vertical Height
							getTheRightNumber(userScore, 0),      // Image
							LCD_COLOR_WHITE,         				// Foreground Color
							LCD_COLOR_BLACK        					// Background Color  
				);
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
	
	lp_io_init();
  
	gp_timer_config_32(TIMER0_BASE,TIMER_TAMR_TAMR_PERIOD, 1000000, false, true);
	gp_timer_config_32(TIMER1_BASE,TIMER_TAMR_TAMR_PERIOD, 20000000, false, true);
  gp_timer_config_32(TIMER2_BASE,TIMER_TAMR_TAMR_PERIOD, 1000000, false, true);
  gp_timer_config_32(TIMER3_BASE,TIMER_TAMR_TAMR_PERIOD, 900000, false, true);
  gp_timer_config_16(TIMER4_BASE,TIMER_TAMR_TAMR_PERIOD, 1000000, false, true);
	gp_timer_config_32(TIMER5_BASE,TIMER_TAMR_TAMR_PERIOD, 1000000, false, true);
	
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
							200,            									// Y Center Point
							touchAnywhereHeightPixels, // Image Vertical Height
							touchAnywhereBitmap,      // Image
							LCD_COLOR_WHITE,         				// Foreground Color
							LCD_COLOR_BLACK        					// Background Color  
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
				renderHighScore();
			
			
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
			
			// Render out score text
							lcd_draw_image(
							42,            									// X Center Point
							scoreWidthPixels,   // Image Horizontal Width
							300,            									// Y Center Point
							scoreHeightPixels, // Image Vertical Height
							scoreBitmap,      // Image
							LCD_COLOR_WHITE,         				// Foreground Color
							LCD_COLOR_BLACK        					// Background Color  
				);
				// Render out the rest of the score text
				renderUserScore();
								
					while(!game_over)
							{	
								if(ALERT_RAISE_SCORE) {
								ALERT_RAISE_SCORE = false;
								userScore++;
								renderUserScore();
								}
								
								if(ERASE_CAR){
									ERASE_CAR = false;
									lcd_draw_rectangle_centered(222, 36, 240, 20, LCD_COLOR_BLACK);
								}
							if(ALERT_CAR1)
							{
								ALERT_CAR1 = false;
								
								lcd_draw_image(
									CAR11_X_COORD,            // X Center Point
									car1WidthPixels,   // Image Horizontal Width
									CAR11_Y_COORD,            // Y Center Point
									car1HeightPixels,  // Image Vertical Height
									car1Bitmaps,       // Image
									LCD_COLOR_BLACK,          // Foreground Color
									LCD_COLOR_ORANGE          // Background Color
								);
								game_over = check_game_over(
												CAR11_X_COORD,
												CAR11_Y_COORD,
												car1HeightPixels,
												car1WidthPixels,
												PLAYER_X_COORD,
												PLAYER_Y_COORD,
												invaderHeightPixels,
												invaderWidthPixels
											);
		if(game_over) {
				game_state = 2;
				break;
		};
							}
							if(ALERT_CAR2)
							{
								ALERT_CAR2 = false;
								lcd_draw_image(
									CAR12_X_COORD,            // X Center Point
									car1WidthPixels,   // Image Horizontal Width
									CAR12_Y_COORD,            // Y Center Point
									car1HeightPixels,  // Image Vertical Height
									car1Bitmaps,       // Image
									LCD_COLOR_BLACK,          // Foreground Color
									LCD_COLOR_GREEN          // Background Color
								);
							game_over = check_game_over(
												CAR12_X_COORD,
												CAR12_Y_COORD,
												car1HeightPixels,
												car1WidthPixels,
												PLAYER_X_COORD,
												PLAYER_Y_COORD,
												invaderHeightPixels,
												invaderWidthPixels
											);
										if(game_over) {
				game_state = 2;
				break;
		};
							}
							if(ERASE_TRUCK){
								ERASE_TRUCK = false;
									lcd_draw_rectangle_centered(214, 52, 100, 20, LCD_COLOR_BLACK);
								}
							if(ALERT_TRUCK1)
							{
								ALERT_TRUCK1= false;
								
								lcd_draw_image(
									CAR21_X_COORD,            // X Center Point
									car2WidthPixels,   // Image Horizontal Width
									CAR21_Y_COORD,            // Y Center Point
									car2HeightPixels,  // Image Vertical Height
									car2Bitmaps,       // Image
									LCD_COLOR_BLACK,          // Foreground Color
									LCD_COLOR_BLUE          // Background Color
								);
								game_over = check_game_over(
												CAR21_X_COORD,
												CAR21_Y_COORD,
												car2HeightPixels,
												car2WidthPixels,
												PLAYER_X_COORD,
												PLAYER_Y_COORD,
												invaderHeightPixels,
												invaderWidthPixels
											);
													if(game_over) {
				game_state = 2;
				break;
		};
							}
							if(ALERT_TRUCK2)
							{
								ALERT_TRUCK2= false;
								lcd_draw_image(
									CAR22_X_COORD,            // X Center Point
									car2WidthPixels,   // Image Horizontal Width
									CAR22_Y_COORD,            // Y Center Point
									car2HeightPixels,  // Image Vertical Height
									car2Bitmaps,       // Image
									LCD_COLOR_BLACK,          // Foreground Color
									LCD_COLOR_YELLOW          // Background Color
								);
											game_over = check_game_over(
												CAR22_X_COORD,
												CAR22_Y_COORD,
												car2HeightPixels,
												car2WidthPixels,
												PLAYER_X_COORD,
												PLAYER_Y_COORD,
												invaderHeightPixels,
												invaderWidthPixels
											);
										if(game_over) {
				game_state = 2;
				break;
		};
							}
							
							if(ALERT_VIRUS1)
							{
								ALERT_VIRUS1= false;
								
								// switches out bitmaps every now and then
								if(SWITCH_IMAGE) {
									virusBitMapVar = virus_detailBitmaps;
								} else {
								virusBitMapVar = virus_noDetailBitmaps;
								}
								
								if(virusFrameCounter != 20) {
								virusFrameCounter++;
								} else {
								SWITCH_IMAGE = !SWITCH_IMAGE;
									virusFrameCounter = 0;
								}
								
								lcd_draw_image (
									VIRUS1_X_COORD,            // X Center Point
									virus_detailWidthPixels,   // Image Horizontal Width
									VIRUS1_Y_COORD,            // Y Center Point
									virus_detailHeightPixels,  // Image Vertical Height
									virusBitMapVar,       // Image
									LCD_COLOR_BLUE,          	 // Foreground Color
									LCD_COLOR_BLACK          	 // Background Color
								);
								game_over = check_game_over(
												VIRUS1_X_COORD,
												VIRUS1_Y_COORD,
												virus_detailHeightPixels,
												virus_detailWidthPixels,
												PLAYER_X_COORD,
												PLAYER_Y_COORD,
												invaderHeightPixels,
												invaderWidthPixels
											);
										if(game_over) {
				game_state = 2;
				break;
		};
							}
							if(ALERT_VIRUS2)
							{
								ALERT_VIRUS2 = false;
								
								// switches out bitmaps every now and then
								if(SWITCH_IMAGE) {
									virusBitMapVar = virus_detailBitmaps;
								} else {
								virusBitMapVar = virus_noDetailBitmaps;
								}
								
								if(virusFrameCounter != 20) {
								virusFrameCounter++;
								} else {
								SWITCH_IMAGE = !SWITCH_IMAGE;
									virusFrameCounter = 0;
								}
								lcd_draw_image(
									VIRUS2_X_COORD,            // X Center Point
									virus_detailWidthPixels,   // Image Horizontal Width
									VIRUS2_Y_COORD,            // Y Center Point
									virus_detailHeightPixels,  // Image Vertical Height
									virusBitMapVar,       		 // Image
									LCD_COLOR_YELLOW,          // Foreground Color
									LCD_COLOR_BLACK       	   // Background Color
								);
											game_over = check_game_over(
												VIRUS2_X_COORD,
												VIRUS2_Y_COORD,
												virus_detailHeightPixels,
												virus_detailWidthPixels,
												PLAYER_X_COORD,
												PLAYER_Y_COORD,
												invaderHeightPixels,
												invaderWidthPixels
											);
										if(game_over) {
				game_state = 2;
				break;
		};
							}

							if(ALERT_PLAYER)
							{
								
							lcd_draw_image(
								PLAYER_X_COORD,          // X Center Point
								invaderWidthPixels,       // Image Horizontal Width
								PLAYER_Y_COORD,          // Y Center Point
								invaderHeightPixels,      // Image Vertical Height
								invaderBitmaps,           // Image
								LCD_COLOR_BLACK,            // Foreground Color
								LCD_COLOR_WHITE           // Background Color
								);


							game_over = check_game_over(
													CAR11_X_COORD,
													CAR11_Y_COORD,
													car1HeightPixels,
													car1WidthPixels,
													PLAYER_X_COORD,
													PLAYER_Y_COORD,
													invaderHeightPixels,
													invaderWidthPixels
												);
														if(game_over) {
				game_state = 2;
				break;
		};
												game_over = check_game_over(
												CAR12_X_COORD,
												CAR12_Y_COORD,
												car1HeightPixels,
												car1WidthPixels,
												PLAYER_X_COORD,
												PLAYER_Y_COORD,
												invaderHeightPixels,
												invaderWidthPixels
											);
													if(game_over) {
				game_state = 2;
				break;
		};
											game_over = check_game_over(
												VIRUS1_X_COORD,
												VIRUS1_Y_COORD,
												virus_detailHeightPixels,
												virus_detailWidthPixels,
												PLAYER_X_COORD,
												PLAYER_Y_COORD,
												invaderHeightPixels,
												invaderWidthPixels
											);
													if(game_over) {
				game_state = 2;
				break;
		};
											game_over = check_game_over(
												VIRUS2_X_COORD,
												VIRUS2_Y_COORD,
												virus_detailHeightPixels,
												virus_detailWidthPixels,
												PLAYER_X_COORD,
												PLAYER_Y_COORD,
												invaderHeightPixels,
												invaderWidthPixels
											);
													if(game_over) {
				game_state = 2;
				break;
		};
											game_over = check_game_over(
												CAR21_X_COORD,
												CAR21_Y_COORD,
												car2HeightPixels,
												car2WidthPixels,
												PLAYER_X_COORD,
												PLAYER_Y_COORD,
												invaderHeightPixels,
												invaderWidthPixels
											);
													if(game_over) {
				game_state = 2;
				break;
		};
											game_over = check_game_over(
												CAR22_X_COORD,
												CAR22_Y_COORD,
												car2HeightPixels,
												car2WidthPixels,
												PLAYER_X_COORD,
												PLAYER_Y_COORD,
												invaderHeightPixels,
												invaderWidthPixels
											);
				if(game_over) {
				game_state = 2;
				break;
		};
											ALERT_PLAYER = false;
										}
							}
							break;
			case 2:
							if(gameOverFirstTime) {
								lcd_clear_screen(LCD_COLOR_BLACK);
								gameOverFirstTime = false;
							}
				
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
