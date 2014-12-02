#include <RainMeter.h>
//---------------------------------------------------------------
// Pin parameters - All pins owned by devices.

// PIR MOTION SENSOR
// TRIGGER -> NC
// OUTPUT -> 8
int sensorPin = 54;

// STEPPER MOTOR
// YELLOW -> 9
// ORANGE -> 10
// BROWN -> 11
// BLACK -> 12
int motorPins[4] = {9, 10, 11, 12};

//---------------------------------------------------------------
// Motion Sensor
int lcdState = HIGH;

//---------------------------------------------------------------
// RainMeter - Chance of rain

RainMeter *meter;

//---------------------------------------------------------------
// CityWeather - City weather information data structure
#include <CityWeather.h>

CityWeather Cities[5];
  
   
//---------------------------------------------------------------
// Time management
long curTime = 0;
 
// Add minute
const unsigned long clockDelay = 60000;
unsigned long clockTime = clockDelay;

// Update delay
unsigned long updateDelay = 1000;
unsigned long updateTime = updateDelay;
//---------------------------------------------------------------
// Touchscreen
#include <UTFT.h>
#include <UTouch.h>
#include <Display.h>
Display LCDTouchscreen;
// Declare which fonts we will be using
extern uint8_t BigFont[]; //16x16
extern uint8_t SevenSegNumFont[]; //32x50
int city, new_city;
int x, y, settings, update_i, temp_i;
String update[24] = {"01","02","03","04","05","06","07","08","09","10","11","12","13","14","15","16","17","18","19","20","21","22","23","24"};
String temp[3] = {"F","C","K"};
String hour ="6", minute = "66", meridian="NT";

UTFT myGLCD(ITDB50,25,26,27,28);
UTouch myTouch(6,5,4,3,2);

bool touchLastFrame = false;

//-----------------------------------------------
// Comms

#include <WeatherComms.h>
WeatherComms comms;

//---------------------------------------------------------------
// the setup routine runs once when you press reset:
void setup() {
  // LCD
  myGLCD.InitLCD(PORTRAIT);
  myTouch.InitTouch(LANDSCAPE);
  myTouch.setPrecision(PREC_MEDIUM);
  myGLCD.clrScr();
  
  // City stuff
  city = -1;
  new_city = 0;
  settings = 0;
  update_i = 0; //index to update array
  temp_i = 0; //index to temp array
  
  // Comms
  Serial.begin(9600);
  Serial1.begin(9600);
  delay(500);
  comms.registerDevice();
  
  //Set up the rain meter
  pinMode(motorPins[0], OUTPUT);
  pinMode(motorPins[1], OUTPUT);
  pinMode(motorPins[2], OUTPUT);
  pinMode(motorPins[3], OUTPUT);
  meter = new RainMeter(7.5, 180.0, motorPins);
  
  // Set up PIR sensor output pin (and LED for status)
  pinMode(sensorPin, INPUT_PULLUP);
  pinMode(13, OUTPUT);
  
  // Request Weather
  Serial.println("Init: Done");
  
  // Setup the LCD
  delay(1000);
  comms.SendRequest("f");
}

int percent = 0;
// the loop routine runs over and over again forever:
void loop() {
  // Logic for if screen is on
  if (analogRead(sensorPin) > 512) {
    myGLCD.lcdOn();
    digitalWrite(13, HIGH);
  }
  else {
    myGLCD.lcdOff();
    digitalWrite(13, LOW);
  }
  
  // Current time for task timing.
  curTime = millis();
  
  // Check for incomming messages
  comms.packetBuilder();

  // Automatic update upkeep
  /* if (updateTime < curTime) {
    updateTime += updateDelay * (update_i + 1);
    comms.SendRequest("f");
    Serial.println("New update");
  } */
  
  // Update LCD Task - Performs scrolling of data.
  if(clockTime < curTime)
  {
    clockTime += clockDelay;
    city = -1;
    int hourRollover = 0;
    // Increase minute
    if(minute[1] == '9')
    {
      if(minute[0] == '5')
        {
          minute[0] = '0';
          hourRollover = 1;
        }
       else
          minute[0]++;
       minute[1] ='0';
    }
    else
    {
     minute[1]++; 
    }
    // Hour
    if(hourRollover)
    {
      if(hour.length() == 1)
      {
         if(hour == "9")
           hour = "10"; 
         else
           hour[0]++;
      }
      else
      {
         if(hour == "12")
           hour = "1";
         else
           hour[1]++;
      }
    }
  }
  LCDTouchscreen.dumbDisplay();
}
