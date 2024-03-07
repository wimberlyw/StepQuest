#ifndef POPUP_H
#define POPUP_H
#include "structs.h"

#define SCREENDEBOUNCE 500

boolean checkInteraction();

void checkForTap();

void writePopupText(String s);

boolean createYesNoPopup(String s);

void createPopup(String s);
#endif
