#ifndef settingsMenu_H
#define settingsMenu_H
//#include "structs.h"
#include "wifitime.h"

#define TFT_BL 14

void checkMenuPress(int x, int y, struct timekeeping *timekeeperPTR, uint8_t * brightnessVal);

void TFT_SET_BL(uint8_t * Value);

void drawSetTimeScreen(struct timekeeping *timekeeperPTR);

#endif
