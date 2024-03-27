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
// space is 160 x 60, lets limit to 25 characters per line
void writePopupText(String s)
{
  s = s + " "; // add extra space for the conditions below
  int start_indent = 5; // the y-coordinate since it changes unlike the x-coordinate
  int end_segment = 25-1; // last element of the current line
  int i = 0;
  popupText.fillScreen(TFT_WHITE);
  popupText.setTextColor(TFT_BLACK);

  while (i < s.length())
  {
    if (end_segment > s.length()) end_segment = s.length();
    
    while (!isSpace(s.charAt(end_segment-1)))
    {
      end_segment -= 1;
    }

    String temp = s.substring(i, end_segment-1); // -1 cuts out the extra space at the end
    popupText.setCursor(5,start_indent);
    popupText.print(temp);

    // setup for next line
    start_indent += 10;
    i = end_segment;
    end_segment += 25;
  }
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
  s = s + " (Tap anywhere to continue)";
  writePopupText(s);
  popupText.pushToSprite(&background,40,80);
  background.pushSprite(0,0);

  checkForTap();
}
