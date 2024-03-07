#include "player.h"

extern Player p;

void checkForLevelUp()
{
  while (p.xp >= p.xpToNextLvl) // we leveled up!
  {
    p.level++;
    p.xp -= p.xpToNextLvl;
    p.xpToNextLvl += 100;
  }
}
