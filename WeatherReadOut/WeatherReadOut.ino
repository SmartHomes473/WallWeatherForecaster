//---------------------------------------------------------------
// Pin parameters - All pins owned by devices.

// PIR MOTION SENSOR
// TRIGGER -> NC
// OUTPUT -> 8
int sensorPin = 8;

// STEPPER MOTOR
// YELLOW -> 9
// ORANGE -> 10
// BROWN -> 11
// BLACK -> 12
int yellowPin = 9;
int orangePin = 10;
int brownPin = 11;
int blackPin = 12;

//---------------------------------------------------------------


//---------------------------------------------------------------
// rainMeter - Chance of rain
int percentage = 0;
class RainMeter{
  public:
  // Current Chance
  int chance;
  // Current State
  int state;
  // Pins
  int yellow, orange,brown,black;
  
  RainMeter(int yel, int org, int brn, int blk){
    state = 0;
    yellow = yel;
    orange = org;
    brown  = brn;
    black  = blk;
    pinMode(yellow, OUTPUT);
    pinMode(orange, OUTPUT);
    pinMode(brown, OUTPUT);
    pinMode(black, OUTPUT);
    SetState(state);
    ResetToZero();
  }
  
  void ResetToZero(){
    
  }
  
  void SetState(int nextState){
    switch(nextState)
    {
       case 0:
         
       break; 
       case 1:
         
       break; 
       case 2:
         
       break; 
       case 3:
         
       break; 
       case 5:
         
       break;
       default:
         
       break; 
    }
  }
  void SetChance(int pop){
    
  }
};

RainMeter * Meter;

//---------------------------------------------------------------
// LCD - Displays temp, humidity etc.
char blah = 'a';

class CityWeather{    
  public:
    // Precent of Percipitation
    int rainChance;
    // City Name
    String cityName;
    // High forecasted temperature
    String highTemp;
    // Low forecasted temperature
    String lowTemp;
    // Humidity percentage
    String humidityPercent;
    // Condition
    String condition;
    

   CityWeather() {
     // Default constructor
     cityName = "Bum Fuk, Egypt"+String(blah);
     rainChance = 50; 
     blah++;
     highTemp = "100";
     lowTemp = "-32";
     humidityPercent = "56";
     condition = "Cold as Shit";
   }
   
   CityWeather(String name, int hi, int low, unsigned humidity, unsigned rain)
   {
       UpdateWeather(name,hi,low,humidity,rain);
   }
   
   void UpdateWeather(String name, int hi, int low, unsigned humidity, unsigned rain)
   {
       cityName = name;
       highTemp = String(hi);
       lowTemp = String(low);
       humidityPercent = String(humidity);
       rainChance = rain%101;
   }
};

CityWeather Cities[5];

//-----------------------------------------------
// ISR
/*
// ISR for LCD toggle
void lcdToggle(){
   // Toggle the LCD flag
  lcdState = !lcdState;
  // Turn it off or on
  if (lcdState){
    LCD.lcdOn();
  }
  else {
    LCD.lcdOff();
  }
}
*/
  
   
//---------------------------------------------------------------
// Time management
long curTime = 0;

// Weather Update
const unsigned long updateDelay = 5000; 
long updateTime = 0;

// Motion Sensor
int lcdState = HIGH;

// Rain Meter
const unsigned long rainMeterDelay = 5000;
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

//---------------------------------------------------------------
// Comms
#define WaitingforStart 0
#define WaitingforEnd 1
#define PacketComplete 2
byte SMRTControlID = 1;
// Status of string being processed;
byte packetState = WaitingforStart;
// Incomming char buffer
#define INCOMINGMAX 2024
char incoming[2024];
// partail packet buffer
#define INBUFFERMAX 8192
char inbuffer[INBUFFERMAX];
long inbufferI = 0;
// full packet buffer
String packet;

void SendAck(String data)
{
  // Sends formated message of acknowledge
  unsigned len = data.length();
  Serial1.print("\x0f\x01\x03");
  Serial1.print(char(len>>8));
  Serial1.print(char(len&0xff));
  Serial1.print(data);
  Serial1.println("\x04");
}

void SendRequest(String data)
{
  // Sends formated request
  unsigned len = data.length();
  Serial1.print("\x0f\x01\x05");
  Serial1.print(char(len>>8));
  Serial1.print(char(len&0xff));
  Serial1.print(data);
  Serial1.println("\x04");
}

String ParseValue(int &i,String s)
{
 // Extracts value from string
 while(s != NULL && s[i] != ';') {
   i++;
 }
 int start = ++i;
 while(s != NULL && (s[i] != ';' && s[i] != '#')) 
 {
   i++;
 }
 return s.substring(start,i);
}

void processWeatherUpdate(int & index,String Data){
  Serial.println(Data+index);
  // Update Weather Forecast
  byte cityNumber = byte(ParseValue(index,Data).toInt())-1;
  String cityName = ParseValue(index,Data)+' ';
  int cityHigh = ParseValue(index,Data).toInt();
  int cityLow = ParseValue(index,Data).toInt();
  unsigned cityHumid = unsigned(ParseValue(index,Data).toInt());
  unsigned cityPOP = unsigned(ParseValue(index,Data).toInt());
  Cities[cityNumber].UpdateWeather(cityName,cityHigh,cityLow,cityHumid,cityPOP);
}

void packetProcessor(byte id, byte packetStatus, unsigned packetLength, String Data)
{
  // Process completed packet
  int index = 0;
  if (id == SMRTControlID)
    {
      // Is this packet for me?
      char cmd;
      if(packetStatus == 2)
	{
	  while(Data[index] != 0)
	    {
	      cmd = Data[index++];
	      if( cmd == 'w')
		{
		  processWeatherUpdate(index,Data);
		}
	    }
	  SendAck("s");
	}
      if(packetStatus == 6)
	{
	  while(Data[index] != 0)
	    {
	      cmd = Data[index++];
	      if( cmd == 'w')
		{
		  processWeatherUpdate(index,Data);
		}
	    }
	}
    }
}

void packetBuilder()
{ // builds packet from xbee input
  if (Serial1.available())
  {
    int i = 0;
     while(Serial1.available())
     {
        incoming[i++]= char(Serial1.read());
     }
     //Serial1.println("Incoming: "+String(incoming));
     incoming[i++] = NULL;
     if(packetState == WaitingforStart)
     {
       char * location =strchr(incoming,0x0f);
       if(location != NULL)
       {
         char * endL = strchr(location,0x04);
         
         while( endL != NULL && ((endL - location) < 4))
         {
             endL = strchr(endL+1,0x04);
         }
         
         int j = i-long(location-incoming);
         //Serial1.println("found: "+String(i-inbufferI));
         memcpy(inbuffer,location,j);
         inbufferI += j-1;
         if(endL)
           packetState = PacketComplete;
         else
           packetState = WaitingforEnd;
       }
     }
     else if(packetState == WaitingforEnd)
     {
       char * endL = strchr(incoming,0x04);
       if(i+inbufferI < INBUFFERMAX)
       {
         memcpy(inbuffer+inbufferI,incoming,i);
         inbufferI += i-1;
         if(endL)         
         {
             packetState = PacketComplete;
             inbufferI++;
         }
       }
       else
       {
         packetState = WaitingforStart;
       }
     }
     if(packetState == PacketComplete)
     {
       //Serial1.println("Processing: ");
       packetState = WaitingforStart;
       char* beginD = strchr(inbuffer,0x0f);
       char* endD = strchr(beginD+5,0x04);
       //Serial1.println(String(long(endD)));
       char* pendD = endD;
       while(beginD != 0 && endD!= 0)
       {
         *endD = 0;
         pendD = endD;
         byte devID = *(beginD+1);
         byte devStatus = *(beginD+2);
         unsigned pLength = *(beginD+3)<<8 + *(beginD+4);
         packet = String(beginD+5);
         //Serial.println("Processing: Start");
         packetProcessor(devID,devStatus,pLength,packet);
         //Serial.println("Processing: done");
         
         beginD = strchr(endD+1,0x0f);
         if(beginD)
           endD = strchr(beginD,0x04);
       }
       if(beginD)
       {
         packetState = WaitingforEnd;
         int j = inbufferI - (beginD-inbuffer);
         memcpy(inbuffer,beginD,j);
         inbufferI = j;
       }
       else
       {
         inbufferI = 0;
       }
     }
  //Serial.println("Made it");
  }
}

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
  delay(1000);
  // Setup the LCD
  delay(1000);
  myGLCD.InitLCD(PORTRAIT);
  myTouch.InitTouch(LANDSCAPE);
  myTouch.setPrecision(PREC_MEDIUM);
  myGLCD.clrScr();
  city = 0;
  new_city = 0;
  settings = 0;
  update_i = 0; //index to update array
  temp_i = 0; //index to temp array
  // Comms
  Serial.begin(9600);
  Serial1.begin(9600);
  pinMode(sensorPin, INPUT_PULLUP);
  // Initalize & zero rainMeter
  
  // Set up PIR sensor output pin
  pinMode(sensorPin, INPUT_PULLUP);
  //attachInterrupt(sensorPin, lcdToggle, CHANGE);
  
  // Request Weather
  Serial.println("Init: Done");
  SendRequest("f");
}

// the loop routine runs over and over again forever:
void loop() {
  // Current time for task timing.
  curTime = millis();
  // Check for incomming messages
  packetBuilder();
  // Motion Sensor
  /*
  if(curTime > rainMeterTime)
  {
   rainMeterTime += rainMeterDelay;
   SendRequest("f"); 
  }
  */
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
