// Nicholas Dedenbach
// 
// This sketch utilizes modified UTouch and UTFT libraries
// in order to create a touch screen interface for the WWF.

#include <UTouch.h>
#include <UTouchCD.h>

#include <memorysaver.h>
#include <UTFT.h>

#include <CityWeather.h>

// Set the screen's normal mode to landscape
#define TOUCH_ORIENTATION LANDSCAPE

// Number of buttons is number of cities wanted plus settings
// within the context of the WWF
#define NUM_BUTTONS 6

enum State {
  MAIN,
  SETTINGS
} state;

// Initialize our LCD and Touch modules
UTFT LCD(SSD1963_800, 3, 4, 5, 6);
UTouch touch(40, 39, 38, 37, 36);

// Link to a defined font within the library
extern uint8_t SmallFont[];

// Used to keep track of screen size
int dispx, dispy;

// Used to keep track of where screen is being touched
int touch_x, touch_y;

// Used to track which button was last pressed
int buttonPressed = 0;

// Array of city information loaded from the command center
CityWeather cities[5];

void setup()
{
  // Your basic LCD initialization functions
  LCD.InitLCD();
  LCD.clrScr();
  LCD.setFont(SmallFont);
  LCD.setBackColor(VGA_BLUE);
  dispx = LCD.getDisplayXSize();
  dispy = LCD.getDisplayYSize();
  
  // Set up the touch interface
  touch.InitTouch(TOUCH_ORIENTATION);
  
  // Show the main screen initially
  state = MAIN;
  
  // Load in the intial city data from the control center
}

void loop()
{
  if (state == MAIN) {
    // Draw the buttons
    rowButtons(NUM_BUTTONS, 0, dispy / 4);
    
    // Check if the screen is being touched
    // If so, update the onscreen information
    if (touch.dataAvailable()){
      // Get the X and Y coordinates of the touch
      touch.read();
      touch_x = touch.getX();
      touch_y = touch.getY();
      
      // Calculate which button is being pressed
      buttonPressed = detectRowPress(NUM_BUTTONS, 0, dispy / 4, touch_x, touch_y);
    }
    
    if (buttonPressed == NUM_BUTTONS) {
      state = SETTINGS;
    }
    
    // Logic for what city is to be displayed
    LCD.setColor(VGA_WHITE);
    LCD.print(cities[buttonPressed - 1].getCityName(), dispx * 0.1, dispy * 0.95);
    LCD.print(cities[buttonPressed - 1].getCondition(), dispx * 0.1, dispy * 0.85);
    LCD.print("High: " + cities[buttonPressed - 1].getHigh(), dispx * 0.1, dispy * 0.75);
    LCD.print("Low: " + cities[buttonPressed - 1].getLow(), dispx * 0.1, dispy * 0.65);
    LCD.print("Humidity: " + cities[buttonPressed - 1].getHumidity(), dispx * 0.1, dispy * 0.55);
  }
  else if (state == SETTINGS){
    // Settings menu stuff handled
  }
}

void rowButtons(int numButtons, int bottom, int top){
  // Fit numButtons across the screen
  int x = 0;
  int buttonWidth = dispx / numButtons;
  for (int i = 1; i <= numButtons; i++){
    // Set color and draw rectangle perimeter
    LCD.setColor(VGA_BLACK);
    LCD.drawRect(x + 5, bottom, x + buttonWidth - 5, top);
    // Set color and fill rectangle with color
    LCD.setColor(VGA_RED);
    LCD.fillRect(x + 5, bottom, x + buttonWidth - 5, top);
    // Place the appropriate button label on it
    LCD.setColor(VGA_BLACK);
    if (i != numButtons){
      LCD.printNumI(i, x + buttonWidth - 5, bottom + (top - bottom)/2 + 5);
    }
    else{
      LCD.print("S", x + buttonWidth - 5, bottom + (top - bottom)/2 + 5); 
    }
    
    // Increment to the next button
    x += buttonWidth;
  }
}     

int detectRowPress(int numButtons, int bottom, int top, int x, int y){
  // Set up button width
  int buttonWidth = dispx / numButtons;
  int button_x = 0;
  
  // Loop through each button zone, checking for a touch
  if (y >= bottom && y <= top){
    for (int i = 1; i <= numButtons; i++){
      if (x > (button_x + 5) && x < (button_x + buttonWidth - 5)) {
        return i;
      }
    }
  }

  return 0;
}
