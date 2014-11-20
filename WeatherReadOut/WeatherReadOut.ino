//---------------------------------------------------------------
// Pin parameters - All pins owned by devices.

//---------------------------------------------------------------


//---------------------------------------------------------------
// rainMeter - Chance of rain
int percentage = 0;

int SetServoPercent(int p)
{ 
  
}


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
    

   CityWeather() {
     // Default constructor
     cityName = "Bum Fuk, Egypt"+String(blah);
     rainChance = 50; 
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
   
//---------------------------------------------------------------
// Time management
long curTime = 0;

// Weather Update
const unsigned long updateDelay = 5000; 
long updateTime = 0;

// Motion Sensor

// Rain Meter
const unsigned long rainMeterDelay = 5000;
unsigned long rainMeterTime = rainMeterDelay;

// LCD
const unsigned long lcdDelay = 500;
unsigned long lcdTime = lcdDelay;


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
  Serial.print("\x0f\x01\x03");
  Serial.print(char(len>>8));
  Serial.print(char(len&0xff));
  Serial.print(data);
  Serial.println("\x04");
}

void SendRequest(String data)
{
  // Sends formated request
  unsigned len = data.length();
  Serial.print("\x0f\x01\x05");
  Serial.print(char(len>>8));
  Serial.print(char(len&0xff));
  Serial.print(data);
  Serial.println("\x04");
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
  
  // Comms
  Serial.begin(9600);
  
  // Initalize & zero rainMeter
  
  
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
  
  // Update LCD Task - Performs scrolling of data.
  if(lcdTime < curTime)
  {
  }
}

