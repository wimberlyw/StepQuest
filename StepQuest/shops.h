#ifndef SHOPS_H
#define SHOPS_H
#include "structs.h"

Item selectItem();

void returnShopItem(Item item);

boolean buyItem(Item item);

void checkShopLocation(int x, int y);

void displayItems();

Item selectDungeonItem(int level);


void recallShop();

#endif
