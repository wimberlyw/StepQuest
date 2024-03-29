/*
 * Town.h defines the town menus including the shops and quest boards
 */
#ifndef DUNGS_H
#define DUNGS_H

//#include "structs.h"

void setupDungeon(int level, int location, dungeon* D);

//void displayShopMenu();

void drawDungeonQuests(dungeon D);

void checkDungeonClick(int x, int y, dungeon* D);
//
//Item selectDungeonItem();

//void refreshDungeons();

#endif
