#include "CityWeather.h"
#include "Arduino.h"

char blah = 'a';

CityWeather::CityWeather() {
 // Default constructor
 cityName = "Bum Fuk, Egypt"+String(blah);
 rainChance = 50; 
 blah++;
 highTemp = "100";
 lowTemp = "-32";
 humidityPercent = "56";
 condition = "Cold as Shit";
}

CityWeather::CityWeather(String name, int hi, int low, unsigned humidity, unsigned rain)
{
   UpdateWeather(name,hi,low,humidity,rain);
}

void CityWeather::UpdateWeather(String name, int hi, int low, unsigned humidity, unsigned rain)
{
   cityName = name;
   highTemp = String(hi);
   lowTemp = String(low);
   humidityPercent = String(humidity);
   rainChance = rain%101;
}