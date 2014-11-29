#include "RainMeter.h"

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
#include "CityWeather.h"

CityWeather Cities[5];
  
   
//---------------------------------------------------------------
// Time management
long curTime = 0;

// Weather Update
const unsigned long updateDelay = 5000; 
long updateTime = 0;

// Rain Meter
const unsigned long rainMeterDelay = 1000;
unsigned long rainMeterTime = rainMeterDelay;

// LCD
const unsigned long lcdDelay = 1;
unsigned long lcdTime = lcdDelay;


//---------------------------------------------------------------
// Touchscreen
#include <UTFT.h>
#include <UTouch.h>

// Declare which fonts we will be using
extern uint8_t BigFont[]; //16x16
extern uint8_t SevenSegNumFont[]; //32x50
int city, new_city;
int x, y, settings, update_i, temp_i;
String update[24] = {"01","02","03","04","05","06","07","08","09","10","11","12","13","14","15","16","17","18","19","20","21","22","23","24"};
String temp[3] = {"F","C","K"};

UTFT myGLCD(ITDB50,25,26,27,28);
UTouch myTouch(6,5,4,3,2);

// Button Selector Class
class CitySelector{
  
};

//-----------------------------------------------
// Non-Volatile Storage

//-----------------------------------------------
// Comms

#include "WeatherComms.h"
WeatherComms comms;
//-----------------------------------------------
// Helper Functions

//This functions takes in a string temperature and converts it to F, C, or K
String displayTemp(int temp_index, String temperature) {
  //Fahrenheit
  if(temp_index == 0) {
    return temperature;
  }
  //Celsius
  else if(temp_index == 1) {
    return String((int)((temperature.toInt()-32)/1.8));
  }
  //Kelvin
  else if(temp_index == 2) {
    return String((int)(((temperature.toInt()-32)/1.8)+273.15));
  }  
}

//This function returns the number of digits in a string containing a number
int digitCounter(String temp) {
  if(abs(temp.toInt()) < 100 & abs(temp.toInt()) >= 10) {
    return 2;
  }
  else if(abs(temp.toInt()) < 10) {
    return 1;
  }
  else if(abs(temp.toInt()) >= 100) {
    return 3;
  }
}

//---------------------------------------------------------------
// the setup routine runs once when you press reset:
void setup() {
  // LCD
  myGLCD.InitLCD(PORTRAIT);
  myTouch.InitTouch(LANDSCAPE);
  myTouch.setPrecision(PREC_MEDIUM);
  myGLCD.clrScr();
  
  // City stuff
  city = 0;
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

  // Update LCD Task - Performs scrolling of data.

  //Clear screen logic
  if(new_city != city) {
    myGLCD.clrScr();
    city = new_city;
  }
  
  myGLCD.setColor(255, 0, 0);
  myGLCD.setFont(BigFont);
  myGLCD.print("Wall Weather Forecaster", CENTER, 0);
  
  //Set the Current Time
  myGLCD.setFont(SevenSegNumFont);
  myGLCD.print("10", 150, 80);
  myGLCD.setFont(BigFont);
  myGLCD.print(":", 210, 100);
  myGLCD.setFont(SevenSegNumFont);
  myGLCD.print("17", 220, 80);
  myGLCD.setFont(BigFont);
  myGLCD.print("PM", 290, 100);
  
  //Print Current City
  myGLCD.print(Cities[city].cityName, CENTER, 175);
  
  //Print Condition
  myGLCD.print("Condition:", 0, 240);
  myGLCD.print(Cities[city].condition, 163, 240);
  
  //Print High Temperature
  myGLCD.print("High Temp:", 0, 300);
  myGLCD.setFont(SevenSegNumFont);
  
  // Set Percentage  
  meter->setChance(Cities[city].rainChance);
  //if displaying triple digit number
  if(digitCounter(displayTemp(temp_i, Cities[city].highTemp)) == 3) {
    //Negative Temp, print '-' sign
    if(displayTemp(temp_i, Cities[city].highTemp).toInt() < 0) {
      myGLCD.setFont(BigFont);
      myGLCD.print("-", 160, 300);
      myGLCD.setFont(SevenSegNumFont);
    }
    myGLCD.print(String(abs(displayTemp(temp_i, Cities[city].highTemp).toInt())), 175, 280);
  }
  else if(digitCounter(displayTemp(temp_i, Cities[city].highTemp)) == 2) {
    //Negative Temp, print '-' sign
    if(displayTemp(temp_i, Cities[city].highTemp).toInt() < 0) {
      myGLCD.setFont(BigFont);
      myGLCD.print("-", 192, 300);
      myGLCD.setFont(SevenSegNumFont);
    }
    myGLCD.print(String(abs(displayTemp(temp_i, Cities[city].highTemp).toInt())), 207, 280);
  }
  else if(digitCounter(displayTemp(temp_i, Cities[city].highTemp)) == 1) {
    //Negative Temp, print '-' sign
    if(displayTemp(temp_i, Cities[city].highTemp).toInt() < 0) {
      myGLCD.setFont(BigFont);
      myGLCD.print("-", 224, 300);
      myGLCD.setFont(SevenSegNumFont);
    }
    myGLCD.print(String(abs(displayTemp(temp_i, Cities[city].highTemp).toInt())), 239, 280);
  }

  myGLCD.setFont(BigFont);
  myGLCD.print(temp[temp_i], 270, 280);
  
  //if displaying triple digit number
  if(digitCounter(displayTemp(temp_i, Cities[city].lowTemp)) == 3) {
    //Print Low Temperature
    myGLCD.print("Low Temp:", 0, 380);
    myGLCD.setFont(SevenSegNumFont);
    if(displayTemp(temp_i, Cities[city].lowTemp).toInt() < 0) {
      myGLCD.setFont(BigFont);
      myGLCD.print("-", 160, 380);
      myGLCD.setFont(SevenSegNumFont);
    }
    myGLCD.print(String(abs(displayTemp(temp_i, Cities[city].lowTemp).toInt())), 175, 360);
  }
  else if(digitCounter(displayTemp(temp_i, Cities[city].lowTemp)) == 2) {
    //Print Low Temperature
    myGLCD.print("Low Temp:", 0, 380);
    myGLCD.setFont(SevenSegNumFont);
    if(displayTemp(temp_i, Cities[city].lowTemp).toInt() < 0) {
      myGLCD.setFont(BigFont);
      myGLCD.print("-", 192, 380);
      myGLCD.setFont(SevenSegNumFont);
    }
    myGLCD.print(String(abs(displayTemp(temp_i, Cities[city].lowTemp).toInt())), 207, 360);
  }
  else if(digitCounter(displayTemp(temp_i, Cities[city].lowTemp)) == 1) {
    //Print Low Temperature
    myGLCD.print("Low Temp:", 0, 380);
    myGLCD.setFont(SevenSegNumFont);
    if(displayTemp(temp_i, Cities[city].lowTemp).toInt() < 0) {
      myGLCD.setFont(BigFont);
      myGLCD.print("-", 224, 380);
      myGLCD.setFont(SevenSegNumFont);
    }
    myGLCD.print(String(abs(displayTemp(temp_i, Cities[city].lowTemp).toInt())), 239, 360);
  }  
  
  myGLCD.setFont(BigFont);
  myGLCD.print(temp[temp_i], 270, 360); 
  
  //Print Humidity %
  myGLCD.print("Humidity:", 0, 460);
  myGLCD.setFont(SevenSegNumFont);
  if(digitCounter(Cities[city].humidityPercent) == 3)
    myGLCD.print(Cities[city].humidityPercent, 175, 440);
  else if(digitCounter(Cities[city].humidityPercent) == 2)
    myGLCD.print(Cities[city].humidityPercent, 207, 440);
  else if(digitCounter(Cities[city].humidityPercent) == 1)
    myGLCD.print(Cities[city].humidityPercent, 239, 440);
    
  myGLCD.setFont(BigFont);
  myGLCD.print("%", 270, 440); 
  
  if(settings == 0) {
    //Draw Cities/Settings Boxes
    myGLCD.print("Stored Cities", CENTER, 570);
    myGLCD.setFont(SevenSegNumFont);
    if(city == 0) {
      myGLCD.setColor(0, 0, 255); 
      myGLCD.drawRect(51, 600, 105, 675);
      myGLCD.print("1", 55, 610);
      myGLCD.setColor(255, 0, 0); 
      myGLCD.drawRect(132, 600, 186, 675);
      myGLCD.print("2", 143, 610);
      myGLCD.drawRect(213, 600, 267, 675);
      myGLCD.print("3", 224, 610);
      myGLCD.drawRect(294, 600, 348, 675);
      myGLCD.print("4", 305, 610);
      myGLCD.drawRect(375, 600, 429, 675);
      myGLCD.print("5", 386, 610);  
    }
    else if(city == 1) {
      myGLCD.drawRect(51, 600, 105, 675);
      myGLCD.print("1", 55, 610);
      myGLCD.setColor(0, 0, 255); //draw in blue
      myGLCD.drawRect(132, 600, 186, 675);
      myGLCD.print("2", 143, 610);
      myGLCD.setColor(255, 0, 0); //draw in red
      myGLCD.drawRect(213, 600, 267, 675);
      myGLCD.print("3", 224, 610);
      myGLCD.drawRect(294, 600, 348, 675);
      myGLCD.print("4", 305, 610);
      myGLCD.drawRect(375, 600, 429, 675);
      myGLCD.print("5", 386, 610);
      myGLCD.drawRect(375, 600, 429, 675);   
    }
    else if(city == 2) {
      myGLCD.drawRect(51, 600, 105, 675);
      myGLCD.print("1", 55, 610); 
      myGLCD.drawRect(132, 600, 186, 675);
      myGLCD.print("2", 143, 610);
      myGLCD.setColor(0, 0, 255); //draw in blue
      myGLCD.drawRect(213, 600, 267, 675);
      myGLCD.print("3", 224, 610);
      myGLCD.setColor(255, 0, 0); //draw in red
      myGLCD.drawRect(294, 600, 348, 675);
      myGLCD.print("4", 305, 610);
      myGLCD.drawRect(375, 600, 429, 675);
      myGLCD.print("5", 386, 610);
    }
    else if(city == 3) {
      myGLCD.drawRect(51, 600, 105, 675);
      myGLCD.print("1", 55, 610);
      myGLCD.drawRect(132, 600, 186, 675);
      myGLCD.print("2", 143, 610);
      myGLCD.drawRect(213, 600, 267, 675);
      myGLCD.print("3", 224, 610);
      myGLCD.setColor(0, 0, 255); //draw in blue
      myGLCD.drawRect(294, 600, 348, 675);
      myGLCD.print("4", 305, 610);
      myGLCD.setColor(255, 0, 0); //draw in red
      myGLCD.drawRect(375, 600, 429, 675);
      myGLCD.print("5", 386, 610); 
    }
    else if(city == 4) {
      myGLCD.drawRect(51, 600, 105, 675);
      myGLCD.print("1", 55, 610);
      myGLCD.drawRect(132, 600, 186, 675);
      myGLCD.print("2", 143, 610);
      myGLCD.drawRect(213, 600, 267, 675);
      myGLCD.print("3", 224, 610);
      myGLCD.drawRect(294, 600, 348, 675);
      myGLCD.print("4", 305, 610);
      myGLCD.setColor(0, 0, 255); //draw in blue
      myGLCD.drawRect(375, 600, 429, 675);
      myGLCD.print("5", 386, 610); 
      myGLCD.setColor(255, 0, 0); //draw in red 
    }
    
    myGLCD.setFont(BigFont);
    myGLCD.drawRect(51, 710, 429, 765);
    myGLCD.print("Settings", CENTER, 730);
    
    //TouchScreen Logic
    if(myTouch.dataAvailable()) {
      myTouch.read();
      x = myTouch.getX();
      y = myTouch.getY();
      if(x >= 125 & x <= 200) {
        if(y >= 51 & y <= 105){
          new_city = 0;
        }
        else if(y >= 132 & y <= 186){
          new_city = 1;
        }
        else if(y >= 213 & y <= 267){
          new_city = 2;
        }
        else if(y >= 294 & y <= 348){
          new_city = 3;
        }
        else if(y >= 375 & y <= 429){
          new_city = 4;
        }    
      }
      else if(x >= 35 & x <= 90) {
        if(y >= 51 & y <= 429) {
          settings = 1;  
          myGLCD.clrScr();
        } 
      }
    }
  }
  else if(settings == 1) {
    myGLCD.drawLine(0,515,480,515);
    myGLCD.print("Update Interval:", 0, 555);
    myGLCD.drawRect(265,550,305,580);
    myGLCD.print("<",275,560);
    
    myGLCD.setFont(SevenSegNumFont);
    myGLCD.print(update[update_i], 310, 535);
    
    myGLCD.setFont(BigFont);
    myGLCD.print("hr", 375,555);
    myGLCD.drawRect(410,550,450,580);
    myGLCD.print(">",420,560);
    
    if(temp_i == 0) {
      myGLCD.setColor(0,0,255);
      myGLCD.drawRect(80,625,160,675);
      myGLCD.print("F",110,645);
      myGLCD.setColor(255,0,0);
      myGLCD.drawRect(200,625,280,675);
      myGLCD.print("C", 230, 645);
      myGLCD.drawRect(320,625,400,675);
      myGLCD.print("K", 350, 645);
    }
    else if(temp_i == 1) {
      myGLCD.drawRect(80,625,160,675);
      myGLCD.print("F",110,645);
      myGLCD.setColor(0,0,255);
      myGLCD.drawRect(200,625,280,675);
      myGLCD.print("C", 230, 645);
      myGLCD.setColor(255,0,0);
      myGLCD.drawRect(320,625,400,675);
      myGLCD.print("K", 350, 645);
    }
    else if(temp_i == 2) {
      myGLCD.drawRect(80,625,160,675);
      myGLCD.print("F",110,645);
      myGLCD.drawRect(200,625,280,675);
      myGLCD.print("C", 230, 645);
      myGLCD.setColor(0,0,255);
      myGLCD.drawRect(320,625,400,675);
      myGLCD.print("K", 350, 645);
      myGLCD.setColor(255,0,0);
    }
    
    myGLCD.drawRect(80,725,200,775);
    myGLCD.print("Update", 95, 740);
    myGLCD.drawRect(280,725,400,775);
    myGLCD.print("Back", 305, 740);
    
    if(myTouch.dataAvailable()) {
      myTouch.read();
      x = myTouch.getX();
      y = myTouch.getY();
      //temp buttons
      if(x >= 100 & x <= 200) {
        if(y >= 60 & y < 180){
          temp_i = 0;
          myGLCD.clrScr();
        }
        else if(y >= 180 & y < 300) {
          temp_i = 1; 
          myGLCD.clrScr();
        }
        else if(y >= 300 & y <= 420) {
          temp_i = 2; 
          myGLCD.clrScr();
        }
      }
      else if(x >= 210 & x <= 260) {
        // < button pressed
        if(y >= 250 & y <= 320) {
          if(update_i == 0) {
            update_i = 23;
          }
          else {
            update_i = update_i - 1;  
          }
        }
        //> button pressed
        else if(y >= 395 & y <= 470) {
          if(update_i == 23) {
            update_i = 0;
          }
          else {
            update_i = update_i + 1;
          }  
        }
      }
      else if(x >= 0 & x < 100) {
        //Update button pressed
        if(y >= 60 & y < 240) {
          //do update stuff
        }  
        //settings button pressed
        else if(y >= 240 & y <= 420) {
          settings = 0;  
          myGLCD.clrScr();
        }
      }
    } 
  }
}
