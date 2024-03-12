#include "player.h"

extern Player p;
Item sword = {.itemName="Training Sword",.type=0,.level=1,.price=10,.obtainable=true,.place=0};
Item helmet = {.itemName="Padded Helmet",.type=1,.level=1,.price=10,.obtainable=true,.place=0};
Item chestpiece = {.itemName="Padded Chestpiece",.type=2,.level=1,.price=15,.obtainable=true,.place=0};

void checkForLevelUp()
{
  while (p.xp >= p.xpToNextLvl) // we leveled up!
  {
    p.level++;
    p.xp -= p.xpToNextLvl;
    p.xpToNextLvl += 100;
  }
}

Player setupPlayer()
{
  Player p;

  p.location = 0; // start location
  p.level = 1;
  p.gold = 500;
  p.xp = 0;
  p.xpToNextLvl = 100;
  p.path = -1;
  p.items[0] = sword;
  p.items[1] = helmet;
  p.items[2] = chestpiece;
  p.questRerolls[0] = 10;
  p.questRerolls[1] = 20;
  p.questRerolls[2] = 30;
  p.itemRerolls[0] = 5;
  p.itemRerolls[1] = 10;
  p.itemRerolls[2] = 15;

  for (int i = 0; i < 3; i++)
  {
    p.itemLevels[i] = p.items[0].level;
    p.shopItemsRem[i] = false;
  }

  return p;
}
