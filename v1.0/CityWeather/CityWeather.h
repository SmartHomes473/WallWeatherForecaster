//---------------------------------------------------------------
// LCD - Displays temp, humidity etc.
class CityWeather{    
  public:
    int cityIndex;
	int low;
	int high;
	String condition;
    int rainChance;
    String cityName;

   CityWeather();
   CityWeather(String name, int hi, int low, unsigned humidity, unsigned rain);
   
   String getCityName();
   int getRainChance();
   String getCondition();
   int getHumidity();
   int getLow();
   int getHigh();
};