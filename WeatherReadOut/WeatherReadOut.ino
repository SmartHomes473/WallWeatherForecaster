/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.
 
  This example code is in the public domain.
 */
#include <Servo.h>
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

//---------------------------------------------------------------
// Status LEDc
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
   servo.write((97-percent) *1.8);
}


//---------------------------------------------------------------
// LCD - Displays temp, humidity etc.
char blah = 'a';
class CityWeather{    
  public:
    unsigned cityIndex;
    unsigned dataIndex;
    int rainChance;
    String cityName;
    String data; // TODO: Remove
    String highTemp;
    String lowTemp;
    String humidityPercent;
    

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
       UpdateWeather(name,hi,low,humidity,rain);
   }
   
   void UpdateWeather(String name, int hi, int low, unsigned humidity, unsigned rain)
   {
       cityName = name;
       highTemp = String(hi);
       lowTemp = String(low);
       humidityPercent = String(humidity);
       data = "High: "+String(hi)+degree+"F Low: "+String(low)+degree+"F "+"Humidity: "+String(humidity)+"% ";
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

// Update
const unsigned long updateDelay = 5000; 
long updateTime = 0;
// Status LED
const unsigned long statLedDelay = 1000;
long statLedTime = statLedDelay;

// Rain Meter
const unsigned long rainMeterDelay = 5000;
unsigned long rainMeterTime = rainMeterDelay;

// LCD
const unsigned long lcdDelay = 500;
unsigned long lcdTime = lcdDelay;

// Switch
const unsigned long switchDelay = 1000;
unsigned long switchTime = switchDelay;

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
  //Serial.println("Sending ACK: "+data);
  unsigned len = data.length();
  Serial.print("\x0f\x01\x03");
  Serial.print(char(len>>8));
  Serial.print(char(len&0xff));
  Serial.print(data);
  Serial.println("\x04");
}

void SendRequest(String data)
{
  //Serial.println("Sending REQ: "+data);
  unsigned len = data.length();
  //Serial.println("Sending REQ: "+String((len>>8)));
  //Serial.println("Sending REQ: "+String(len&0xff));
  Serial.print("\x0f\x01\x05");
  Serial.print(char(len>>8));
  Serial.print(char(len&0xff));
  Serial.print(data);
  Serial.println("\x04");
}

String ParseValue(int &i,String s)
{
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
  byte cityNumber = byte(ParseValue(index,Data).toInt())-1;
  //Serial.println("number: "+String(cityNumber));
  String cityName = ParseValue(index,Data)+' ';
  //Serial.println("name: "+String(cityName));
  int cityHigh = ParseValue(index,Data).toInt();
  //Serial.println("H: "+String(cityHigh));
  int cityLow = ParseValue(index,Data).toInt();
  //Serial.println("L: "+String(cityLow));
  unsigned cityHumid = unsigned(ParseValue(index,Data).toInt());
  //Serial.println("Hu: "+String(cityHumid));
  unsigned cityPOP = unsigned(ParseValue(index,Data).toInt());
  Cities[cityNumber].UpdateWeather(cityName,cityHigh,cityLow,cityHumid,cityPOP);
}

void packetProcessor(byte id, byte packetStatus, unsigned packetLength, String Data)
{ // reads data
  int index = 0;
  //Serial.println("Data: "+Data);
  if (id == SMRTControlID)
    {
      //Serial.println(packetStatus);
      char cmd;
      if(packetStatus == 2)
	{
	  while(Data[index] != 0)
	    {
	      //Serial.println(Data[index]);
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
  if (Serial.available())
  {
    int i = 0;
     while(Serial.available())
     {
        incoming[i++]= char(Serial.read());
     }
     //Serial.println("Incoming: "+String(incoming));
     incoming[i++] = NULL;
     if(packetState == WaitingforStart)
     {
       char * location =strchr(incoming,0x0f);
       if(location != NULL)
       {
         char * endL = strchr(location,0x04);
         int j = i-long(location-incoming);
         //Serial.println("found: "+String(i-inbufferI));
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
     /*for(int j = 0; j < 5; j++)
       Serial.println("found: "+String(j)+" "+String(*(inbuffer+j),HEX));
     for(int j = 5; j < inbufferI; j++)
     Serial.println("found: "+String(j)+" "+String(char(*(inbuffer+j)),HEX));*/
     if(packetState == PacketComplete)
     {
       //Serial.println("Processing: ");
       packetState = WaitingforStart;
       char* beginD = strchr(inbuffer,0x0f);
       char* endD = strchr(beginD+5,0x04);
       //Serial.println(String(long(endD)));
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

//---------------------------------------------------------------
// the setup routine runs once when you press reset:
void setup() {
  // LCD
  
  // Serial for debugging
  Serial.begin(9600);
  //SerialSerial.begin(9600);
  /*
  if (Ethernet.begin(mac) == 0)
      Serial.println("Failed to configure Ethernet using DHCP");
  else
      Serial.println(Ethernet.localIP());*/
  
  // initialize the digital pin as an output.
  pinMode(statLedPin, OUTPUT);
  
  // Initalize Servo for rainMeter
  rainMeter.attach(rainMeterPin,544,2370);
  
  // Get current city
  switchOldReading = analogRead(citySwitchPin);
  
  // Output LCD stuff
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
  if(switchTime < curTime)
  {
    switchTime = curTime + switchDelay;
    switchCurReading = analogRead(citySwitchPin)/205;
    //Serial.println(analogRead(citySwitchPin)/205);
    if(switchCurReading != switchOldReading)
    {
      //Serial.println("Chance: "+int(Cities[switchCurReading].rainChance));
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
  }
}

