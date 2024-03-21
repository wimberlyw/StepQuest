#include "quests.h"
#include "player.h"
#include "popup.h"

/*TODO Jumping jacks need some more testing, squats and step tasks seem to work*/

Location quest_1 = {.x1=50,.x2=180,.y1=70,.y2=100};
Location quest_2 = {.x1=50,.x2=180,.y1=120,.y2=140};
Location quest_3 = {.x1=50,.x2=180,.y1=160,.y2=190};
Location exit_but = {.x1=30,.x2=180,.y1=200,.y2=240};
Location questLocations[4] = {exit_but,quest_1,quest_2,quest_3};

Location trash1 = {.x1=15,.x2=40,.y1=70,.y2=100};
Location begin_but1 = {.x1=200,.x2=240,.y1=70,.y2=100};
Location questButtons1[2] = {trash1, begin_but1};

Location trash2 = {.x1=15,.x2=40,.y1=110,.y2=140};
Location begin_but2 = {.x1=200,.x2=240,.y1=110,.y2=140};
Location questButtons2[2] = {trash2, begin_but2};

Location trash3 = {.x1=15,.x2=40,.y1=150,.y2=190};
Location begin_but3 = {.x1=200,.x2=240,.y1=150,.y2=190};
Location questButtons3[2] = {trash3, begin_but3};

extern boolean questDisplayed;
extern TFT_eSprite background;
extern int quest_selected;
extern Town t;
extern Player p;
extern boolean stepTaskActive;
extern boolean squatTaskActive;
extern boolean jackTaskActive;

extern float correction;

int cyclesSquat = 0;
int cyclesJump = 0;
float squatThresholdMax = 5;
float squatThresholdMin = -3;
boolean squatFlag = false; // 0 = nothing, 1 == max reached,
float jumpThresholdMax = 24;
float jumpThresholdMin = 3; // was -3
boolean jumpFlag = false;

// type 0 = "take x steps"
 int min_requirement_0 = 50;

 // type 1 = "complete x squats"
 int min_requirement_1 = 5;

 // type 2 = "complete x jumping jacks"
 int min_requirement_2 = 10;

 // type 3 = "walk for x time"
 String quest_name_3 = "Patrol Quest";

 // creates a randomized quest based on the player level and location (Currently does not create patrol tasks)
 Quest createQuest(int level, int location)
 {
    int type = random(3); // generate number between 0 and 2 
    String desc1 = "";
    String desc2 = "";
    int requirement;
    int num = level * (location+1);
    int amp;

    switch(type)
    {
      case (0): // step quest
      {
        requirement = random(20*num) + min_requirement_0; // maxes out at ~10,050
        desc1 = "Take ";
        desc1 = desc1 + requirement;
        desc1 = desc1 + " steps.";
        amp = 1;
        // need to add a mention of time once that is figured out

        // ONLY FOR TEST
        Quest q_step = {.desc1="Take 50 steps.",.desc2="",.requirement=50,.progress=0,.type=0,.gold=10,.xp=5,.valid=true,.active=false};
        return q_step;
        
        break;
      }
      case (1): // squat quest
      {
        requirement = random(2*num) + min_requirement_1; // maxes out at ~1,005
        if (requirement > 500) requirement = 500;
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

        // ONLY FOR TESTING
        Quest q_squat = {.desc1="Complete 10 squats.",.desc2="",.requirement=10,.progress=0,.type=1,.gold=10,.xp=5,.valid=true,.active=false};
        return q_squat;

        break;
      }
      case(2): // jumping jack quest
      {
        requirement = random(5*num)+min_requirement_2; // maxes out at ~2,510
        if (requirement > 1000) requirement = 1000;
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
        // ONLY FOR TESTING 
        Quest q_jump = {.desc1="Complete 10",.desc2=" jumping jacks.",.requirement=10,.progress=0,.type=2,.gold=10,.xp=5,.valid=true,.active=false};
        return q_jump; 

        break;
      }
    }

    int gold = (int)((requirement / (random(5)+2))*amp) + 1;
    int xp = (int)((requirement/(random(3)+2))*amp) + 3;

    Quest q = {.desc1=desc1,.desc2=desc2,.requirement=requirement,.progress=0,.type=type,.gold=gold,.xp=xp,.valid=true,.active=false};
    return q;
 }

void completeQuest()
{
  // pay out reward
  p.gold += t.curQuests[quest_selected-1].gold;
  p.xp += t.curQuests[quest_selected-1].xp;

  checkForLevelUp();

  // provide new quest if applicable
  if (p.questRerolls[t.location] > 0)
  {
    t.curQuests[quest_selected-1] = createQuest(p.level, p.location);
    p.questRerolls[t.location]--;
  }
  else if (t.curQuests[quest_selected-1].valid) // make the quest invalid
  {
    invalidateQuest();
  }
  quest_selected = 0;
}

void jumpingJacks(sensors_event_t a)
{
  float x = a.acceleration.x;
  float y = a.acceleration.y;
  float z = a.acceleration.z;

  // calculate the amplitude
  float amp = sqrt(pow((x),2)+pow((y),2)+pow((z),2)) - correction;

  if (amp < 0) amp *= -1;

  if (amp >= jumpThresholdMax && cyclesJump >= 50) // more than a half second has passed since last jumping jack
  {
    t.curQuests[quest_selected-1].progress++;
    cyclesJump = 0;

    if (t.curQuests[quest_selected-1].progress >= t.curQuests[quest_selected-1].requirement)
    {
      jackTaskActive = false;
      completeQuest();
    }
  }

//  if (amp >= jumpThresholdMax)
//  {
//    cyclesJump = 0;
//    jumpFlag = true;
//  }
//  
//  if (jumpFlag && cyclesJump >= 100) // 2 second since the max value reached
//  {
//    jumpFlag = false;
//  }
//  else if (jumpFlag && amp <= jumpThresholdMin) // record a jumping jack
//  {
//    t.curQuests[quest_selected-1].progress++;
//    jumpFlag = false;
//
//    if (t.curQuests[quest_selected-1].progress >= t.curQuests[quest_selected-1].requirement)
//    {
//      jackTaskActive = false;
//      completeQuest();
//    }
//  }
  cyclesJump++;
}

void squats(sensors_event_t a)
{
  float x = a.acceleration.x;
  float y = a.acceleration.y;
  float z = a.acceleration.z;

  // calculate the amplitude
  float amp = sqrt(pow((x),2)+pow((y),2)+pow((z),2)) - correction;

  if (amp <= squatThresholdMin)
  {
    cyclesSquat = 0;
    squatFlag = true;
  }
  
  if (squatFlag && cyclesSquat >= 100) // 2 seconds since the min value reached
  {
    squatFlag = false;
  }
  else if (squatFlag && amp >= squatThresholdMax) // record a squat
  {
    t.curQuests[quest_selected-1].progress++;
    squatFlag = false;

    if (t.curQuests[quest_selected-1].progress >= t.curQuests[quest_selected-1].requirement)
    {
      squatTaskActive = false;
      completeQuest();
    }
  }
  cyclesSquat++;
}

void beginQuest()
{
  if (!t.curQuests[quest_selected-1].valid)
  {
    return;
  }
  t.curQuests[quest_selected-1].active = true;
  // what type of quest are we completing?
  switch(t.curQuests[quest_selected-1].type)
  {
    case (0): // walking, simplest case
    {
      stepTaskActive = true;
      break;
    }
    case(1): // squats
    {
      squatTaskActive = true;
      break;
    }
    case(2): // jumping jacks
    {
      jackTaskActive = true;
      break;
    }
  }
}

void stopQuest()
{
  t.curQuests[quest_selected-1].active = false;
  // what type of quest are we completing?
  switch(t.curQuests[quest_selected-1].type)
  {
    case (0): // walking, simplest case
    {
      stepTaskActive = false;
      break;
    }
    case(1): // squats
    {
      squatTaskActive = false;
      break;
    }
    case(2): // jumping jacks
    {
      jackTaskActive = false;
      break;
    }
  }
}

void invalidateQuest()
{
  t.curQuests[quest_selected-1].valid = false;
  t.curQuests[quest_selected-1].desc1 = "No quests available";
  t.curQuests[quest_selected-1].desc2 = "Resets at 12:00";
  t.curQuests[quest_selected-1].gold = 0;
  t.curQuests[quest_selected-1].xp = 0;
  t.curQuests[quest_selected-1].requirement = 0;
}
 
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
              createPopup("Only one quest can be active at a time!");
            }
            return;
          }
          case (2):
          {
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
              createPopup("Only one quest can be active at a time!");
            }
            return;
          }
          case(3):
          {
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
              createPopup("Only one quest can be active at a time!");
            }
            
            return;
          }
        }
      }
    }
  }
  switch(quest_selected) // add checks for the trash and begin button
  {
    case(0):
    {
      return;
    } // end of case 0
    case(1):
    {
      for (int i = 0; i < 2; i++)
      {
        if (questButtons1[i].x1 <= x && questButtons1[i].x2 >= x)
        {
          if (questButtons1[i].y1 <= y && questButtons1[i].y2 >= y)
          {
            if (i == 0) // trash button
            {
              String s = "Are you sure you want to delete this quest? You will have ";
              s = s + (p.questRerolls[t.location]-1);
              s = s + " quests left. (Quests reset at 12:00)";
              boolean ans = createYesNoPopup(s); 

              if (ans)
              {
                if (p.questRerolls[t.location] > 0)
                {
                  t.curQuests[quest_selected-1] = createQuest(p.level, p.location);
                  p.questRerolls[t.location]--;
                }
                else if (t.curQuests[quest_selected-1].valid) // make the quest invalid
                {
                  invalidateQuest();
                }
                quest_selected = 0;
              }
            }
            else // begin button
            {
              if (t.curQuests[quest_selected-1].active) stopQuest();
              else beginQuest();
            }
          }
        }
      }
      break;
    } // end case 1
    case(2):
    {
      for (int i = 0; i < 2; i++)
      {
        if (questButtons2[i].x1 <= x && questButtons2[i].x2 >= x)
        {
          if (questButtons2[i].y1 <= y && questButtons2[i].y2 >= y)
          {
            if (i == 0) // trash button
            {
              String s = "Are you sure you want to delete this quest? You will have ";
              s = s + (p.questRerolls[t.location]-1);
              s = s + " quests left. (Quests reset at 12:00)";
              boolean ans = createYesNoPopup(s); 

              if (ans)
              {
                if (p.questRerolls[t.location] > 0)
                {
                  t.curQuests[quest_selected-1] = createQuest(p.level, p.location);
                  p.questRerolls[t.location]--;
                }
                else if (t.curQuests[quest_selected-1].valid) // make the quest invalid
                {
                  invalidateQuest();
                }
                quest_selected = 0;
              }
            }
            else // begin button
            {
              if (t.curQuests[quest_selected-1].active) stopQuest();
              else beginQuest();
            }
          }
        }
      }
      break;
    } // end case 2
    case(3):
    {
      for (int i = 0; i < 2; i++)
      {
        if (questButtons3[i].x1 <= x && questButtons3[i].x2 >= x)
        {
          if (questButtons3[i].y1 <= y && questButtons3[i].y2 >= y)
          {
            if (i == 0) // trash button
            {
              String s = "Are you sure you want to delete this quest? You will have ";
              s = s + (p.questRerolls[t.location]-1);
              s = s + " quests left. (Quests reset at 12:00)";
              boolean ans = createYesNoPopup(s); 

              if (ans)
              {
                if (p.questRerolls[t.location] > 0)
                {
                  t.curQuests[quest_selected-1] = createQuest(p.level, p.location);
                  p.questRerolls[t.location]--;
                }
                else if (t.curQuests[quest_selected-1].valid) // make the quest invalid
                {
                  invalidateQuest();
                }
                quest_selected = 0;
              }
            }
            else // begin button
            {
              if (t.curQuests[quest_selected-1].active) stopQuest();
              else beginQuest();
            }
          }
        }
      }
      break;
    } // end case 3
  } // end switch
}

void displayQuests(Quest quests[])
{
  background.setTextSize(1);
  background.setTextColor(TFT_BLACK);
  for (int i = 0; i < 3; i++)
  {
    switch(i)
    {
      case(0):
      {
        background.setTextSize(1);
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
          background.setTextSize(1);
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
          background.setTextSize(1);
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
