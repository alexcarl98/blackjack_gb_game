#ifndef __common_h_INCLUDE
#define __common_h_INCLUDE
#include <gb/gb.h>
#include <string.h>
#include <stdlib.h>

void performantdelay(uint8_t numloops);
void fadeout();
void fadein();
void DrawNumberBkg(uint8_t x,uint8_t y, uint16_t number,uint8_t digits);
void DrawNumber(uint8_t x,uint8_t y, uint16_t number,uint8_t digits);

#endif