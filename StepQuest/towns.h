/*
 * Town.h defines the town menus including the shops and quest boards
 */
#ifndef TOWNS_H
#define TOWNS_H

#include "structs.h"

Town setupTown(int level, int location);

void diplayShopMenu();

void displayQuestMenu();

void checkTownLocation(int x, int y);

void refreshTown(Town* t, Player p);

#endif
