CityWeather::CityWeather() {
	cityIndex = 0;
	rainChance = 50;
}

CityWeather::CityWeather(String name, int hi, int low, unsigned humidity, unsigned rain)
{
	cityName = name;
	rainChance = rain%101;
}

String CityWeather::getCityName()
{
	return cityName;
}

int CityWeather::getRainChance()
{
	return rainChance; 
}

String CityWeather::getCondition() {
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