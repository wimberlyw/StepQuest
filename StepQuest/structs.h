/* structs.h stores structures for use in multiple files */
#ifndef STRUCTS_H
#define STRUCTS_H
#include <Arduino.h>
#include <TFT_eSPI.h>
#include <CST816S.h>

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
  String quest_name; // name for user
  int requirement; // requirement to complete
  int progress;
  // need to add something for a time limit
  int type; // type of exercise
  int gold; // gold rewarded on completion
  int xp; // xp rewarded on completion
 }Quest;

typedef struct Town{
  int quests_per_12hr;
  int items_per_12hr;
  Quest curQuests[3];
}Town;

struct timekeeping{
  unsigned int _hours;
  unsigned int _minutes; 
  unsigned int  _seconds;
  unsigned long previousMillisTime;
  char *  _days;
  bool connection;
};

#endif
