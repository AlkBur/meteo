#include "data.h"
#include "draw.h"

void updateData(bool UpdateProgress, bool onlyUpdate)
{
	if (UpdateProgress)
		drawProgress(0, "Updating conditions...", onlyUpdate);

	time_t now = time(nullptr);
	drawProgress(50, "Updating conditions...", onlyUpdate);
	OpenWeatherMapCurrent *currentWeatherClient = new OpenWeatherMapCurrent();
	currentWeatherClient->setMetric(true);
	currentWeatherClient->setLanguage(OPEN_WEATHER_MAP_LANGUAGE);
	currentWeatherClient->updateCurrentById(&currentWeather, OPEN_WEATHER_MAP_API_KEY, OPEN_WEATHER_MAP_LOCATION_ID);
	delete currentWeatherClient;
	currentWeatherClient = nullptr;

	drawProgress(70, "Updating forecasts...", onlyUpdate);
	OpenWeatherMapForecast *forecastClient = new OpenWeatherMapForecast();
	forecastClient->setMetric(true);
	forecastClient->setLanguage(OPEN_WEATHER_MAP_LANGUAGE);
	uint8_t allowedHours[] = {12, 0};
	forecastClient->setAllowedHours(allowedHours, sizeof(allowedHours));
	forecastClient->updateForecastsById(forecasts, OPEN_WEATHER_MAP_API_KEY, OPEN_WEATHER_MAP_LOCATION_ID, MAX_FORECASTS);
	delete forecastClient;
	forecastClient = nullptr;

	drawProgress(80, "Updating astronomy...", onlyUpdate);

	SunMoonCalc *smCalc = new SunMoonCalc(now, currentWeather.lat, currentWeather.lon);
	moonData = smCalc->calculateSunAndMoonData().moon;
	delete smCalc;
	smCalc = nullptr;
	Serial.printf("Free mem: %d\n", ESP.getFreeHeap());

	delay(1000);
}

// converts the dBm to a range between 0 and 100%
int8_t getWifiQuality()
{
	int32_t dbm = WiFi.RSSI();
	if (dbm <= -100)
	{
		return 0;
	}
	else if (dbm >= -50)
	{
		return 100;
	}
	else
	{
		return 2 * (dbm + 100);
	}
}

String getMeteoconIcon(String iconText)
{
	if (iconText == "01d" || iconText == "01n")
		return "sunny";
	if (iconText == "02d" || iconText == "02n")
		return "partlysunny";
	if (iconText == "03d" || iconText == "03n")
		return "partlycloudy";
	if (iconText == "04d" || iconText == "04n")
		return "mostlycloudy";
	if (iconText == "09d" || iconText == "09n")
		return "rain";
	if (iconText == "10d" || iconText == "10n")
		return "rain";
	if (iconText == "11d" || iconText == "11n")
		return "tstorms";
	if (iconText == "13d" || iconText == "13n")
		return "snow";
	if (iconText == "50d" || iconText == "50n")
		return "fog";
	return "";
}