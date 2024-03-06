#include "travel.h"

Location Town1 = {.x1=25,.x2=55,.y1=190,.y2=215};
Location Town2 = {.x1=165,.x2=210,.y1=180,.y2=230};
Location Dungeon1 = {.x1=205,.x2=240,.y1=100,.y2=125};
Location Town3 = {.x1=0,.x2=55,.y1=95,.y2=130};
Location Dungeon2 = {.x1=130,.x2=185,.y1=5,.y2=65};
Location travelLocations[5] = {Town1,Town2,Dungeon1,Town3,Dungeon2};

// Defining yes and no locations for popups
Location Yes = {.x1=40,.x2=200,.y1=155,.y2=170};
Location No = {.x1=40,.x2=200,.y1=175,.y2=190};

// Steps between locations (simple for now)
int locSteps[4] = {50, 100, 150, 200};
//                0-1  1-2  2-3  3-4

// player location based on town/dungeon
int curLocation = 0;
int travelLocation = 0; // where player is travelling rn
extern volatile int travelSteps; // steps left
extern volatile int totalTravelSteps; // steps taken
//extern volatile int fractionTravelSteps; // divides travelSteps into eight sections to display progress on map
extern boolean travelling;
extern int player_location;
//extern volatile int stepsToChangePos;

// external items
extern TFT_eSprite background;
extern TFT_eSprite popup;
extern TFT_eSprite popupText;
extern CST816S touch;
extern unsigned long previousMillisScreen;
extern int screen;

boolean ans;

void writePopupText(String s)
{
  popupText.fillScreen(TFT_WHITE);
  popupText.setCursor(5,5);
  popupText.setTextColor(TFT_BLACK);
  popupText.print(s);
}

boolean checkInteraction() // returns true if popup is answered
{
  if(touch.available()){ 
    // Check if the gesture falls outside of debounce time
    if (millis() - previousMillisScreen < SCREENDEBOUNCE){
      return false;
    }
    //Get the gesture
    String gest = touch.gesture();
    Serial.println(gest);
  
    if (gest == "SINGLE CLICK")
    {
      previousMillisScreen = millis();
      int x = touch.data.x;
      int y = touch.data.y;
      
      if (Yes.x1 <= x && Yes.x2 >= x && Yes.y1 <= y && Yes.y2 >= y)
      {
        ans = true;
        return true;
      }
      else if (No.x1 <= x && No.x2 >= x && No.y1 <= y && No.y2 >= y)
      {
        ans = false;
        return true;
      }
    }
  }
  return false;
}

String locationName(int location)
{
  switch(location)
  {
    case(0):
    {
      return "Town 1";
    }
    case(1):
    {
      return "Town 2";
    }
    case(2):
    {
      return "Dungeon 1";
    }
    case(3):
    {
      return "Town 3";
    }
    case(4):
    {
      return "Dungeon 2";
    }
  } // end switch
}

void finishTravel() // since the screen freezes (including step screen) maybe make it block out the entire background as a notification
{
  travelling = false;
  curLocation = travelLocation;
  player_location = travelLocation;
  // also need to setup location info like shop items, quest board, etc.

  // Might want to implement a way to push a notification that tells the user they finished travel
  // Temporarily though
  writePopupText("You have arrived at your destination!");
  popupText.pushToSprite(&background,40,80);
  background.pushSprite(0,0);

  while(!touch.available()){}

}
// NEED TO CONSIDER THE PREVIOUS TRAVEL LOCATION TOO WHEN CALCULATING STEPS!
void beginTravel(int location)
{
  int potentialSteps = 0;
//  boolean flag = false; // are we changing from a travelling position?

  if (travelling)
  {
    if (travelLocation < location) // we're travelling past where we were before, add new location steps to current ones
    {
      potentialSteps += travelSteps;

      for (int i = travelLocation; i < location; i++)
      {
        potentialSteps += locSteps[i];
      }
    }
    else // we're no longer travelling as far as before, subtract the difference from current steps
    {
      potentialSteps += travelSteps;

      for (int i = location; i < travelLocation; i++)
      {
        potentialSteps -= locSteps[i];
      }
    }
//    flag = true;
  }
  else
  {
    if (curLocation < location) // moving forward
    {
      for (int i = curLocation; i < location; i++)
      {
        potentialSteps += locSteps[i];
      }
    }
    else // moving backwards
    {
      for (int i = curLocation; i > location; i--)
      {
        potentialSteps += locSteps[i-1];
      }
    }
  }

  if (potentialSteps < 0)
  {
    potentialSteps *= -1;
  }
  
  String s = "It will take ";
  s = s + potentialSteps;
  s = s + " steps to get to ";
  s = s + locationName(location);
  s = s + ". Is this okay?";
  writePopupText(s);
  popup.pushToSprite(&background,40,80);
  popupText.pushToSprite(&background,40,80);
  background.pushSprite(0,0);

  while(!checkInteraction()){} // wait until ppup is answered

  if (ans)
  {
//    if (flag)
//    {
//      curLocation = travelLocation;
//    }
    travelLocation = location;
    travelSteps = potentialSteps;
    player_location = -1;
//    fractionTravelSteps = (int)(travelSteps/8);
    totalTravelSteps = 0;
    travelling = true;
    writePopupText("Travel started! Enjoy your trip!");
    popupText.pushToSprite(&background,40,80);
    background.pushSprite(0,0); // need to find way to remove popup (yes,no) from this, need to check what happens at end of travel (does it work).

    while(!touch.available()){}

    return;
  }
  writePopupText("Travel cancelled.");
  popupText.pushToSprite(&background,40,80);
  background.pushSprite(0,0);

  while(!touch.available()){}

  return;
}

void checkMapLocation(int x, int y)
{
  for (int i = 0; i < 5; i++)
  {
    if (travelLocations[i].x1 <= x && travelLocations[i].x2 >= x)
    {
      if (travelLocations[i].y1 <= y && travelLocations[i].y2 >= y)
      {
        if (travelling) // already traveling
        {
          if (i == travelLocation)
          {
            // Output that already travelling there and steps remaining
            String s = "You are currently traveling to that location! You have ";
            s = s + travelSteps;
            s = s + " remaining.";
            writePopupText(s);
            popupText.pushToSprite(&background, 40,80);
            background.pushSprite(0,0);

            while(!touch.available()){}

            return;
          }
          else
          {
            // Do we want to travel there instead? if yes function call
            popup.pushToSprite(&background,40,80);
            String s = "You are currently travelling to ";
            s = s + locationName(travelLocation);
            s = s + ". Would you rather travel to ";
            s = s + locationName(i);
            s = s + "?";

            writePopupText(s);
            popupText.pushToSprite(&background,40,80);
            background.pushSprite(0,0);

            while(!checkInteraction()){} // wait until popup is answered

            if (ans)
            {
              beginTravel(i);
            }
            return;
          }
        }
        else if (i == curLocation)
        {
          screen = 3; // assumes 3 is the local location screen, may change
          return;    
        }
        else
        {
          // Do you want to travel here? If yes function call
          popup.pushToSprite(&background, 40, 80);
          String s = "Do you want to travel to ";
          s = s + locationName(i);
          s = s + "?";
          writePopupText(s);
          popupText.pushToSprite(&background, 40,80);
          background.pushSprite(0,0);

          while(!checkInteraction()){} // wait until popup is answered

          if (ans)
          {
            beginTravel(i);
          }
          return;
        }
      }
    }
  }
}
