#include "timekeeping.h"
#include "structs.h"

  void processOfflineTimekeeping(struct timekeeping *timekeeperPTR){
   // get the previous values from the struct
   Serial.println("timekeeping");
   bool amConnected = timekeeperPTR->connection;
   Serial.println(amConnected);
   if(!amConnected)
   {
    Serial.println("inside");
     unsigned int prevMillisTime = timekeeperPTR->previousMillisTime;
     unsigned int H = timekeeperPTR->_hours;
     unsigned int M = timekeeperPTR->_minutes; 
     unsigned int S = timekeeperPTR->_seconds;
  
     if ((unsigned long)(millis() - prevMillisTime) >= SECONDINTERVAL) {
      S++;
      //idletime--;
      prevMillisTime = millis();
      Serial.println(S + " secs");
      }
  
      //time keeping
      if (S > 59)
      {
        M++;
        S = 0;
      }
      if (M > 59)
      {
        H++;
        M = 0;
      }
      if (H > 12)
      {
        H = 1;
      }
      
     // Update the struct
     timekeeperPTR->previousMillisTime = prevMillisTime;
     timekeeperPTR->_hours = H;
     timekeeperPTR->_minutes = M;
     timekeeperPTR->_seconds = S;
  } // End if ! connected
}
