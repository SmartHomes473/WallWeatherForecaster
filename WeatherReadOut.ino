/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.
 
  This example code is in the public domain.
 */
#include <Servo.h>
#include "rgb_lcd.h"
#include <Ethernet.h>
#include <SD.h>
//---------------------------------------------------------------
// Pin parameters
#define statLedPin 13
#define rainMeterPin 5
#define areaSelectorPin 6
#define citySwitchPin 14
//---------------------------------------------------------------

// Some constants
#define DELTA 25
#define degree String(char(223))
byte mac[] = { 0x98, 0x4f, 0xee, 0x01, 0x86, 0x30 };  

//---------------------------------------------------------------
// Status LED
int statLedState = LOW;

//---------------------------------------------------------------
// rainMeter - Chance of rain
Servo rainMeter;
int percentage = 0;
int percentageDelta = DELTA;

int SetServoPercent(Servo servo, int percent)
{ 
  // Sets servo arm to percentage.
  //    0 % is all the way counter clockwise
  //  100 % is all the way clockwise.
   servo.write((100-percent) *1.8);
}


//---------------------------------------------------------------
// LCD - Displays temp, humidity etc.
char blah = 'a';
class CityWeather{    
  public:
    unsigned cityIndex = 0;
    unsigned dataIndex = 0;
    int rainChance = 50;
    String cityName;
    String data;

   CityWeather() {
     //CityWeather( "Bum Fuk, Egypt"+blah,blah*10,blah*5,blah*20,blah++*20);
     cityName = "Bum Fuk, Egypt"+String(blah);
     data = "Hi: 999"+String(degree)+"K Low: 1"+String(degree)+"K Humidity: 100%"+String(blah++)+" ";
     rainChance = 50; 
   }
   
   CityWeather(String fileName){
     // Read from file.
   }
   
   CityWeather(String name, int hi, int low, unsigned humidity, unsigned rain)
   {
     cityName = name;
     data = String("Hi: " +hi%1000+String(degree)+"F Low: "+low%1000+String(degree)+"F Humidity: "+humidity%101+'% ');
     rainChance = rain%101;
   }
   
   String GetCityName()
   {
     return cityName;
   }
   
   String GetData()
   {
     return data;
   }
   
   unsigned GetRainChance()
   {
    return rainChance; 
   }
   
   void LoadScrolling(String source, char *dest, unsigned *sIndex)
   {
     //clearer.toCharArray(dest,17);
     if(source.length() < 16)
     {
       (source+"             ").toCharArray(dest,17);
     }
     else
     {
       if(source.length() >= (*sIndex+16))
       {
         source.substring(*sIndex).toCharArray(dest,17);
       }
       else
       {
         (source.substring(*sIndex)+source).toCharArray(dest,17);
       }
      *sIndex = ((*sIndex)+1) % source.length();
     }
   }
   
   void LoadCityName(char *s) {LoadScrolling(cityName,s,&cityIndex);}
   
   void LoadData(char *s) {LoadScrolling(data,s,&dataIndex);}
};

rgb_lcd lcd;

CityWeather Cities[5];
char c[17];
char d[17];


//---------------------------------------------------------------
// Switch Reading - Determines city displayed on LCD screen.
unsigned switchOldReading;
unsigned switchCurReading;
   
//---------------------------------------------------------------
// Time management
long curTime = 0;

// Status LED
const unsigned long statLedDelay = 1000;
long statLedTime = statLedDelay;

// Rain Meter
const unsigned long rainMeterDelay = 5000;
unsigned long rainMeterTime = rainMeterDelay;

// LCD
const unsigned long lcdDelay = 250;
unsigned long lcdTime = lcdDelay;

// Switch
const unsigned long switchDelay = 1000;
unsigned long switchTime = switchDelay;

//---------------------------------------------------------------
// the setup routine runs once when you press reset:
void setup() {
  // LCD
  lcd.begin(16, 2);
  lcd.setRGB(255,0,0);
  lcd.print("Initalizing:");
  lcd.setCursor(0,1);
  lcd.print("WORKING");
  
  // Serial for debugging
  Serial.begin(9600);
  // Initalize ip and telnet
  system("telnetd -l /bin/sh");
  system("touch FINDME.FIND");
  delay(3000);
  if (Ethernet.begin(mac) == 0)
      Serial.println("Failed to configure Ethernet using DHCP");
  else
      Serial.println(Ethernet.localIP());
  
  // initialize the digital pin as an output.
  pinMode(statLedPin, OUTPUT);
  
  // Run python script to retrieve weather information.
  SD.begin();
  File f = SD.open("/weather.txt");
 for(int i =0; i < 5; i++)
 {
   int j = 0;
    while(char(f.peek())!= '\n')
     {  
     String temp;
      while(f.peek() != ',')
      {
        temp = temp + char(f.read());
        if(f.peek() == '\n')
          break;
      }
      Serial.println(temp+j);
      switch(j++)
      { // "Hi: 999"+String(degree)+"K Low: 1"+String(degree)+"K Humidity: 100%"+String(blah++)+" ";
        case 0:
          Cities[i].cityName = temp;
        break;
        case 1:
          Cities[i].cityName = Cities[i].cityName+','+temp;
        break;
        case 2:
          Cities[i].data = "High: "+temp+degree+"F "; 
        break;
        case 3:
          Cities[i].data = Cities[i].data+"Low: "+temp+degree+"F "; 
        break;
        case 4:
          Cities[i].data = Cities[i].data+"Humidity: "+temp+"% ";
        break;
        case 5:
          Serial.println("String of Chance,"+temp+"\nIndex ="+i);
          Serial.println(temp.toInt());
          Cities[i].rainChance = temp.toInt()%101;
        break;
        
      }
      if(f.peek() != '\n')
        f.read();
     } 
    Serial.println(Cities[i].cityName);
    Serial.println(Cities[i].data);
    Serial.println(Cities[i].rainChance);
   Serial.print(char(f.read()));
   Serial.println(i);
 }
   Serial.println("Done");
 
 
  
  // Initalize Servo for rainMeter
  rainMeter.attach(rainMeterPin,544,2370);
  
  // Get current city
  switchOldReading = analogRead(citySwitchPin);
  
  lcd.setCursor(0,1);
  delay(1000);
  lcd.print("COMPLETE");
  lcd.setRGB(0,255,0);
  delay(2000);
  lcd.clear();
  lcd.setRGB(255,255,255);
}

// the loop routine runs over and over again forever:
void loop() {
  // Current time for task timing.
  curTime = millis();
  
  if(switchTime < curTime)
  {
    switchTime = curTime + switchDelay;
    switchCurReading = analogRead(citySwitchPin)/205;
    Serial.println(analogRead(citySwitchPin)/205);
    if(switchCurReading != switchOldReading)
    {
      Serial.println("Chance: "+int(Cities[switchCurReading].rainChance));
      SetServoPercent(rainMeter,Cities[switchCurReading].rainChance);
      lcdTime = curTime - 1;
      switchOldReading = switchCurReading;
    }
  }
  // Status LED task - toggle power every second.
  if(statLedTime < curTime)
  {
    statLedTime = curTime + statLedDelay;
    if(statLedState == HIGH)
      statLedState = LOW;
    else
      statLedState = HIGH;
    digitalWrite(statLedPin, statLedState);   // turn the LED on (HIGH is the voltage level)
  }
  
  // Update LCD Task - Performs scrolling of data.
  if(lcdTime < curTime)
  {
    lcdTime += lcdDelay;
    Cities[switchOldReading].LoadCityName(c);
    Cities[switchOldReading].LoadData(d);
    lcd.setCursor(0,0);
    lcd.write(c);
    lcd.setCursor(0,1);
    lcd.write(d);
  }
  /*
  // Rain Meter Task - Adjust servo to percentage
  if(rainMeterTime < curTime)
  {
    rainMeterTime = curTime + rainMeterDelay;
    // Get new Percentage and move servo.
    percentage = percentage + percentageDelta;
    SetServoPercent(rainMeter,percentage);
    // Change degree change if at max/min
    if(percentage >= 100)
      percentageDelta = -DELTA;
    else if(percentage <= 0)
      percentageDelta = DELTA;
    Serial.println(percentage);
  }
  */
}
