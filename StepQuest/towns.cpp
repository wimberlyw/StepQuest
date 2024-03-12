#include "towns.h"
#include "quests.h"
#include "shops.h"

Location quest_but = {.x1=50,.x2=180,.y1=80,.y2=120};
Location shop_but = {.x1=50,.x2=180,.y1=130,.y2=170};
Location townLocations[2] = {quest_but,shop_but};

extern boolean shopDisplayed;
extern boolean questDisplayed;
extern Town t;
extern Player p;


void setupTown(int level, int location)
{
  // Which town are we in?
  switch(location)
  {
    case(0):
    {
      t.location = 0;
      break;
    }
    case(1):
    {
      t.location = 1;
      break;
    }
    case(3):
    {
      t.location = 2;
      break;
    }
  }//end switch

  Quest quests[3];
  Item items[3];

  for (int i = 0; i < 3; i++)
  {
    t.curQuests[i] = createQuest(level, location);
    if (p.shopItemsRem[t.location])
    {
      t.shopItems[i] = p.shopItems[t.location][i];
    }
    else
    {
      t.shopItems[i] = selectItem();
    }
  }

  return;
}

void displayShopMenu()
{
  if(!questDisplayed) shopDisplayed = true;
}

void displayQuestMenu()
{
  if(!shopDisplayed) questDisplayed = true;
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

void refreshTowns()
{
  p.questRerolls[0] = 10;
  p.questRerolls[1] = 20;
  p.questRerolls[2] = 30;
  p.itemRerolls[0] = 5;
  p.itemRerolls[1] = 10;
  p.itemRerolls[2] = 15;

  if (p.location == 0 || p.location == 1 || p.location == 3) // we are in a town!
  {
    // refresh any invalid quests and change all shop items
    for (int i = 0; i < 3; i++)
    {
      if (!t.curQuests[i].valid)
      {
        t.curQuests[i] = createQuest(p.level,p.location);
      }
  
      returnShopItem(t.shopItems[i]);
      t.shopItems[i] = selectItem();
    }
  }

  for (int i = 0; i < 3; i++)
  {
    p.shopItemsRem[i] = false;
  }
}
