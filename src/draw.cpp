#include <TFT_eSPI.h>
#include "bmp.h"
#include "draw.h"
#include "data.h"
#include "ArialRoundedMTBold14.h"
#include "ArialRoundedMTBold36.h"
#include "MoonphaseRegular36.h"

#include "logo.h"
#include "icons.h"
#include "mini_icons.h"
#include "moon_icons.h"

#define ArialRoundedMTBold14 &ArialRoundedMtBold6pt7b
#define ArialRoundedMTBold36 &ArialRoundedMtBold20pt7b
#define MoonphaseRegular36 &moonphase20pt7b

#define GFXFF 1

TFT_eSprite img = TFT_eSprite(&tft);

template <size_t N>
void drawBMP(int x, int y, const uint8_t (&bmp)[N])
{
	const size_t n = N;
	Serial.printf("Test count = %d\n", n);
	if (n < 4)
		return;

	int delta = 2;
	size_t buffidx = 0;
	int w = (int)pgm_read_word(&bmp[buffidx]);
	buffidx += delta;
	int h = (int)pgm_read_word(&bmp[buffidx]);
	buffidx += delta;

	int row, col;
	Serial.printf("n = %d; h=%d; w=%d\n", n, h, w);
	// uint8_t sdbuffer[3 * 80]; // pixel buffer (R+G+B per pixel)

	delay(20);

	for (row = 0; row < h; row++)
	{ // For each scanline...
		for (col = 0; col < w; col++)
		{ // For each pixel...
			if ((buffidx + delta) < n && (col + x) <= img.width() && (row + y) <= img.height())
			{
				//	Serial.printf("colorw=%d\n", bmp[buffidx]);
				//  		// To read from Flash Memory, pgm_read_XXX is required.
				//  		// Since image is stored as uint16_t, pgm_read_word is used as it uses 16bit address
				//  		if (buffidx < n && (col + x) <= img.width() && (row + y) <= img.height())
				uint16_t color = (uint16_t)pgm_read_word(&bmp[buffidx]);
				color = (color >> 8) | (color << 8);
				img.drawPixel(col + x, row + y, (uint32_t)color);
			}
			buffidx += delta;
		} // end pixel
	}
	delay(200);
}

// Progress bar helper
void drawProgress(uint8_t percentage, String text, bool onlyUpdate)
{
	if (percentage == 0 && !onlyUpdate)
	{
		tft.fillScreen(TFT_BLACK);
		img.createSprite(80, 80);
		img.setColorDepth(8);
		// drawJpeg(&img, "/logo.jpg", 0, 0);
		Serial.printf("Logo count = %d\n", sizeof(Logo) / sizeof(Logo[0]));
		drawBMP(0, 0, Logo);
		img.pushSprite(80, 5);
		img.deleteSprite();

		tft.setFreeFont(ArialRoundedMTBold14);
		tft.setTextColor(TFT_WHITE);
		tft.setCursor(120, 90);
		tft.drawCentreString("Weather forecast", 120, 90, GFXFF);
	}

	if (onlyUpdate)
		img.createSprite(240, 80);
	else
		img.createSprite(240, 45);

	img.setColorDepth(8);
	img.fillSprite(TFT_BLACK);

	img.setFreeFont(ArialRoundedMTBold14);
	img.setTextColor(TFT_YELLOW);
	int yDelta = 0;
	if (onlyUpdate)
		yDelta = 20;

	img.drawCentreString(text, img.width() / 2, 0 + yDelta, GFXFF);

	img.drawRect(10, 30 + yDelta, 240 - 20, 15, TFT_WHITE);

	img.fillRect(12, 32 + yDelta, 216 * percentage / 100, 11, TFT_BLUE);
	if (onlyUpdate)
		img.pushSprite(0, 80);
	else
		img.pushSprite(0, 320 / 2 - img.height() / 2);

	img.deleteSprite();
}

void drawBlock1()
{
	char time_str[11];
	time_t now = time(nullptr);
	struct tm *timeinfo = localtime(&now);
	String date = WDAY_NAMES[timeinfo->tm_wday] + " " + MONTH_NAMES[timeinfo->tm_mon] + " " + String(timeinfo->tm_mday) + " " + String(1900 + timeinfo->tm_year);

	img.createSprite(240, 320 / 4); // 80 height
	img.setColorDepth(8);

	// block 1
	img.fillSprite(TFT_BLACK);
	img.setTextColor(TFT_WHITE);

	img.setFreeFont(ArialRoundedMTBold14);
	img.drawCentreString(date, 120, 6, GFXFF);
	img.setFreeFont(ArialRoundedMTBold36);
	// sprintf(time_str, "%02d:%02d\n", timeinfo->tm_hour, timeinfo->tm_min); //hh:mm
	sprintf(time_str, "%02d:%02d:%02d\n", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec); // hh:mm:ss
	img.drawCentreString(time_str, 120, 30, GFXFF);

	int8_t quality = getWifiQuality();

	img.setFreeFont(LOAD_GLCD);
	img.drawRightString(String(quality) + "%", 228, 11, 1);
	for (int8_t i = 0; i < 4; i++)
	{
		for (int8_t j = 0; j < 2 * (i + 1); j++)
		{
			if (quality > i * 25 || j == 0)
			{
				img.drawPixel(230 + 2 * i, 18 - j, TFT_WHITE);
			}
		}
	}

	img.pushSprite(0, 0 * 80);
}

void drawBlock2()
{
	// // block 2
	int y = 12;
	img.fillSprite(TFT_BLACK);
	String iconText = currentWeather.icon;
	if (iconText == "01d" || iconText == "01n")
		drawBMP(15, y, sunny);
	else if (iconText == "02d" || iconText == "02n")
		drawBMP(15, y, partlysunny);
	else if (iconText == "03d" || iconText == "03n")
		drawBMP(15, y, partlycloudy);
	else if (iconText == "04d" || iconText == "04n")
		drawBMP(15, y, mostlycloudy);
	else if (iconText == "09d" || iconText == "09n")
		drawBMP(15, y, rain);
	else if (iconText == "10d" || iconText == "10n")
		drawBMP(15, y, rain);
	else if (iconText == "11d" || iconText == "11n")
		drawBMP(15, y, tstorms);
	else if (iconText == "13d" || iconText == "13n")
		drawBMP(15, y, snow);
	else if (iconText == "50d" || iconText == "50n")
		drawBMP(15, y, fog);
	else
		drawBMP(15, y, unknown);

	img.setFreeFont(ArialRoundedMTBold14);
	img.setTextColor(TFT_BLUE);
	img.drawRightString(DISPLAYED_LOCATION_NAME, 220, 0, GFXFF);

	img.setFreeFont(ArialRoundedMTBold36);
	img.setTextColor(TFT_WHITE);
	img.drawRightString(String(currentWeather.temp, 1) + "°C", 220, 20, GFXFF);

	img.setFreeFont(ArialRoundedMTBold14);
	img.setTextColor(TFT_YELLOW);
	img.drawRightString(currentWeather.description, 220, 65, GFXFF);

	img.pushSprite(0, 1 * 80);
}

// helper for the forecast columns
void drawForecastDetail(uint16_t x, uint16_t y, uint8_t dayIndex)
{
	img.setFreeFont(ArialRoundedMTBold14);
	img.setTextColor(TFT_YELLOW);
	time_t time = forecasts[dayIndex].observationTime;
	Serial.print("time: ");
	Serial.print(time);
	Serial.print("\n");

	struct tm *timeinfo = localtime(&time);
	img.drawCentreString(WDAY_NAMES[timeinfo->tm_wday] + " " + String(timeinfo->tm_hour) + ":00", x + 25, y, GFXFF);

	img.setTextColor(TFT_WHITE);
	img.drawCentreString(String(forecasts[dayIndex].temp, 1) + "°C", x + 25, y + 15, GFXFF);

	//   gfx.drawPalettedBitmapFromPgm(x, y + 15, getMiniMeteoconIconFromProgmem(forecasts[dayIndex].icon));
	String iconText = forecasts[dayIndex].icon;
	int xIcon = x + 12, yIcon = y + 35;
	if (iconText == "01d" || iconText == "01n")
		drawBMP(xIcon, yIcon, minisunny);
	else if (iconText == "02d" || iconText == "02n")
		drawBMP(xIcon, yIcon, minipartlysunny);
	else if (iconText == "03d" || iconText == "03n")
		drawBMP(xIcon, yIcon, minipartlycloudy);
	else if (iconText == "04d" || iconText == "04n")
		drawBMP(xIcon, yIcon, minimostlycloudy);
	else if (iconText == "09d" || iconText == "09n")
		drawBMP(xIcon, yIcon, minirain);
	else if (iconText == "10d" || iconText == "10n")
		drawBMP(xIcon, yIcon, minirain);
	else if (iconText == "11d" || iconText == "11n")
		drawBMP(xIcon, yIcon, minitstorms);
	else if (iconText == "13d" || iconText == "13n")
		drawBMP(xIcon, yIcon, minisnow);
	else if (iconText == "50d" || iconText == "50n")
		drawBMP(xIcon, yIcon, minifog);
	else
		drawBMP(xIcon, yIcon, miniunknown);

	img.setTextColor(TFT_BLUE);
	img.drawCentreString(String(forecasts[dayIndex].rain, 1) + "mm", (uint32_t)x + 25, (uint32_t)y + 70, GFXFF);
}

void drawBlock3()
{
	// // block 3
	img.fillSprite(TFT_BLACK);

	drawForecastDetail(10, 0, 0);
	drawForecastDetail(95, 0, 1);
	drawForecastDetail(180, 0, 2);

	img.pushSprite(0, 2 * 80 + 10);
	img.deleteSprite();

	img.createSprite(240, 10); // 80-10 height
	img.pushSprite(0, 2 * 80);
	img.deleteSprite();
}

void determineMoonIcon()
{
	// 	char moonIcon;
	// 	// index in range of 0..14
	int index = round(moonData.illumination * 12);
	if (index > 12)
		index = 12;
	else if (index < 1)
		index = 23;
	Serial.printf("Moon illumination: %f -> moon icon index: %d\n", moonData.illumination, index);
	if (index < 23 && index < 11 && currentWeather.lat > 0)
	{
		index = 23 - index;
		Serial.printf("Moon illumination: %f -> moon icon index: %d\n", moonData.illumination, index);
	}

	switch (index)
	{
	case 0:
		drawBMP(90, 10, moonphase_L0);
		break;
	case 1:
		drawBMP(90, 10, moonphase_L1);
		break;
	case 2:
		drawBMP(90, 10, moonphase_L2);
		break;
	case 3:
		drawBMP(90, 10, moonphase_L3);
		break;
	case 4:
		drawBMP(90, 10, moonphase_L4);
		break;
	case 5:
		drawBMP(90, 10, moonphase_L5);
		break;
	case 6:
		drawBMP(90, 10, moonphase_L6);
		break;
	case 7:
		drawBMP(90, 10, moonphase_L7);
		break;
	case 8:
		drawBMP(90, 10, moonphase_L8);
		break;
	case 9:
		drawBMP(90, 10, moonphase_L9);
		break;
	case 10:
		drawBMP(90, 10, moonphase_L10);
		break;
	case 11:
		drawBMP(90, 10, moonphase_L11);
		break;
	case 12:
		drawBMP(90, 10, moonphase_L12);
		break;
	case 13:
		drawBMP(90, 10, moonphase_L13);
		break;
	case 14:
		drawBMP(90, 10, moonphase_L14);
		break;
	case 15:
		drawBMP(90, 10, moonphase_L15);
		break;
	case 16:
		drawBMP(90, 10, moonphase_L16);
		break;
	case 17:
		drawBMP(90, 10, moonphase_L17);
		break;
	case 18:
		drawBMP(90, 10, moonphase_L18);
		break;
	case 19:
		drawBMP(90, 10, moonphase_L19);
		break;
	case 20:
		drawBMP(90, 10, moonphase_L20);
		break;
	case 21:
		drawBMP(90, 10, moonphase_L21);
		break;
	case 22:
		drawBMP(90, 10, moonphase_L22);
		break;

	default:
		drawBMP(90, 10, moonphase_L23);
		break;
	}

	// 	if (moonData.phase.index > 4)
	// 	{
	// 		// waning (4 = full moon)
	// 		moonIcon = currentWeather.lat > 0 ? MOON_ICONS_NORTH_WANING[index] : MOON_ICONS_SOUTH_WANING[index];
	// 	}
	// 	else
	// 	{
	// 		// waxing
	// 		moonIcon = currentWeather.lat > 0 ? MOON_ICONS_NORTH_WAXING[index] : MOON_ICONS_SOUTH_WAXING[index];
	// 	}
	// 	return moonIcon;
}

String getTime(time_t *timestamp)
{
	struct tm *timeInfo = localtime(timestamp);

	char buf[9]; // "12:34 pm\0"
	char ampm[3];
	ampm[0] = '\0'; // Ready for 24hr clock
	uint8_t hour = timeInfo->tm_hour;

	sprintf(buf, "%02d:%02d %s", hour, timeInfo->tm_min, ampm);

	return String(buf);
}

void drawBlock4()
{

	img.createSprite(240, 70); // 80-10 height
	img.setColorDepth(8);
	// // block 4
	img.fillSprite(TFT_BLACK);

	img.setFreeFont(MoonphaseRegular36);
	img.setTextColor(TFT_WHITE);

	// img.drawCentreString(String((char)(97 + (moonData.illumination * 26))), 120, 11, GFXFF);
	determineMoonIcon();

	img.setFreeFont(ArialRoundedMTBold14);
	img.setTextColor(TFT_YELLOW);
	img.drawCentreString(MOON_PHASES[moonData.phase.index], 120, 10, GFXFF);

	img.drawString(SUN_MOON_TEXT[0], 5, 10);

	img.setTextColor(TFT_WHITE);
	time_t time = currentWeather.sunrise;
	img.drawString(SUN_MOON_TEXT[1] + ":", 5, 30);
	img.drawString(getTime(&time), 45, 30);
	time = currentWeather.sunset;
	img.drawString(SUN_MOON_TEXT[2] + ":", 5, 50);
	img.drawString(getTime(&time), 45, 50);

	img.setTextColor(TFT_YELLOW);
	img.drawRightString(SUN_MOON_TEXT[3], 235, 10, GFXFF);

	img.setTextColor(TFT_WHITE);
	img.drawRightString(SUN_MOON_TEXT[4] + ":", 190, 30, GFXFF);
	img.drawRightString(String(moonData.age, 1) + "d", 235, 30, GFXFF);
	img.drawRightString(SUN_MOON_TEXT[5] + ":", 190, 50, GFXFF);
	img.drawRightString(String(moonData.illumination * 100, 0) + "%", 235, 50, GFXFF);

	//   float lunarMonth = 29.53;
	//   // approximate moon age
	//   gfx.drawString(190, 276, SUN_MOON_TEXT[4] + ":");
	//   gfx.drawString(235, 276, String(moonData.age, 1) + "d");
	//   gfx.drawString(190, 291, SUN_MOON_TEXT[5] + ":");
	//   gfx.drawString(235, 291, String(moonData.illumination * 100, 0) + "%");

	img.pushSprite(0, 3 * 80 + 10);

	img.deleteSprite();
}