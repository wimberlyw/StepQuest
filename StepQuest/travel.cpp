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
extern volatile int travelSteps; // steps left
extern volatile int totalTravelSteps; // steps taken
//extern volatile int fractionTravelSteps; // divides travelSteps into eight sections to display progress on map
extern boolean travelling;
extern Player p;
//extern volatile int stepsToChangePos;

extern int screen;
extern boolean left;

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
  p.location = travelLocation;
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

  if (travelling)
  {
    if (totalTravelSteps == 0 && p.location != -1)
    {
      // leave at 0 steps
    }
    else if (travelLocation < location) // we're travelling past where we were before, add new location steps to current ones
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
    if (p.location < location) // moving forward
    {
      for (int i = p.location; i < location; i++)
      {
        potentialSteps += locSteps[i];
      }
    }
    else // moving backwards
    {
      for (int i = p.location; i > location; i--)
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
  s = s + ". You will also lose any current quest progress. Is this okay?";
  boolean ans = createYesNoPopup(s);

  if (ans)
  {
//    if (flag)
//    {
//      p.location = travelLocation;
//    }
    recallShop();
    travelLocation = location;
    travelSteps = potentialSteps;
    //p.location = -1;
    left = true;
//    fractionTravelSteps = (int)(travelSteps/8);
    totalTravelSteps = 0;
    travelling = true;
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
