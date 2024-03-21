/* structs.h stores structures for use in multiple files */
#ifndef STRUCTS_H
#define STRUCTS_H
#include <Arduino.h>
#include <TFT_eSPI.h>
#include <CST816S.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_MPU6050.h>

enum screen {
  HOMESCREEN = 0,
  STATUSSCREEN,
  WORLDMAP,
   SETTINGS,
   TOWNMENU,
   TIMEMENU
};

typedef struct Location{
  int x1;
  int x2;
  int y1;
  int y2;
  // possibly add pointer to appropriate town/dungeon here
}Location;

typedef struct Item{
  String itemName;
  int type; // 0 = weapon, 1 = headpiece, 2 = chestpiece
  int level; // if weapon then power level, if armor then armor level
  int price;
  boolean obtainable; // true if still available
  int place; // position in the shop array
}Item;

typedef struct Quest{
  String desc1; // description for user
  String desc2;
  int requirement; // requirement to complete
  int progress;
  // need to add something for a time limit
  int type; // type of exercise
  int gold; // gold rewarded on completion
  int xp; // xp rewarded on completion
  boolean valid; // if this quest can be completed (used for when player runs out of quests)
  boolean active; // lets us know if this quest is active
 }Quest;

typedef struct Town{
  int location;
  Quest curQuests[3];
  Item shopItems[3];
}Town;


typedef struct Player{
  int location;
  int level;
  int gold;
  float xp;
  int xpToNextLvl;
  int path; // what path are we on? 0-3, -1 means not travelling
  int itemLevels[3]; // keeps max stat for each item the player can hold
  Item items[3]; // keeps the best item player has stored for viewing
  int questRerolls[3]; // keeps amount of rerolls per 12 hr stored to avoid abuse
  int itemRerolls[3]; // keeps amount of rerolls per 12 hr stored to avoid abuse
  boolean shopItemsRem[3]; // keeps track of whether the shop state is stored
  Item shopItems[3][3]; // keeps track of current state of shop, set when town is setup and updated when leaving town
}Player;

struct timekeeping{
  unsigned int _hours;
  unsigned int _minutes; 
  unsigned int  _seconds;
  unsigned long previousMillisTime;
  char *  _days;
  bool connection;
  int idleTime;
  // Time Setting Screen
  bool settingTime;
};

#endif
