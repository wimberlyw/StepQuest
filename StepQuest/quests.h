/*
 * Quests.h defines the quests (for use in dungeons and towns)
 */
#ifndef QUESTS_H
#define QUESTS_H
#include "structs.h"

Quest createQuest(int level, int location);

void displayQuests(Quest quests[]);

void checkQuestLocation(int x, int y);

#endif
