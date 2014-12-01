#ifndef CityWeather_h
#define CityWeather_h

#include "String.h"
#include "WString.h"

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
    

   CityWeather();
   CityWeather(String name, int hi, int low, unsigned humidity, unsigned rain);
   
   void UpdateWeather(String name,String cond, int hi, int low, unsigned humidity, unsigned rain);
};

#endif
