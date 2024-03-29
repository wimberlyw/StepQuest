/*
 * Quests.h defines the quests (for use in dungeons and towns)
 */
#ifndef QUESTS_H
#define QUESTS_H
#include "structs.h"

void completeQuestPopup();

Quest createQuest(int level, int location);

void displayQuests(Quest quests[]);

void checkQuestLocation(int x, int y);

void invalidateQuest();

void beginQuest();

void completeQuest();

void jumpingJacks(sensors_event_t a);

void squats(sensors_event_t a);

#endif
