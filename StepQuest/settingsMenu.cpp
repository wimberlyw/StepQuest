#include "settingsMenu.h"
//#include "wifitime.h"
#include "structs.h"

Location wifiOption = {.x1 = 50, .x2 = 200, .y1 = 80, .y2 = 106};
Location backlightOption = {.x1 = 50, .x2 = 200, .y1 = 110, .y2 = 136};
Location timeOption = {.x1 = 50, .x2 = 200, .y1 = 140, .y2 = 190};
Location settingsMenuItems[3] = {wifiOption, backlightOption, timeOption};

Location setHourUp = {.x1 = 0, .x2 = 60, .y1 = 65, .y2 = 106};
Location setHourDown = {.x1 = 0, .x2 = 65, .y1 = 135, .y2 = 165};
Location setMinUp = {.x1 = 75, .x2 = 125, .y1 = 65, .y2 = 90};
Location setMinDown = {.x1 = 75, .x2 = 125, .y1 = 140, .y2 = 190};
Location TimeOK = {.x1 = 0, .x2 = 240, .y1 = 195, .y2 = 240};
Location setTimeItems[5] = {setHourUp, setHourDown, setMinUp, setMinDown, TimeOK};
// add check for quest_selected

extern TFT_eSprite background;
uint8_t localBrightness;


void checkMenuPress(int x, int y, struct timekeeping *timekeeperPTR, uint8_t * brightnessVal)
{
  Serial.print(x);
  Serial.print("\t");
  Serial.print(y);
  Serial.print("\t\n");
  if (!timekeeperPTR->settingTime) {
    for (int i = 0; i < 3; i++)
    {
      if (settingsMenuItems[i].x1 <= x && settingsMenuItems[i].x2 >= x)
      {
        if (settingsMenuItems[i].y1 <= y && settingsMenuItems[i].y2 >= y)
        {
          switch (i)
          {
            case (0):
              {
                Serial.println("Wifi");
                connectToWifi(timekeeperPTR);
                break;

              }
            case (1):
              {
                Serial.println("Backlight");
                localBrightness = *brightnessVal;
                Serial.println(localBrightness);
                localBrightness -= 25;
                if (localBrightness < 25 || localBrightness > 100 )
                {
                  localBrightness = 100;
                }
                *brightnessVal = localBrightness;
                Serial.println(localBrightness);
                Serial.println(*brightnessVal);

                TFT_SET_BL(brightnessVal);
                break;

              }
            case (2):
              {

                Serial.println("Time");

                timekeeperPTR->settingTime = true;
                break;

                
              }
          }
        }
      }
    }
  } // if !settingTime
  else // Setting Time
  {
    for (int i = 0; i < 5; i++)
    {
      if (setTimeItems[i].x1 <= x && setTimeItems[i].x2 >= x){
        if(setTimeItems[i].y1 <= y && setTimeItems[i].y2 >= y)
        {
          switch (i)
          {
            case (0):
              {
                ++(timekeeperPTR->_hours);
                if(timekeeperPTR->_hours == 13) timekeeperPTR->_hours = 1;
                Serial.println("Hours Up");
                break;
              }
            case (1):
              {
                --(timekeeperPTR->_hours);
                if(timekeeperPTR->_hours == 0) timekeeperPTR->_hours = 12;
                Serial.println("Hours Down");
                break;
              }
            case (2):
              {
                ++(timekeeperPTR->_minutes);
                if(timekeeperPTR->_minutes == 60) timekeeperPTR->_minutes = 0;
                Serial.println("Mins Up");
                break;
              }
            case (3):
              {
                --(timekeeperPTR->_minutes);
                if(timekeeperPTR->_minutes == -1) timekeeperPTR->_minutes = 59;
                Serial.println("Mins Down");
                break;
              }
            case (4):
              {
                Serial.println("OK");
                timekeeperPTR->_seconds = 0;
                timekeeperPTR->settingTime = false;
                break;
              }
              

          }

        }
        
      }
    }
  }
    return;

  }


void drawSetTimeScreen(struct timekeeping *timekeeperPTR)
{
  Serial.println("in draw");
  background.fillScreen(TFT_BLACK);
  background.setCursor(62, 100, 4);
  background.setTextColor(TFT_WHITE, TFT_BLACK);
  background.println("TEST");
  background.pushSprite(0, 0);

  /*if(timekeeperPTR->_hours <10){
    background.print(" ");
    }
    background.print(timekeeperPTR->_hours);
    background.print(":");
    if (timekeeperPTR->_minutes < 10) {
    background.print('0');
    }
    background.print(timekeeperPTR->_minutes);
    background.print(":");
    //background.setCursor(175, 65, 2);
    if (timekeeperPTR->_seconds < 10) {
    background.print("0");
    }
    background.print(timekeeperPTR->_seconds);
    if(timekeeperPTR->connection == true)
    {
     background.setCursor(62, 130, 4);
     background.println(timekeeperPTR->_days);
    }*/
}

void TFT_SET_BL(uint8_t * Value) {
  if (*Value < 0 || *Value > 100) {
    Serial.println("TFT_SET_BL Error \r\n");
  } else {
    analogWrite(TFT_BL, *Value * 2.55);
  }
}
