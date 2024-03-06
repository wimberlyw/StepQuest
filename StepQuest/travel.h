/* Implements in world map location and the travel 
 *  system to traverse those locations
 */
 /* 
  *  Remaining TODOs: Some popup messages that shouldn't have Yes or No prompts do due to the way it is implemeneted.
  *  Need to showcase where the player currently is in some way (all commented out rn due to complexity, might simply 
  *  to just showing the town or path player is on later).  Need to figure out a better way to notify players that travel has
  *  been completed (using vibration motor). Need to add the random combat mechanic during travel (later since also used by dungeon).
  */
  
/*
 * Location structure defines where location is on
 * screen and how many steps to reach it 
 * (assuming linear progression between locations)
 */
#ifndef TRAVEL_H
#define TRAVEL_H

#include "structs.h"
#define SCREENDEBOUNCE 500

void writePopupText(String s);

boolean checkInteraction();

String locationName(int location);

void finishTravel();

void beginTravel(int location);

void checkMapLocation(int x, int y);

#endif
