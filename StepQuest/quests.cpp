#include "quests.h"

Location quest_1 = {.x1=50,.x2=180,.y1=70,.y2=100};
Location quest_2 = {.x1=50,.x2=180,.y1=120,.y2=140};
Location quest_3 = {.x1=50,.x2=180,.y1=160,.y2=190};
Location exit_but = {.x1=30,.x2=180,.y1=200,.y2=240};
Location questLocations[4] = {exit_but,quest_1,quest_2,quest_3};

extern boolean questDisplayed;
extern TFT_eSprite background;
extern int quest_selected;

// type 0 = "take x steps"
 String quest_name_0 = "Step Quest";
 int min_requirement_0 = 50;

 // type 1 = "complete x squats"
 String quest_name_1 = "Squat Quest";
 int min_requirement_1 = 5;

 // type 2 = "complete x jumping jacks"
 String quest_name_2 = "Jumping Jack Quest";
 int min_requirement_2 = 10;

 // type 3 = "walk for x time"
 String quest_name_3 = "Patrol Quest";

 // creates a randomized quest based on the player level and location (Currently does not create patrol tasks)
 Quest createQuest(int level, int location)
 {
    int type = random(3); // generate number between 0 and 2 
    String desc1 = "";
    String desc2 = "";
    String quest_name = "";
    int requirement;
    int num = level * (location+1);
    int amp;

    switch(type)
    {
      case (0): // step quest
      {
        requirement = random(20*num) + min_requirement_0; // maxes out at ~10,050
        quest_name = quest_name_0;
        desc1 = "Take ";
        desc1 = desc1 + requirement;
        desc1 = desc1 + " steps.";
        amp = 1;
        // need to add a mention of time once that is figured out
        break;
      }
      case (1): // squat quest
      {
        requirement = random(2*num) + min_requirement_1; // maxes out at ~1,005
        if (requirement > 500) requirement = 500;
        quest_name = quest_name_1;
        desc1 = "Complete ";
        desc1 = desc1 + requirement;
        if (requirement > 99)
        {
          desc2 = desc2 + "squats.";
        }
        else
        {
          desc1 = desc1 + " squats.";
        }
        amp = 3;

        break;
      }
      case(2): // jumping jack quest
      {
        requirement = random(5*num)+min_requirement_2; // maxes out at ~2,510
        if (requirement > 1000) requirement = 1000;
        quest_name = quest_name_2;
        desc1 = "Complete ";
        desc1 = desc1 + requirement;
        if (requirement > 99)
        {
          desc2 = desc2 + "jumping jacks.";
        }
        else
        {
          desc1 = desc1 + " jumping";
          desc2 = desc2 + "jacks.";
        }
        
        amp = 2;    

        break;
      }
    }

    int gold = (int)((requirement / (random(5)+2))*amp) + 1;
    int xp = (int)((requirement/(random(3)+2))*amp) + 3;

    Quest q = {.desc1=desc1,.desc2=desc2,.quest_name=quest_name,.requirement=requirement,.progress=0,.type=type,.gold=gold,.xp=xp};
    return q;
 }
// add check for quest_selected
void checkQuestLocation(int x, int y)
{
  for (int i = 0; i < 4; i++)
  {
    if (questLocations[i].x1 <= x && questLocations[i].x2 >= x)
    {
      if (questLocations[i].y1 <= y && questLocations[i].y2 >= y)
      {
        switch(i)
        {
          case (0):
          {
            questDisplayed = false;
            return;
          }
          case (1):
          {
            Serial.print("Quest1");
            if (quest_selected == 0)
            {
              quest_selected = 1;
            }
            else if (quest_selected == 1)
            {
              quest_selected = 0;
            }
            else
            {
              //popup
            }
            return;
          }
          case (2):
          {
            Serial.print("Quest2");
            if (quest_selected == 0)
            {
              quest_selected = 2;
            }
            else if (quest_selected == 2)
            {
              quest_selected = 0;
            }
            else
            {
              //popup
            }
            return;
          }
          case(3):
          {
            Serial.print("Quest3");
            if (quest_selected == 0)
            {
              quest_selected = 3;
            }
            else if (quest_selected == 3)
            {
              quest_selected = 0;
            }
            else
            {
              //popup
            }
            
            return;
          }
        }
      }
    }
  }
  switch(quest_selected) // need to add checks for the trash and start button
  {
    case(0):
    {
      return;
    }
    case(1):
    {
      break;
    }
    case(2):
    {
      break;
    }
    case(3):
    {
      break;
    }
  }
}

void displayQuests(Quest quests[])
{
  background.setTextColor(TFT_BLACK);
  for (int i = 0; i < 3; i++)
  {
    switch(i)
    {
      case(0):
      {
        background.setCursor(65,55);
        background.print(quests[i].desc1);
        background.setCursor(65,65);
        background.print(quests[i].desc2);
        background.setCursor(65,75);
        background.print("Gold: ");
        background.print(quests[i].gold);
        background.print(" XP: ");
        background.print(quests[i].xp);
        break;
      }
      case(1):
      {
        background.setCursor(65,105);
        background.print(quests[i].desc1);
        background.setCursor(65,115);
        background.print(quests[i].desc2);
        background.setCursor(65,125);
        background.print("Gold: ");
        background.print(quests[i].gold);
        background.print(" XP: ");
        background.print(quests[i].xp);
        break;
      }
      case(2):
      {
        background.setCursor(65,155);
        background.print(quests[i].desc1);
        background.setCursor(65,165);
        background.print(quests[i].desc2);
        background.setCursor(65,175);
        background.print("Gold: ");
        background.print(quests[i].gold);
        background.print(" XP: ");
        background.print(quests[i].xp);
        break;
      }
    }
  }
}
