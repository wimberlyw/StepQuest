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
extern boolean stepTaskActive;
extern boolean squatTaskActive;
extern boolean jackTaskActive;
extern TFT_eSprite background;
Location Dquest_1 = {.x1=50,.x2=180,.y1=120,.y2=140};
Location Dexit_but = {.x1=30,.x2=180,.y1=200,.y2=240};
Location dLocations[2] = {Dexit_but,Dquest_1};

Location dQuestButtons = {.x1=170,.x2=240,.y1=110,.y2=210};



extern boolean shopDisplayed;
extern boolean questDisplayed;
extern Player p;


void setupDungeon(int level, int location, dungeon* D) {
  Serial.println(p.currStatus);
  int dungeonFloors = random(1, (5 * level)); // random 1 - 5

  D->numDungeonFloors = dungeonFloors;
  D->defeated = false;

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
      Item it = selectDungeonItem(level);
      D->currItems[i] = it;
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
        background.print(D.currFloor + 1);
        background.print("/");
        background.print(D.numDungeonFloors); 
        background.setTextColor(TFT_BLACK);
        if(D.currQuests[D.currFloor].type == 4){
            background.setCursor(65,105); 
            background.print("You've found a ");
            background.setCursor(65,115);
            background.print(D.currQuests[D.currFloor].desc1);
            background.setCursor(65,125);
            background.print(D.currQuests[D.currFloor].desc2);    
        }
        else{
          background.setCursor(65,105);
          background.print(D.currQuests[D.currFloor].desc1);
          background.setCursor(65,115);
          background.print(D.currQuests[D.currFloor].desc2);  
        }
        
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
      }
    }
    }
        
        switch(D->dungeon_quest_selected)
        {
          case 0:{break;}
          case 1:
          {
            
            if(D->currQuests[D->currFloor].type > 2){
                 switch(D->currQuests[D->currFloor].type)
                 {
                  case 3: // money
                        {
                            Serial.println("in Money");
                            p.gold += D->currQuests[D->currFloor].gold;
                            p.xp += D->currQuests[D->currFloor].xp;
                            String s = "";
                            s = "You took the gold ";
                            createPopup(s);
                            D->currFloor++;
                            if(D->currFloor +1 > D->numDungeonFloors)
                              { D->defeated = true;}
                            D->dungeon_quest_selected = 0;
                            break;
                            
                        }
                  case 4: // item
                        {
                           Serial.println("items");
                          Item theItem = D->currItems[D->currFloor];
                          int prev_level;
                          String s = "";
                          prev_level = p.itemLevels[theItem.type];
                          if (p.itemLevels[theItem.type] <= theItem.level)
                                
                          {
                                  s = "You have replaced " + p.items[theItem.type].itemName + " with " + theItem.itemName + ". ";
                                  p.itemLevels[theItem.type] = theItem.level;
                                  p.items[theItem.type] = theItem;
                                
                          
                                s = s + "You have gained " + (p.itemLevels[theItem.type]-prev_level) + (theItem.type==0? " power." : " armor.");
                                createPopup(s);
                          }
                          else {
                             s = "You threw the item away";
                                createPopup(s);
                          }
                          D->currFloor++;
                            if(D->currFloor +1 > D->numDungeonFloors)
                              { D->defeated = true;}
                          
                          D->dungeon_quest_selected = 0;
                          break; 
                       }
                }
                Serial.println("floor" + D->currFloor +1);
            }
            
            else{
              if (dQuestButtons.x1 <= x && dQuestButtons.x2 >= x) 
                {
                if (dQuestButtons.y1 <= y && dQuestButtons.y2 >= y)
                  {
                    if(D->currQuests[D->currFloor].active == true){
                          D->currQuests[D->currFloor].active = false;
                           stepTaskActive = false;
                           squatTaskActive = false;
                           jackTaskActive = false;
                          break;
                    }
                    else{
                      
                      D->currQuests[D->currFloor].active = true;
                      // what type of quest are we completing?
                      switch(D->currQuests[D->currFloor].type)
                        {
                          case (0): // walking, simplest case
                          {
                            //D->currQuests[D->currFloor].active = true;
                            
                            stepTaskActive = true;
                            
                            break;
                          }
                          case(1): // squats
                          {
                            //D->currQuests[D->currFloor].active = true;
                            squatTaskActive = true;
                            
                            break;
                          }
                          case(2): // jumping jacks
                          {
                            //D->currQuests[D->currFloor].active = true;
                            jackTaskActive = true;
                            break;
                          }
  
                        }// send switch currfloor.type
                    }// end else currentquest active
                  }
                }
                }// else type 0 - 2)
          }// end case1
        }// end switch dungeon_quest_selected
  
}
