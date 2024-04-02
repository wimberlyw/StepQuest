#include "wifitime.h"
#include "structs.h"
#include "time.h"
#include "popup.h"

const char* ssid       = "belkin.a92";
const char* password   = "Pepper01";

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = -18000  ;
const int   daylightOffset_sec = 3600;

void updateTimeWifi(struct timekeeping *timekeeperPTR){
    bool wifiConnected = timekeeperPTR->connection;
    if(wifiConnected){
      unsigned int prevMillisTime = timekeeperPTR->previousMillisTime;
      // If time > a second
      if ((unsigned long)(millis() - prevMillisTime) >= SECONDINTERVAL) {  
        //init and get the time
        configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
        struct tm timeinfo;
        if(!getLocalTime(&timeinfo)){
          Serial.println("Failed to obtain time");
          return;
        }
     
      Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
      unsigned int prevMillisTime = timekeeperPTR->previousMillisTime;
      unsigned int H = timekeeperPTR->_hours;
      unsigned int M = timekeeperPTR->_minutes; 
      unsigned int S = timekeeperPTR->_seconds;
      unsigned int D = 0;
      
      // Update locally
      H = timeinfo.tm_hour;
      M = timeinfo.tm_min;
      S = timeinfo.tm_sec;
      D = timeinfo.tm_wday;

      if(H>12){
        H = H-12;
      }
      
      switch(D)
      {
        case 0:
        {
          timekeeperPTR->_days = "SUNDAY";
          break;
        }
        case 1:
        {
          timekeeperPTR->_days = "MONDAY";
          break;
        }
        case 2:
        {
          timekeeperPTR->_days = "TUESDAY";
          break;
        }
        case 3:
        {
          timekeeperPTR->_days = "WEDNESDAY";
          break;
        }
        case(4):
        {
          timekeeperPTR->_days = "THURSDAY";
          break;
        }
        case(5):
        {
          timekeeperPTR->_days = "FRIDAY";
          break;
        }
        case(6):
        {
          timekeeperPTR->_days = "SATURDAY";
          break;
        }
      }
      
      
      prevMillisTime = millis();

      // update the global struct
     timekeeperPTR->previousMillisTime = prevMillisTime;
     timekeeperPTR->_hours = H;
     timekeeperPTR->_minutes = M;
     timekeeperPTR->_seconds = S;
     }
  }
    
}

void connectToWifi(struct timekeeping *timekeeperPTR){
  
  bool amConnected = timekeeperPTR->connection;
  amConnected = !amConnected;
  
  if(amConnected == true)
  {
  //connect to WiFi
  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      String s = "connecting";
      
      delay(500);
      Serial.print(".");
      createBlankPopup(s);
      s += ".";
  }
  Serial.println(" CONNECTED");
  } // end if *connection = true
  else{
  //disconnect WiFi as it's no longer needed
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  Serial.println("Disconnected");
  
  }
  timekeeperPTR->connection = amConnected;
  Serial.println("amconnected: " + amConnected);
}

void printLocalTime()
{
  
  //init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}
