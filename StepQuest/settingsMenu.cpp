#include "settingsMenu.h"
Location wifiOption = {.x1=50,.x2=130,.y1=80,.y2=106};
Location backlightOption = {.x1=50,.x2=200,.y1=110,.y2=136};
Location timeOption = {.x1=50,.x2=120,.y1=140,.y2=166};
Location settingsMenuItems[3] = {wifiOption, backlightOption, timeOption};
// add check for quest_selected
void checkMenuPress(int x, int y)
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
            return;
          }
          case (1):
          {
            Serial.print("Backlight");
            return;
          }
          case (2):
          {
            Serial.print("Time");
            return;
          }
        }
      }
    }
  }
}
