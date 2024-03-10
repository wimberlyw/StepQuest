#ifndef WIFITIME_H
#define WIFITIME_H
#include <WiFi.h>
//#include "time.h"
//#include "timekeeping.h"

#define SECONDINTERVAL 1000


 
void printLocalTime();

void connectToWifi(struct timekeeping *timekeeperPTR);

void updateTimeWifi(struct timekeeping *timekeeperPTR);

#endif
