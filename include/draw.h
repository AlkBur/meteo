#ifndef DRAW_H
#define DRAW_H

#include <Arduino.h>

#define MAX_IMAGE_WIDTH 320
void drawProgress(uint8_t percentage, String text, bool onlyUpdate);
void drawBlock1();
void drawBlock2();
void drawBlock3();
void drawBlock4();

#endif