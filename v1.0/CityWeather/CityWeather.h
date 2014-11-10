
//---------------------------------------------------------------
// LCD - Displays temp, humidity etc.
class CityWeather{    
  public:
    int cityIndex;
	int low;
	int high;
	char* condition;
    int rainChance;
    char* cityName;
	int humidity;

   CityWeather();
   CityWeather(char* name, int hi, int low, unsigned humidity, unsigned rain);
   
   char* getCityName();
   int getRainChance();
   char* getCondition();
   int getHumidity();
   int getLow();
   int getHigh();
};