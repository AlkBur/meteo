#ifndef DATA_H
#define DATA_H

#include <OpenWeatherMapCurrent.h>
#include <OpenWeatherMapForecast.h>
#include <SunMoonCalc.h>
#include "settings.h"

#define MAX_FORECASTS 12

inline OpenWeatherMapCurrentData currentWeather;
inline OpenWeatherMapForecastData forecasts[MAX_FORECASTS];
inline SunMoonCalc::Moon moonData;

void updateData(bool UpdateProgress, bool onlyUpdate);
int8_t getWifiQuality();
String getMeteoconIcon(String iconText);

#endif