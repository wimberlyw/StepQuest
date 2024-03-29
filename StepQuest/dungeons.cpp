#include "towns.h"
#include "quests.h"
#include "popup.h"
#include "shops.h"
#include "dungeons.h"
#include "structs.h"

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

  int dungeonFloors = random(1, (5 * level)); // random 1 - 5

  D->numDungeonFloors = dungeonFloors;

  int numQuests = 0;
  int numItems =0;
  int roll;
  Serial.print("Floors: ");
  Serial.println(dungeonFloors);
    
  for(int i= 0; i<dungeonFloors; i++)
  {
    
    roll = random(3);
    Serial.print("roll ");
    Serial.print(roll);
    switch(roll){
      case 0 :
      {
        D->currQuests[i] = createQuest(random(level, 2*level), location); // Generate a quest at 1 or 2 times the player level
        Serial.println(" Quest");
        numQuests++;
        break;
      }
      case 1:
      {
      Item it = selectDungeonItem();
      Serial.println(" Item");
      String d1 = "";
      String d2 = "";
      
      d1 = it.itemName;
      String strs[3];
      int StringCount = 0;  

      //Split the string name because we have long ass item names
     while (d1.length() > 0)
      {
        int index = d1.indexOf(' ');
        if (index == -1) // No space found
        {
          strs[StringCount++] = d1;
          break;
        }
        else
        {
          strs[StringCount++] = d1.substring(0, index);
          d1 = d1.substring(index+1);
        }
      }
      switch(StringCount){
        case 0:
        {
          d1 = strs[0];
          break;
        }
        case 1:
        {
          d1 = strs[0] + strs[1];
          break;
        }
       case 3:
       {
          d1 = strs[0] + " " + strs[1];
          d2 = strs[2];
          break;
       }
      }
      
      
      Quest itemQ = {.desc1= d1,.desc2= d2,.requirement=0,.progress=0,.type=4,.gold=0,.xp=0,.valid=true,.active=false};
      D->currQuests[i] = itemQ;
      numItems++;  
      break;
    }
   case 2:
    {
      Serial.println(" Money");
      Quest qe = createMoneyQuest(level, location);
      Serial.println(qe.desc1);
      Serial.println(qe.desc2);
      Serial.println(qe.gold);
      D->currQuests[i] = qe; 
      break;
    }
    } 
  } // for
    
  D->currFloor = 0;
  D->numQuests = numQuests;
  D->numItems = numItems;
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
        if(D.currQuests[D.currFloor].type < 3){
        background.setCursor(65,125);
        background.print("Gold: ");
        background.print(D.currQuests[D.currFloor].gold);
        background.print(" XP: ");
        background.print(D.currQuests[D.currFloor].xp);
        }
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
