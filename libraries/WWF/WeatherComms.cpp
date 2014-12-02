#include <WeatherComms.h>
extern int city;
extern String hour,minute, meridian;
extern int temp_i, update_i;

void WeatherComms::registerDevice() {
   devID = FlashStorage.read(0)+4;
   Serial.println("ID: " + String(devID));
   // If all bits are 1 the flash hasn't been written and therefore the device hasn't been registered
   if (devID == 255)
   {
	/* Register device */
        // Prepare registration according to comm definition
	uint8_t message[39] = {PACKET_START_BYTE, 0x00, 0x00, 0x00, 0x11};
	memcpy(message + 5, DEVICE_URL, 33);
	message[38] = PACKET_END_BYTE;
        
        // Send register device command to SMRTControl
        for(int i = 0; i < 38; i++) {
          Serial1.print(message[i]);
        }
        Serial1.println(message[38]);
        
        // Write 0 to the flash storage to show that registration has been attempted
	FlashStorage.write(devIDLocation, 0); 
        Serial.println("Zeroed ID: " + String(FlashStorage.read(devIDLocation)));
  } 
}

void WeatherComms::SendAck(String data)
{
  // Sends formated message of acknowledge
  unsigned len = data.length();
  Serial1.print("\x0f"+String(char(devID))+"\x03");
  Serial1.print(char(len>>8));
  Serial1.print(char(len&0xff));
  Serial1.print(data);
  Serial1.println("\x04");
}

void WeatherComms::SendRequest(String data)
{
  // Sends formated request
  lastRequest = data;
  unsigned len = data.length();
  Serial1.print("\x0f"+String(char(devID))+"\x05");
  Serial1.print(char(len>>8));
  Serial1.print(char(len&0xff));
  Serial1.print(data);
  Serial1.println("\x04");
}

String WeatherComms::ParseValue(int &i,String s)
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

void WeatherComms::processTime(int &index, String Data){
  // Inserts data for time:
  // Data: hour;minute;meridian#
  hour = ParseValue(index,Data);
  minute = ParseValue(index,Data);
  meridian = ParseValue(index,Data);
}

void WeatherComms::processSettings(int &index, String Data){
  // Inserts data for settings change.
  // Data: updateInterval;temperatureScale#
  
  update_i = (ParseValue(index,Data).toInt() - 1)%24;
  temp_i = ParseValue(index,Data).toInt()%3;
  Serial.println('U: '+String(update_i)+'T:'+String(temp_i));
}

void WeatherComms::processWeatherUpdate(int & index,String Data){
  Serial.println("Getting data starting at: "+String(index));
  Serial.println(Data.substring(index));
  // Update Weather Forecast
  byte cityNumber = byte(ParseValue(index,Data).toInt())-1;
  String cityName = ParseValue(index,Data)+' ';
  String condition = ParseValue(index,Data);
  int cityHigh = ParseValue(index,Data).toInt();
  int cityLow = ParseValue(index,Data).toInt();
  unsigned cityHumid = unsigned(ParseValue(index,Data).toInt());
  unsigned cityPOP = unsigned(ParseValue(index,Data).toInt());
  Cities[cityNumber].UpdateWeather(cityName,condition,cityHigh,cityLow,cityHumid,cityPOP);
}

void WeatherComms::packetProcessor(byte id, byte packetStatus, unsigned packetLength, String Data)
{ 
  // Process completed packet
  Serial.println("Packet Length: "+String(packetLength)+" Actual: "+Data.length());
  if( packetLength !=  Data.length())
  {
     // Drop packet, Resend if data was a request
     if(packetStatus == 6)
       SendRequest(lastRequest);
     return;
  }
  int index = 0;
  Serial.println(devID);
  
  // Overwrite the current devID with the new registered ID if the special registration
  // reply packet arrives
  if (packetStatus == 0 && devID == 0) {
    FlashStorage.write(devIDLocation, id);
  }
  
  if (id == devID)
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
	      if( cmd == 's')
		{
		  processSettings(index,Data);
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
	      if( cmd == 't')
		{
		  processTime(index,Data);
		}
	    }
	}
    }
}

void WeatherComms::packetBuilder()
{ // builds packet from xbee input
  if (Serial1.available())
  {
    int i = 0;
    Serial.print("Recieving: ");
     while(Serial1.available())
     {
        // Copy next character from serial buffer, print to serial monitor
        incoming[i]= char(Serial1.read());
        Serial.print(incoming[i++]);
     }
     Serial.println(" Done");
     //Serial1.println("Incoming: "+String(incoming));
     incoming[i++] = NULL;
     if(packetState == WaitingforStart)
     {
       char * location =(char*)memchr(incoming,0x0f,i);
       if(location != NULL)
       {
	 char * endL = (char*)memchr(location,0x04,i);
         while( endL != NULL && ((endL - location) < 4))
         {
	   endL = (char*)memchr((endL+1),0x04,i-(endL-location));
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
       char * endL = (char*)memchr(incoming,0x04,i);
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
         Serial.println("Dev ID: "+String(devID));
         byte devStatus = *(beginD+2);
         Serial.println("Status: "+String(devID));
         unsigned pLength = ((unsigned)(*(beginD+4)));
         pLength += ((unsigned)*(beginD+3))<<8;
         Serial.println("Length: "+String(pLength));
         Serial.println("Length: "+String(unsigned(*(beginD+3)))+" "+String(unsigned(*(beginD+4))));
         packet = String(beginD+5);
         //Serial.println("Processing: Start");
         packetProcessor(devID,devStatus,pLength,packet);
	 city = -1;
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
  }
}
