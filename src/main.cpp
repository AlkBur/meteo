// /*****************************
//  * Important: see settings.h to configure your settings!!!
//  * ***************************/
#include "settings.h"
#include "data.h"
#include "draw.h"
// #include <Adafruit_ST7735.h>
// #include <Adafruit_ST7789.h>
// #include <Adafruit_GFX.h>

TFT_eSPI tft = TFT_eSPI(); // создаем экземпляр объекта TFT_eSPI

#define TFT_CS 15 // Hallowing display control pins: chip select
#define TFT_RST 0 // Display reset
#define TFT_DC 5  // Display data/command select
#define TFT_SCLK 14
#define TFT_MOSI 13

long lastDownloadUpdate = 0;
const int UPDATE_INTERVAL_SECS = 10 * 60; // Update every 10 minutes

// TFT_eSprite img = TFT_eSprite(&tft);
void setup(void)
{
  Serial.begin(115200);
  Serial.println("Initializing screen...");
  tft.init();
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);
  Serial.println("Reading settings...");
  loadPropertiesFromFile();
  Serial.println("Initializing WiFi...");
  connectWifi();
  initTime();
  updateData(false, false);
  long lastDownloadUpdate = millis();
  tft.fillScreen(TFT_BLACK);
}

void loop()
{
  drawBlock1();
  drawBlock2();
  drawBlock3();
  drawBlock4();

  if (millis() - lastDownloadUpdate > 1000 * UPDATE_INTERVAL_SECS)
  {
    updateData(true, true);
    lastDownloadUpdate = millis();
  }
  delay(200);
}