/* Implements in world map location and the travel 
 *  system to traverse those locations
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

// Steps to locations (simple for now)
int locSteps[5] = {0, 500, 1000, 1500, 2000};

// player location based on town/dungeon
int curLocation = 0;
int travelLocation = 0; // where player is travelling rn

// external items
extern TFT_eSprite background;
extern TFT_eSprite popup;
extern CST816S touch;
extern unsigned long previousMillisScreen;


boolean ans;

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

void checkLocation(int x, int y)
{
  for (int i = 0; i < 5; i++)
  {
    if (locations[i].x1 <= x && locations[i].x2 >= x)
    {
      if (locations[i].y1 <= y && locations[i].y2 >= y)
      {
        if (i == curLocation)
        {
          // open appropriate menu
          Serial.println("Already here!");
        }
        else if (curLocation == -1) // already traveling
        {
          if (i == travelLocation)
          {
            // Output that already travelling there and steps remaining
          }
          else
          {
            // Do we want to travel there instead? if yes function call
          }
        }
        else // need to write text to the popup still
        {
          // Do you want to travel here? If yes function call
          popup.pushToSprite(&background, 40, 80);
          background.pushSprite(0,0); // so the sprite shows up but doesn't stay to wait for interaction.
          Serial.println("Do you want to travel here?");

          while(!checkInteraction()){} // wait until popup is answered

          Serial.println(ans);
        }
      }
    }
  }
}
