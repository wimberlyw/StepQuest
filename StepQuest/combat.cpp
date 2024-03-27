#include "combat.h"
#include "popup.h"

extern int stepsToCombat;
extern boolean inCombat;
extern int wins;
extern int totalBattles;
extern int goldGained;
extern int xpGained;
extern Player p;

// Called at start of travel or entrance of dungeon location
void beginCombat()
{
  wins = 0;
  totalBattles = 0;
  goldGained = 0;
  xpGained = 0;
  stepsToCombat = random(96) + 5; // 5-100 steps
  inCombat = true;
}

void combat()
{
  totalBattles++;
  int enemyRoll = random(2*p.level) + 1; // number between 1-(2*player level)
  int playerRoll = (p.itemLevels[0]+p.itemLevels[1]+p.itemLevels[2]);

  if (enemyRoll > 150) enemyRoll = 150; // max stats in game is 150, don't go above this or player will always lose

  // player wins
  if (playerRoll >= enemyRoll)
  {
    wins++;
    goldGained += (random((int)(enemyRoll/3)) + 1);
    xpGained += (random((int)(enemyRoll/2)) + 1);
  }
  else // player loses
  {
    goldGained -= (random((int)(enemyRoll/3)) + 1);
  }

  // need to reset when combat occurs next
  stepsToCombat = random(96) + 5; // 5-100 steps
}

void endCombat()
{
  inCombat = false;
  p.gold += goldGained;
  if (p.gold < 0) p.gold = 0;
  p.xp += xpGained;
}

void battleResults()
{
  String s = "Battle Results         Wins: ";
  s = s + wins; 
  s = s + " Loses: ";
  s = s + (totalBattles-wins);
  s = s + "         Gold: ";
  s = s + goldGained;
  s = s + " XP: ";
  s = s + xpGained;

  createPopup(s);
}
