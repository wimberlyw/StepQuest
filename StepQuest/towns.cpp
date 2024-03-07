#include "towns.h"
#include "quests.h"

Location quest_but = {.x1=50,.x2=180,.y1=80,.y2=120};
Location shop_but = {.x1=50,.x2=180,.y1=130,.y2=170};
Location townLocations[2] = {quest_but,shop_but};

extern boolean shopDisplayed;
extern boolean questDisplayed;


Town setupTown(int level, int location)
{
  Town t;
  // Which town are we in?
  switch(location)
  {
    case(0):
    {
      t.quests_per_12hr=10;
      t.items_per_12hr=5;
      break;
    }
    case(1):
    {
      t.quests_per_12hr=20;
      t.items_per_12hr=10;
      break;
    }
    case(3):
    {
      t.quests_per_12hr=30;
      t.items_per_12hr=15;
      break;
    }
  }//end switch

  Quest quests[3];

  for (int i = 0; i < 3; i++)
  {
    t.curQuests[i] = createQuest(level, location);
  }

  return t;
}

void displayShopMenu()
{
  Serial.print("Shop");
  shopDisplayed = true;
}

void displayQuestMenu()
{
  Serial.print("Quest");
  questDisplayed = true;
}

void checkTownLocation(int x, int y) // figures out what button has been pressed on town menu
{
  for (int i = 0; i < 2; i++)
  {
    if (townLocations[i].x1 <= x && townLocations[i].x2 >= x)
    {
      if (townLocations[i].y1 <= y && townLocations[i].y2 >= y)
      {
        switch(i)
        {
          case (0):
          {
            displayQuestMenu();
            break;
          }
          case (1):
          {
            displayShopMenu();
            break;
          }
        }
      }
    }
  }
}

void refreshTown(Town* t, Player p) // only refreshes quests atm
{
  // Refresh how many items and quests per 12hr
  switch(p.location)
  {
    case(0):
    {
      t->quests_per_12hr=10;
      t->items_per_12hr=5;
      break;
    }
    case(1):
    {
      t->quests_per_12hr=20;
      t->items_per_12hr=10;
      break;
    }
    case(3):
    {
      t->quests_per_12hr=30;
      t->items_per_12hr=15;
      break;
    }
  }//end switch

  // refresh any invalid quests
  for (int i = 0; i < 3; i++)
  {
    if (!t->curQuests[i].valid)
    {
      t->curQuests[i] = createQuest(p.level,p.location);
    }
  }
}
