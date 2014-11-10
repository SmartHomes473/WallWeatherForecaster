#include "CityWeather.h"

CityWeather::CityWeather() {
	cityIndex = 0;
	rainChance = 50;
}

CityWeather::CityWeather(char* name, int hi, int low, unsigned humidity, unsigned rain)
{
	cityName = name;
	rainChance = rain%101;
}

char* CityWeather::getCityName()
{
	return cityName;
}

int CityWeather::getRainChance()
{
	return rainChance; 
}

char* CityWeather::getCondition() {
	return condition;
}

int CityWeather::getHumidity()
{
	return humidity;
}

int CityWeather::getLow() {
	return low;
}

int CityWeather::getHigh() {
	return high;
}