#include "bmp.h"
#include "LittleFS.h"

uint32_t color = 0;

uint16_t read16(File &f)
{
	uint16_t result;
	((uint8_t *)&result)[0] = f.read(); // LSB
	((uint8_t *)&result)[1] = f.read(); // MSB
	return result;
}

uint32_t read32(File &f)
{
	uint32_t result;
	((uint8_t *)&result)[0] = f.read(); // LSB
	((uint8_t *)&result)[1] = f.read();
	((uint8_t *)&result)[2] = f.read();
	((uint8_t *)&result)[3] = f.read(); // MSB
	return result;
}

void drawBmpFromFile(TFT_eSprite *img, String filename, uint8_t x, uint8_t y)
{
	Serial.println("In drawBmpFromFile");
	if ((x >= img->width()) || (y >= img->height()))
		return;

	uint8_t bitsPerPixel = img->getColorDepth();

	File bmpFile;
	int bmpWidth, bmpHeight;			// W+H in pixels
	uint8_t bmpDepth;					// Bit depth (currently must be 24)
	uint32_t bmpImageoffset;			// Start of image data in file
	uint32_t rowSize;					// Not always = bmpWidth; may have padding
	uint8_t sdbuffer[3 * 20];			// pixel buffer (R+G+B per pixel)
	uint8_t buffidx = sizeof(sdbuffer); // Current position in sdbuffer
	boolean goodBmp = false;			// Set to true on valid header parse
	boolean flip = true;				// BMP is stored bottom-to-top
	int w, h, row, col;
	uint8_t r, g, b;
	uint32_t pos = 0, startTime = millis();
	uint16_t paletteRGB[1 << bitsPerPixel][3];
	// for (int i = 0; i < 1 << bitsPerPixel; i++)
	// {
	// 	paletteRGB[i][0] = 255 * (palette[i] & 0xF800 >> 11) / 31;
	// 	paletteRGB[i][1] = 255 * (palette[i] & 0x7E0 >> 5) / 63;
	// 	paletteRGB[i][2] = 255 * (palette[i] & 0x1F) / 31;
	// }

	bmpFile = LittleFS.open(filename, "r");
	// Open requested file on SD card
	if (!bmpFile)
	{
		Serial.print(F("File not found"));
		return;
	}

	// Parse BMP header
	if (read16(bmpFile) == 0x4D42)
	{ // BMP signature
		// Serial.print(F("File size: "));
		uint32_t filesize = read32(bmpFile);
		// Serial.println(filesize);
		(void)read32(bmpFile);			  // Read & ignore creator bytes
		bmpImageoffset = read32(bmpFile); // Start of image data
		// Serial.print(F("Image Offset: ")); Serial.println(bmpImageoffset, DEC);
		//  Read DIB header
		// Serial.print(F("Header size: "));
		uint32_t headerSize = read32(bmpFile);
		bmpWidth = read32(bmpFile);
		bmpHeight = read32(bmpFile);
		if (read16(bmpFile) == 1)
		{								// # planes -- must be '1'
			bmpDepth = read16(bmpFile); // bits per pixel
			// Serial.print(F("Bit Depth: "));
			// Serial.println(bmpDepth);

			uint32_t compessed = read32(bmpFile);
			Serial.printf("Width: %d; Height: %d; Bit Depth: %d; Compessed: \n", bmpWidth, bmpHeight, bmpDepth, compessed);
			if ((compessed == 0))
			{ // 0 = uncompressed

				goodBmp = true; // Supported BMP format -- proceed!
				/*Serial.print(F("Image size: "));
				Serial.print(bmpWidth);
				Serial.print('x');
				Serial.println(bmpHeight);*/

				// BMP rows are padded (if needed) to 4-byte boundary
				rowSize = (bmpWidth * 3 + 3) & ~3;

				// If bmpHeight is negative, image is in top-down order.
				// This is not canon but has been observed in the wild.
				if (bmpHeight < 0)
				{
					bmpHeight = -bmpHeight;
					flip = false;
				}

				// Crop area to be loaded
				w = bmpWidth;
				h = bmpHeight;
				if ((x + w - 1) >= img->width())
					w = img->width() - x;
				if ((y + h - 1) >= img->height())
					h = img->height() - y;

				if ((bmpDepth == 24))
				{

					for (row = 0; row < h; row++)
					{ // For each scanline...

						// Seek to start of scan line.  It might seem labor-
						// intensive to be doing this on every line, but this
						// method covers a lot of gritty details like cropping
						// and scanline padding.  Also, the seek only takes
						// place if the file position actually needs to change
						// (avoids a lot of cluster math in SD library).
						if (flip) // Bitmap is stored bottom-to-top order (normal BMP)
							pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
						else // Bitmap is stored top-to-bottom
							pos = bmpImageoffset + row * rowSize;
						if (bmpFile.position() != pos)
						{ // Need seek?
							bmpFile.seek(pos, SeekSet);
							buffidx = sizeof(sdbuffer); // Force buffer reload
						}

						for (col = 0; col < w; col++)
						{ // For each pixel...
							// Time to read more pixel data?
							if (buffidx >= sizeof(sdbuffer))
							{ // Indeed
								bmpFile.read(sdbuffer, sizeof(sdbuffer));
								buffidx = 0; // Set index to beginning
							}

							// Convert pixel from BMP to TFT format, push to display
							b = sdbuffer[buffidx++];
							g = sdbuffer[buffidx++];
							r = sdbuffer[buffidx++];

							color = ((uint32_t)g << 16 | (uint32_t)b << 8 | (uint32_t)r);

							// uint32_t minDistance = 99999999L;
							// for (int i = 0; i < (1 << bitsPerPixel); i++)
							// {
							// 	int16_t rd = (r - paletteRGB[i][0]);
							// 	int16_t gd = (g - paletteRGB[i][1]);
							// 	int16_t bd = (b - paletteRGB[i][2]);
							// 	uint32_t distance = rd * rd + gd * gd + bd * bd;
							// 	if (distance < minDistance)
							// 	{
							// 		color = i;
							// 		minDistance = distance;
							// 	}
							// }
							img->drawPixel(col + x, row + y, color);
							//_tft->pushColor(_tft->color565(r,g,b));
							yield();
						} // end pixel
					}	  // end scanline
				}
				else if (bmpDepth == 1)
				{
				}

			} // end goodBmp
			else
			{
				Serial.printf("compessed file: %d\n", compessed);
			}
		}
	}

	bmpFile.close();
	if (!goodBmp)
		Serial.println(F("BMP format not recognized."));
}