#include "settingsMenu.h"
//#include "wifitime.h"
#include "structs.h"

Location wifiOption = {.x1=50,.x2=130,.y1=80,.y2=106};
Location backlightOption = {.x1=50,.x2=200,.y1=110,.y2=136};
Location timeOption = {.x1=50,.x2=120,.y1=140,.y2=166};
Location settingsMenuItems[3] = {wifiOption, backlightOption, timeOption};
// add check for quest_selected

uint8_t localBrightness;

void checkMenuPress(int x, int y, struct timekeeping *timekeeperPTR, uint8_t * brightnessVal)
{
  for (int i = 0; i < 3; i++)
  {
    if (settingsMenuItems[i].x1 <= x && settingsMenuItems[i].x2 >= x)
    {
      if (settingsMenuItems[i].y1 <= y && settingsMenuItems[i].y2 >= y)
      {
        switch(i)
        {
          case (0):
          {
            Serial.println("Wifi");
            connectToWifi(timekeeperPTR);
            return;
          }
          case (1):
          {
            Serial.println("Backlight");
            localBrightness = *brightnessVal;
            Serial.println(localBrightness);
            localBrightness -= 25;
            if(localBrightness <25 || localBrightness >100 )
            {
              localBrightness = 100;
            }
            *brightnessVal = localBrightness;
            Serial.println(localBrightness);
            Serial.println(*brightnessVal);

            TFT_SET_BL(brightnessVal);
            return;
          }
          case (2):
          {
            Serial.println("Time");
            printLocalTime();
            return;
          }
        }
      }
    }
  }

}

void TFT_SET_BL(uint8_t * Value) {
  if (*Value < 0 || *Value > 100) {
    Serial.println("TFT_SET_BL Error \r\n");
  } else {
    analogWrite(TFT_BL, *Value * 2.55);
  }
}
