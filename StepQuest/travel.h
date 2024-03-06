/* Implements in world map location and the travel 
 *  system to traverse those locations
 */
 /* 
  *  Remaining TODOs: Some popup messages that shouldn't have Yes or No prompts do due to the way it is implemeneted.
  *  Need to showcase where the player currently is in some way.  Need to figure out a better way to notify players that travel has
  *  been completed (using vibration motor). Need to add the random combat mechanic during travel (later since also used by dungeon).
  */
  
/*
 * Location structure defines where location is on
 * screen and how many steps to reach it 
 * (assuming linear progression between locations)
 */
#define SCREENDEBOUNCE 500
 
typedef struct Location{
  int x1;
  int x2;
  int y1;
  int y2;
  // possibly add pointer to appropriate town/dungeon here
}Location;

// definining the locations
Location Town1 = {.x1=25,.x2=55,.y1=190,.y2=215};
Location Town2 = {.x1=165,.x2=210,.y1=180,.y2=230};
Location Dungeon1 = {.x1=205,.x2=240,.y1=100,.y2=125};
Location Town3 = {.x1=0,.x2=55,.y1=95,.y2=130};
Location Dungeon2 = {.x1=130,.x2=185,.y1=5,.y2=65};
Location locations[5] = {Town1,Town2,Dungeon1,Town3,Dungeon2};

// Defining yes and no locations for popups
Location Yes = {.x1=40,.x2=200,.y1=155,.y2=170};
Location No = {.x1=40,.x2=200,.y1=175,.y2=190};

// used to define locations on path during travel
//typedef struct Point{
//  int x;
//  int y;
//}Point;
//
//Point p1_0 = {.x=76,.y=198};
//Point p2_0 = {.x=82,.y=215};
//Point p3_0 = {.x=82,.y=228};
//Point p4_0 = {.x=105,.y=238};
//Point p5_0 = {.x=130,.y=235};
//Point p6_0 = {.x=168,.y=235};
//Point p7_0 = {.x=163,.y=230};
//Point p8_0 = {.x=163,.y=218};
//Point path0[8] = {p1_0,p2_0,p3_0,p4_0,p5_0,p6_0,p7_0,p8_0}; // points on path from loc 0-1
//Point p1_1 = {.x=170,.y=175};
//Point p2_1 = {.x=150,.y=175};
//Point p3_1 = {.x=130,.y=175};
//Point p4_1 = {.x=108,.y=148};
//Point p5_1 = {.x=150,.y=148};
//Point p6_1 = {.x=190,.y=148};
//Point p7_1 = {.x=230,.y=155};
//Point p8_1 = {.x=230,.y=130};
//Point path1[8] = {p1_1,p2_1,p3_1,p4_1,p5_1,p6_1,p7_1,p8_1}; // points on path from loc 1-2
//Point p1_2 = {.x=190,.y=112};
//Point p2_2 = {.x=170,.y=112};
//Point p3_2 = {.x=155,.y=112};
//Point p4_2 = {.x=120,.y=112};
//Point p5_2 = {.x=82,.y=130};
//Point p6_2 = {.x=65,.y=148};
//Point p7_2 = {.x=48,.y=165};
//Point p8_2 = {.x=14,.y=150};
//Point path2[8] = {p1_2,p2_2,p3_2,p4_2,p5_2,p6_2,p7_2,p8_2}; // points on path from loc 2-3
//Point p1_3 = {.x=35,.y=80};
//Point p2_3 = {.x=35,.y=63};
//Point p3_3 = {.x=70,.y=61};
//Point p4_3 = {.x=100,.y=28};
//Point p5_3 = {.x=132,.y=50};
//Point p6_3 = {.x=97,.y=90};
//Point p7_3 = {.x=134,.y=90};
//Point p8_3 = {.x=165,.y=72};
//Point path3[8] = {p1_3,p2_3,p3_3,p4_3,p5_3,p6_3,p7_3,p8_3}; // points on path from loc 3-4
//
//Point[] paths = {path0,path1,path2,path3};

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
//extern volatile int stepsToChangePos;

// external items
extern TFT_eSprite background;
extern TFT_eSprite popup;
extern TFT_eSprite popupText;
extern CST816S touch;
extern unsigned long previousMillisScreen;
extern int screen;

boolean ans;

// returns true if travelling forward, false if travelling backwards
//boolean travelDirection()
//{
//  return curLocation < travelLocation;
//}

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

void checkLocation(int x, int y)
{
  for (int i = 0; i < 5; i++)
  {
    if (locations[i].x1 <= x && locations[i].x2 >= x)
    {
      if (locations[i].y1 <= y && locations[i].y2 >= y)
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