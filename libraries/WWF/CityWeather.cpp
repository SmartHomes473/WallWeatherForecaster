#include "CityWeather.h"
#include "Arduino.h"

char blah = 'a';

CityWeather::CityWeather() {
 // Default constructor
 cityName = "Middle of BF, Egypt"+String(blah);
 rainChance = 50; 
 blah++;
 highTemp = "100";
 lowTemp = "-32";
 humidityPercent = "56";
 condition = "To hot to handle";
}

CityWeather::CityWeather(String name, int hi, int low, unsigned humidity, unsigned rain)
{
  UpdateWeather(name," ",hi,low,humidity,rain);
}

void CityWeather::UpdateWeather(String name,String cond, int hi, int low, unsigned humidity, unsigned rain)
{
   cityName = name;
   condition = cond;
   highTemp = String(hi);
   lowTemp = String(low);
   humidityPercent = String(humidity);
   rainChance = rain%101;
}
