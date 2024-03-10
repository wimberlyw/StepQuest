#include "travel.h"
#include "shops.h"
#include "popup.h"

Location Town1 = {.x1=25,.x2=55,.y1=190,.y2=215};
Location Town2 = {.x1=165,.x2=210,.y1=180,.y2=230};
Location Dungeon1 = {.x1=205,.x2=240,.y1=100,.y2=125};
Location Town3 = {.x1=0,.x2=55,.y1=95,.y2=130};
Location Dungeon2 = {.x1=130,.x2=185,.y1=5,.y2=65};
Location travelLocations[5] = {Town1,Town2,Dungeon1,Town3,Dungeon2};

// Steps between locations (simple for now)
int locSteps[4] = {20, 25, 30, 35};
//                0-1  1-2  2-3  3-4

int travelLocation = 0; // where player is travelling rn
boolean travelDirection = false; // false means backwards, true means forwards
extern volatile int travelSteps; // steps left
//extern volatile int fractionTravelSteps; // divides travelSteps into eight sections to display progress on map
extern boolean travelling;
extern volatile int stepsToNextPath; // tells us how many steps until we reach the next path
extern Player p;
//extern volatile int stepsToChangePos;

extern int screen;
extern boolean left; // one time check to set quests inactive when leaving a location

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

//void getStepsToNextPath()
//{
//  if (travelDirection) // forward
//  {
//    if ((p.path+1) > 3) return; // prevent errors
//    stepsToNextPath = locSteps[p.path+1];
//    p.path++; 
//  }
//  else
//  {
//    if ((p.path-1) < 0) return; // prevent errors
//    stepsToNextPath = locSteps[p.path-1];
//    p.path--;
//  }
//}

void getStepsToNextPath()
{
  int ans = 0;
  if (travelDirection) // forward
  {
    if ((p.path+1) > 3) return; // prevent errors
    p.path++; 
    ans = travelSteps;
    for (int i = (p.path+1); i < travelLocation; i++)
    {
      ans -= locSteps[i];
    }
  }
  else
  {
    if ((p.path-1) < 0) return; // prevent errors
    p.path--;
    ans = travelSteps;
    for (int i = (p.path-1); i >= travelLocation; i++)
    {
      ans -= locSteps[i];
    }
  }
  stepsToNextPath = ans;
}

void finishTravel() // since the screen freezes (including step screen) maybe make it block out the entire background as a notification
{
  travelling = false;
  p.location = travelLocation;
  p.path = -1;
  // also need to setup location info like shop items, quest board, etc.

  // Might want to implement a way to push a notification that tells the user they finished travel
  // Temporarily though
  String s = "You have arrived at your destination!";
  
  createPopup(s);
}

// To be accurate, I need to know what path I'm on at any point. Fix on different branch soon.
void beginTravel(int location)
{
  int potentialSteps = 0;
  int potentialStepsToPath = 0;
  boolean potentialDirection = false;
  int potentialPath = -1;

  if (travelling)
  {
    if (p.path < location) // travelling forward from current location
    {
      potentialDirection = true;
      if (travelDirection) // we were moving forward
      {
        potentialSteps += stepsToNextPath;
      }
      else // we were moving backwards
      {
        potentialSteps += (locSteps[p.path]-stepsToNextPath);
      }
      potentialStepsToPath = potentialSteps;

      for (int i = (p.path+1); i < location; i++)
      {
        potentialSteps += locSteps[i];
      }
    }
    else // travelling backwards from current location
    {
      potentialDirection = false;
      if (travelDirection) // we were moving forward
      {
        potentialSteps += (locSteps[p.path]-stepsToNextPath);
      }
      else // we were moving backwards
      {
        potentialSteps += stepsToNextPath;
      }
      potentialStepsToPath = potentialSteps;

      for (int i = (p.path-1); i >= location; i--)
      {
        potentialSteps += locSteps[i];
      }
    }
  }
  else // we are starting from a location
  {
    if (p.location < location) // moving forward on map
    {
      potentialStepsToPath = locSteps[p.location]; // only has steps to finish first path
      potentialDirection = true;
      potentialPath = p.location;

      for (int i = p.location; i < location; i++)
      {
        potentialSteps += locSteps[i];
      }
    }
    else // moving backwards on map
    {
      potentialStepsToPath = locSteps[p.location-1]; // only has steps to finish first path
      potentialDirection = false;
      potentialPath = p.location-1;

      for (int i = location; i < p.location; i++)
      {
        potentialSteps += locSteps[i];
      }
    }
  }

  
  
  String s = "It will take ";
  s = s + potentialSteps;
  s = s + " steps to get to ";
  s = s + locationName(location);
  s = s + ". You will also lose any current quest progress. Is this okay?";
  boolean ans = createYesNoPopup(s);

  if (ans)
  {
    recallShop();
    left = true;
    travelLocation = location;
    travelSteps = potentialSteps;
    stepsToNextPath = potentialStepsToPath;
    travelDirection = potentialDirection;
    travelling = true;
    if (potentialPath != -1)
    {
      p.path = potentialPath;
    }

    createPopup("Travel started! Enjoy your trip!");

    return;
  }
  createPopup("Travel cancelled.");

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
            createPopup(s);

            return;
          }
          else
          {
            // Do we want to travel there instead? if yes function call
            String s = "You are currently travelling to ";
            s = s + locationName(travelLocation);
            s = s + ". Would you rather travel to ";
            s = s + locationName(i);
            s = s + "?";
            boolean ans = createYesNoPopup(s);

            if (ans)
            {
              beginTravel(i);
            }
            return;
          }
        }
        else if (i == p.location)
        {
          screen = TOWNMENU; // assumes 3 is the local location screen, may change
          return;    
        }
        else
        {
          // Do you want to travel here? If yes function call
          String s = "Do you want to travel to ";
          s = s + locationName(i);
          s = s + "?";
          boolean ans = createYesNoPopup(s);

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
