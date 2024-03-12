#include "popup.h"

extern TFT_eSprite background;
extern TFT_eSprite popup;
extern TFT_eSprite popupText;
extern CST816S touch;
extern unsigned long previousMillisScreen;

// Defining yes and no locations for popups
Location Yes = {.x1=40,.x2=200,.y1=155,.y2=170};
Location No = {.x1=40,.x2=200,.y1=175,.y2=190};

boolean checkInteraction() // returns the answer of the popup
{
  while(1)
  {
    if(touch.available()){ 
      // Check if the gesture falls outside of debounce time
      if (millis() - previousMillisScreen < SCREENDEBOUNCE){
        continue;
      }
      //Get the gesture
      String gest = touch.gesture();
    
      if (gest == "SINGLE CLICK")
      {
        previousMillisScreen = millis();
        int x = touch.data.x;
        int y = touch.data.y;
        
        if (Yes.x1 <= x && Yes.x2 >= x && Yes.y1 <= y && Yes.y2 >= y)
        {
          return true;
        }
        else if (No.x1 <= x && No.x2 >= x && No.y1 <= y && No.y2 >= y)
        {
          return false;
        }
      }
    }
  }
}

void checkForTap()
{
  while(1)
  {
    if (touch.available())
    {
      if (millis() - previousMillisScreen < SCREENDEBOUNCE)
      {
        continue;
      }
      //Get the gesture
      String gest = touch.gesture();
    
      if (gest == "SINGLE CLICK")
      {
        return;
      }
    }
  }
}

void writePopupText(String s)
{
  popupText.fillScreen(TFT_WHITE);
  popupText.setCursor(5,5);
  popupText.setTextColor(TFT_BLACK);
  popupText.print(s);
}

boolean createYesNoPopup(String s)
{
  writePopupText(s);
  popup.pushToSprite(&background,40,80);
  popupText.pushToSprite(&background,40,80);
  background.pushSprite(0,0);

  return checkInteraction();
}

void createPopup(String s)
{
  s = s + "(Tap anywhere to continue)";
  writePopupText(s);
  popupText.pushToSprite(&background,40,80);
  background.pushSprite(0,0);

  checkForTap();
}
