
#ifndef WeatherComms_h
#define WeatherComms_h

#include "Arduino.h"
#include "String.h"
#include "WString.h"
#include <CityWeather.h>
#include <DueFlashStorage.h>



extern CityWeather Cities[];

//byte _devID = FlashStorage.read(0); // TODO Change. Add auto connect

#define devIDLocation 0
#define WaitingforStart 0
#define WaitingforEnd 1
#define PacketComplete 2
class WeatherComms{
 public:
  DueFlashStorage FlashStorage;
  // Status of string being processed
  byte devID;
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
  String lastRequest;

#define PACKET_START_BYTE 0x0F
#define PACKET_END_BYTE 0x04

#define DEVICE_URL "http://rwooster.com/wwf.tar.gz"

  void registerDevice();

  void SendAck(String data);
  void SendRequest(String data);

  String ParseValue(int &i, String s);
  void processWeatherUpdate(int & index, String Data);
  void packetProcessor(byte id, byte packetStatus, unsigned packetLength, String Data);
  void packetBuilder();
};
#endif
