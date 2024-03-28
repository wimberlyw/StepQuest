#include "towns.h"
#include "quests.h"
#include "popup.h"
#include "shops.h"
#include "dungeons.h"

/*
  Location quest_but = {.x1=50,.x2=180,.y1=80,.y2=120};
  Location shop_but = {.x1=50,.x2=180,.y1=130,.y2=170};
  Location townLocations[2] = {quest_but,shop_but};
*/
extern TFT_eSprite background;
Location Dquest_1 = {.x1=50,.x2=180,.y1=120,.y2=140};
Location Dexit_but = {.x1=30,.x2=180,.y1=200,.y2=240};
Location dLocations[2] = {Dexit_but,Dquest_1};

Location dQuestButtons = {.x1=200,.x2=240,.y1=110,.y2=140};



extern boolean shopDisplayed;
extern boolean questDisplayed;
extern Player p;


void setupDungeon(int level, int location, dungeon* D) {

  randomSeed(millis());

  int dungeonFloors = random(1, 6); // random 1 - 5

  D->numDungeonFloors = dungeonFloors;

  int numQuests = random(1, 6); // random 1 - 5 number of quests
  D->numQuests = numQuests;

  int numItems = dungeonFloors - numQuests;
  D->numItems = numItems;


  for (int i = 0; i < numQuests; i++)
  {

    D->currQuests[i] = createQuest(random(level, 2), location); // Generate a quest at 1 or 2 times the player level

  }
  for (int i = numQuests + 1; i < (numQuests + numItems); i++) // Need to do this
  {
    D->currItems[i] = selectDungeonItem();
  }

  D->currFloor = 0;
}

void drawDungeonQuests(dungeon D)
{
        background.setTextSize(1);
        background.setTextColor(TFT_WHITE);
        background.setCursor(65,75);
        background.print("Current Floor: ");
        background.print(D.currFloor);
         background.setTextColor(TFT_BLACK);
        background.setCursor(65,105);
        background.print(D.currQuests[D.currFloor].desc1);
        background.setCursor(65,115);
        background.print(D.currQuests[D.currFloor].desc2);
        background.setCursor(65,125);
        background.print("Gold: ");
        background.print(D.currQuests[D.currFloor].gold);
        background.print(" XP: ");
        background.print(D.currQuests[D.currFloor].xp);
}




void checkDungeonClick(int x, int y, dungeon* D) // figures out what button has been pressed on town menu
{
    for (int i = 0; i < 2; i++)
    {
    if (dLocations[i].x1 <= x && dLocations[i].x2 >= x)
    {
      if (dLocations[i].y1 <= y && dLocations[i].y2 >= y)
      {
        switch(i)
        {
          case 0: // Exxit Button
          {
            //D->dungeon_quest_selected = 0;
            break;
          }
          case 1:
          {
            if(D->dungeon_quest_selected ==0)
            {
               D->dungeon_quest_selected = 1;
            }
            else if( D->dungeon_quest_selected == 1){
               D->dungeon_quest_selected = 0;
            }
           
            break;
          } 
        }

        switch(D->dungeon_quest_selected)
        {
          case 0:{break;}
          case 1:
          {
            
 
            if (dQuestButtons.x1 <= x && dQuestButtons.x2 >= x)
            {
              if (dQuestButtons.y1 <= y && dQuestButtons.y2 >= y)
              {
                  if (D->currQuests[D->currFloor].active) stopQuest();
                  else beginQuest();
              }
            }
          
          break;
          }// end case1
        }// end switch dungeon_quest_selected
          
        }// end 
      }
    }
  
}
