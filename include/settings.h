#ifndef SETTINGS_H
#define SETTINGS_H

#include <TFT_eSPI.h>
#include <SPI.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>

	/***************************
	 * Start Settings
	 **************************/
	inline String WIFI_SSID = "yourssid";
inline String WIFI_PASS = "yourpassw0rd";
inline String WIFI_HOSTNAME = "meteostation";

// weawer
inline String OPEN_WEATHER_MAP_API_KEY = "";
inline String OPEN_WEATHER_MAP_LOCATION_ID = "";
inline String DISPLAYED_LOCATION_NAME = "";
inline const String OPEN_WEATHER_MAP_LANGUAGE = "en";

// Time
inline String TIMEZONE = "";
#define NTP_SERVERS "time.nist.gov", "pool.ntp.org"
// August 1st, 2018
#define NTP_MIN_VALID_EPOCH 1533081600

extern TFT_eSPI tft;
// Pins for the ST7789
// TFT_CS D8(15)
// TFT_DC D1(5)
// TFT_RST D3(0)
// TFT_SCLK D5(14)
// TFT_MISO D6(12)
// TFT_MOSI D7(13)
// TFT_LED VBUS(-)
// TFT_WIDTH=320
// TFT_HEIGHT=240

// Functions
void loadPropertiesFromFile();
void connectWifi();
void initTime();

const String WDAY_NAMES[] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};
const String MONTH_NAMES[] = {"JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"};
const String SUN_MOON_TEXT[] = {"Sun", "Rise", "Set", "Moon", "Age", "Illum"};
const String MOON_PHASES[] = {"New Moon", "Waxing Crescent", "First Quarter", "Waxing Gibbous",
							  "Full Moon", "Waning Gibbous", "Third quarter", "Waning Crescent"};

/***************************
 * End Settings
 **************************/
#endif