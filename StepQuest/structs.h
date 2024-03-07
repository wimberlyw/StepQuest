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
};

typedef struct Location{
  int x1;
  int x2;
  int y1;
  int y2;
  // possibly add pointer to appropriate town/dungeon here
}Location;

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
  int quests_per_12hr;
  int items_per_12hr;
  Quest curQuests[3];
}Town;

typedef struct Player{
  int location;
  int level;
  int gold;
  int xp;
  int xpToNextLvl;
}Player;

#endif
