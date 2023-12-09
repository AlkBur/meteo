#include "LittleFS.h"
#include "settings.h"
#include "draw.h"
// #include "ArialRoundedMTBold14.h"

void loadPropertiesFromFile()
{
	if (LittleFS.begin())
	{
		const char *msg = "Using '%s' from LittleFS\n";
		Serial.println("Attempting to read application.properties file from LittleFS.");
		File f = LittleFS.open("/settings.txt", "r");
		if (f)
		{
			Serial.println("File exists. Reading and assigning properties.");
			while (f.available())
			{
				String key = f.readStringUntil('=');
				String value = f.readStringUntil('\n');
				key.trim();
				value.trim();
				if (key == "ssid")
				{
					WIFI_SSID = value.c_str();
					Serial.printf(msg, "ssid");
				}
				else if (key == "password")
				{
					WIFI_PASS = value.c_str();
					Serial.printf(msg, "password");
				}
				else if (key == "timezone")
				{
					TIMEZONE = value.c_str();
					Serial.printf(msg, "timezone");
				}
				else if (key == "owmApiKey")
				{
					OPEN_WEATHER_MAP_API_KEY = value.c_str();
					Serial.printf(msg, "owmApiKey");
				}
				else if (key == "owmLocationId")
				{
					OPEN_WEATHER_MAP_LOCATION_ID = value.c_str();
					Serial.printf(msg, "owmLocationId");
				}
				else if (key == "locationName")
				{
					DISPLAYED_LOCATION_NAME = value.c_str();
					Serial.printf(msg, "locationName");
				}
			}
			f.close();
		}
		else
		{
			Serial.println("Does not exist.");
		}
		// int ch = WIFI_SSID.back();
		// Serial.println("last symbol:");

		Serial.println("Effective properties now as follows:");
		Serial.println("\tssid: " + WIFI_SSID + ";");
		Serial.println("\tpassword: " + WIFI_PASS + ";");
		Serial.println("\ttimezone: " + TIMEZONE + ";");
		Serial.println("\tOWM API key: " + OPEN_WEATHER_MAP_API_KEY + ";");
		Serial.println("\tOWM location id: " + OPEN_WEATHER_MAP_LOCATION_ID + ";");
		Serial.println("\tlocation name: " + DISPLAYED_LOCATION_NAME + ";");
	}
	else
	{
		Serial.println("LittleFS mount failed.");
	}
}

void connectWifi()
{
	if (WiFi.status() == WL_CONNECTED)
		return;

	Serial.printf("Connecting to WiFi %s/%s", WIFI_SSID.c_str(), WIFI_PASS.c_str());

	// WiFi.disconnect();
	WiFi.mode(WIFI_STA);
	WiFi.hostname(WIFI_HOSTNAME.c_str());
	WiFi.begin(WIFI_SSID, WIFI_PASS);
	Serial.print("Connecting to WiFi");

	int i = 0;
	while (WiFi.status() != WL_CONNECTED)
	{
		delay(500);
		if (i > 80)
			i = 1;
		drawProgress(i, "Connecting to WiFi", false);
		i += 10;
		Serial.print(".");
	}
	drawProgress(100, "Connected to WiFi", false);
	Serial.println("connected.");
	Serial.printf("Connected, IP address: %s/%s\n", WiFi.localIP().toString().c_str(), WiFi.subnetMask().toString().c_str()); // Get ip and subnet mask
	Serial.printf("Connected, MAC address: %s\n", WiFi.macAddress().c_str());												  // Get the local mac address
}

void initTime()
{
	time_t now;

	// tft.fillScreen(TFT_BLACK);
	// tft.loadFont(ArialRoundedMTBold14);

	Serial.printf("Configuring time for timezone %s\n", TIMEZONE.c_str());
	configTime(TIMEZONE.c_str(), NTP_SERVERS);
	int i = 1;
	while ((now = time(nullptr)) < NTP_MIN_VALID_EPOCH)
	{
		if (i > 9)
			i = 1;
		drawProgress(i * 10, "Updating time...", false);
		Serial.print(".");
		delay(300);
		yield();
		i++;
	}
	drawProgress(100, "Time synchronized", false);
	Serial.println();

	printf("Local time: %s", asctime(localtime(&now))); // print formated local time, same as ctime(&now)
	printf("UTC time:   %s", asctime(gmtime(&now)));	// print formated GMT/UTC time
}