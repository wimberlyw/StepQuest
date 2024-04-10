#include <TFT_eSPI.h>       // Include the graphics library
#include <CST816S.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include "ESP32TimerInterrupt.h"
#include <Wire.h>
#include "Char1.h"
#include "map.h"
#include "castlecropped.h"
#include "dungeoncroppped.h"
#include "forestcropped.h"
#include "steps.h"
#include "travel.h"
#include "dungeons.h"
#include "player.h"
#include "settingsMenu.h"
//#include "wifitime.h"
#include "quests.h"
#include "towns.h"
#include "shops.h"
#include "timekeeping.h"
#include "combat.h"
#include "dungeons.h"

#define I2C_SDA 35
#define I2C_SCL 36
#define SCREENDEBOUNCE 300
#define BUTTONDEBOUNCE 200
#define ANIMINTERVAL 200
#define STEP_TIMER_INTERVAL_MS 20
#define SECOND 1000

Adafruit_MPU6050 mpu;

/*TODO: Use .setTextSize(2) and reorganize things to make it a bit more legible*/


TwoWire I2CMPU = TwoWire(1);

// Graphics
TFT_eSPI tft = TFT_eSPI();  // Create object "tft"
TFT_eSprite Char = TFT_eSprite(&tft);
TFT_eSprite background = TFT_eSprite(&tft);
//TFT_eSprite worldmap = TFT_eSprite(&tft);
//TFT_eSprite foreground = TFT_eSprite(&tft);
TFT_eSprite popup = TFT_eSprite(&tft);
TFT_eSprite popupText = TFT_eSprite(&tft);
//TFT_eSprite Dungeon = TFT_eSprite(&tft);
//TFT_eSprite Forest = TFT_eSprite(&tft);
//TFT_eSprite Castle = TFT_eSprite(&tft);
//TFT_eSprite townMenu = TFT_eSprite(&tft);
CST816S touch(5, 6, 9, 3);  // sda, scl, rst, irq

// Screen Backlight
uint8_t blValue;

// Timers

unsigned long previousMillisScreen =0;
unsigned long previousMillisButton = 0;
unsigned long previousMillisIdle = 0;

/* struct timekeeping{
  unsigned int _hours;
  unsigned int _minutes; 
  unsigned int  _seconds;
  unsigned long previousMillisTime;
  bool connection;
};*/

struct timekeeping timekeeper;

struct timekeeping *timekeeperPtr = &timekeeper;

unsigned long currAnim, prevAnim = 0; //animation Timers
const int interval = 1000;
const int animInterval = 200;
int charFrame = 0;


const int button1 = 4;
const int button2 = 7;
int screen = 0;
int prevScreen = 0;
String gest = "";

int buttonState1 =0;
int buttonState2=0;
bool asleep = false;
bool checkingStats = false;

// Steps & Travel
volatile unsigned int steps = 0;
ESP32Timer StepTimer(0);
//hw_timer_t *StepTimer = NULL;
volatile int stepFlag = 0;
sensors_event_t a, g, temp;
boolean travelling = false;
volatile int travelSteps = 0;
volatile int tempSteps = 0;
volatile int stepsToNextPath = 0; 
volatile int totalTravelSteps = 0;

// enter the drift of the accelerometer (unique to each MPU6050)
float x_drift = 0.40;
float y_drift = 0.45;
float z_drift = 10;

// offsets the gravity and x,y,z drift in order to get an amplitude close to 0 regardless of orientation
float correction = sqrt(pow((x_drift),2)+pow((y_drift),2)+pow((z_drift),2));

// Shop & Quest items
Town t;
boolean shopDisplayed = false;
boolean questDisplayed = false;
int quest_selected = 0;
int dungeon_quest_selected = 0;
int player_level = 50;
boolean stepTaskActive = false;
boolean jackTaskActive = false;
boolean squatTaskActive = false;
boolean left = false;
boolean questCompleted = false;

//Dungeon
dungeon D;

// Combat
int stepsToCombat = 0; // randomly generated number to determine ater how many steps combat occurs
boolean inCombat = false;
int wins = 0;
int totalBattles = 0;
int goldGained = 0;
int xpGained = 0;

// Player
Player p;

// Dungeon and Town Refresh
boolean refresh = false;

bool IRAM_ATTR StepTimerHandler(void * timerNo)
{
  stepFlag = 1;

  return true;
}

void attachStepTimerInterruptTask(void *pvParameters)
{
  if (StepTimer.attachInterruptInterval(STEP_TIMER_INTERVAL_MS * 1000, StepTimerHandler))
  {
    Serial.println("StepTimer started");
  }

  vTaskDelete(NULL);
}


void loop1(void *pvParameters) {
   while(1)
   {
    /*
      Touch Screen data
    if (touch.available()) {
      Serial.print(touch.gesture());
      Serial.print("\t");
      Serial.print(touch.data.points);
      Serial.print("\t");
      Serial.print(touch.data.event);
      Serial.print("\t");
      Serial.print(touch.data.x);
      Serial.print("\t");
      Serial.println(touch.data.y);
    }
    */
    background.setColorDepth(8);
    background.createSprite(240,240);
    background.setSwapBytes(true);
     // Take inputs
     
     checkIdleTime();

     if (refresh)
     {
       refreshTowns(); // untested since time isn't fully implemented yet
       setupDungeon(p.level, p.location, &D);
       refresh = false;
     }

     if (left)// you can't do tasks outside of towns/dungeons
     {
       stepTaskActive = false;
       jackTaskActive = false;
       squatTaskActive = false;
       quest_selected == 0;
       left = false;
     }

     if (travelling && travelSteps <= 0)
    {
      finishTravel();
      // setup the location
      if (p.location == 0 || p.location == 1 || p.location == 3)
      {
        setupTown(p.level,p.location);
      }
      else
      {
        setupDungeon(p.level, p.location, &D);
        p.currStatus = INDUNGEON;
      }
    }

    if (questCompleted)
    {
      questCompleted = false;
      completeQuestPopup();
    }
     
    
    //background.fillSprite(TFT_SKYBLUE);
  
    //// *********************** Screen Handling **********************************/
    // Show the appropriate screen based on the button
    switch(screen)
    {
      case HOMESCREEN:
      {   
        if (shopDisplayed) shopDisplayed = false;
        if (questDisplayed) questDisplayed = false;

  
          background.fillScreen(TFT_BLACK);
   
          background.setTextColor(TFT_RED, TFT_BLACK);
        if (p.currStatus == INTRAVEL)
          {
            background.setCursor(45, 38, 4);
            background.print("Travel: ");
            background.print(totalTravelSteps - travelSteps);
            background.print("/");
            background.println(totalTravelSteps);
        }
          background.setCursor(70, 66, 4);
          background.print("Steps: ");
          background.print(steps);
          background.setCursor(10, 100, 7);
          background.setTextColor(TFT_WHITE, TFT_BLACK);
          if(timekeeper._hours <10){
            background.print(" ");
          }
          background.print(timekeeper._hours);
          background.print(":");
          if (timekeeper._minutes < 10) {
            background.print('0');
          }
          background.print(timekeeper._minutes);
          background.print(":");
          //background.setCursor(175, 65, 2);
          if (timekeeper._seconds < 10) {
            background.print("0");
          }
          background.print(timekeeper._seconds);
          if(timekeeper.connection == true)
          {
             background.setCursor(59, 165, 4);
             background.println(timekeeper._days);
          }

 
        
        
        break;
        
      }// End Case 0
      case STATUSSCREEN:
      {
        
        Char.createSprite(96,96);
        Char.setSwapBytes(false);
        //Char.setColorDepth(8);
        // Set the font colour to be white with a black background
        
      

        background.setTextColor(TFT_BLACK);
        background.setCursor(75, 20, 4);
        switch(p.currStatus){
          case 0:
            {
              background.pushImage(0,0, 240, 240, castlecropped);
              background.print("In Town");
              break;
            }
          case 1:
            {
              background.setTextColor(TFT_WHITE);
              background.pushImage(0,0, 240, 240, forestcropped);
              background.setCursor(69, 20, 4);
              background.print("Traveling");
              //background.setTextColor(TFT_RED);
              background.setCursor(89, 46, 4);
              background.print(totalTravelSteps - travelSteps);
              background.print("/");
              background.println(totalTravelSteps);
              break;
            }
          case 2:
            {
              background.pushImage(0,0, 240, 240, dungeoncropped);
              background.print("Dungeon");
              break;
            }
        }
        //background.setCursor(73, 50, 4);
        background.setTextColor(TFT_GREEN);
        background.setCursor(75, 210, 4);
        background.print("Level: ");
        background.println(p.level);
        // Animate
        if (millis() - prevAnim >= ANIMINTERVAL) //every 300ms
        {
          //re-initialize the timing
          prevAnim = millis();
          charFrame++;
        }
        if(charFrame >2)
        { 
          charFrame = 0;
        }
        Char.pushImage(0,0,96,96, C1[charFrame] ); // push to the created  at 0, 0, size of 96 x 96, the C1 array from char0.h, charFrame index.
        Char.pushToSprite(&background, 70, 108, TFT_BLACK);
        break;
      } // End Case 1
      case WORLDMAP: // map screen
      {
 
        background.pushImage(0,0, 240, 240, map1);

        if (p.path != -1) // we are travelling
        {
          p.currStatus = INTRAVEL;
          switch(p.path)
          {
            case(0): // path 1
            {
              background.drawSpot(130,232,4,TFT_BLACK);
              break;
            } // end of case 0
            case(1): // path 2
            {
              background.drawSpot(123,135,4,TFT_BLACK);
              break;
            } // end of case 1
            case(2): // path 3
            {
              background.drawSpot(120,103,4,TFT_BLACK);
              break;
            } // end of case 2
            case(3): // path 4
            {
              background.drawSpot(120,50,4,TFT_BLACK);
              break;
            } // end of case 3
          } // end of switch
        }
        else
        {
          switch(p.location)
          {
            case(0): // town 1
            {
              p.currStatus = INTOWN;
              background.drawSpot(55,200,4,TFT_BLACK);
              break;
            } // end of town1
            case(1): // town 2
            {
              p.currStatus = INTOWN;
              background.drawSpot(171,200,4,TFT_BLACK);
              break;
            } // end of town2
            case(2): // dungeon 1
            {
              p.currStatus = INDUNGEON;
              background.drawSpot(210,107,4,TFT_BLACK);
              break;
            } // end of dungeon 1
            case(3): // town 3
            {
              p.currStatus = INTOWN;
              background.drawSpot(54,95,4,TFT_BLACK);
              break;
            } // end of case 3
            case(4): // dungeon 2
            {
              p.currStatus = INDUNGEON;
              background.drawSpot(152,45,4,TFT_BLACK);
              break;
            } // end of case 4
            
          } // end of switch
        }
        break;
      } // End Case 2
      case SETTINGS:
      {
        if (shopDisplayed) shopDisplayed = false;
        if (questDisplayed) questDisplayed = false;
        background.fillScreen(TFT_BLACK);
        background.setCursor(60, 40, 4);
        background.setTextColor(TFT_YELLOW, TFT_BLACK);
        background.println("SETTINGS");
        if(timekeeper.connection == true){
          background.setTextColor(TFT_GREEN, TFT_BLACK);
          background.setCursor(86, 80, 4);
          background.println("Wifi");
        }
        else{
          background.setTextColor(TFT_RED, TFT_BLACK);
          background.setCursor(86, 80, 4);
          background.println("Wifi");          
        }
        
        background.setTextColor(TFT_WHITE, TFT_BLACK);
        background.setCursor(86, 110, 4);
        background.println("Backlight");
        
        if(timekeeper.connection == false){
          background.setCursor(86, 140, 4);
          background.println("Time");  
          
        }
        
        // fillRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h, int16_t radius, uint16_t color) 
       // background.fillRoundRect(50, 88, 8, 8, 2, TFT_YELLOW);
        background.setTextSize(1);
        break;  
      } // End Case 3
      case TOWNMENU:
      {
        background.setTextSize(1);
        background.setColorDepth(8);
        background.createSprite(240,240);
        background.setSwapBytes(true);
         if (p.location == 0 || p.location == 1 || p.location == 3) // we are in a town
        {
          background.pushImage(0,0, 240, 240, castlecropped);
          if (shopDisplayed)
          {
            background.setTextSize(1);
            background.fillRoundRect(40,20,160,20,1,TFT_BLUE);
            background.setTextColor(TFT_WHITE);
            background.setCursor(110,20);
            background.print("Shop");
            background.fillRoundRect(40,200,160,20,1,TFT_RED);
            background.setCursor(110,210);
            background.print("Exit");
            background.setCursor(50,30);
            background.print(p.itemRerolls[p.location]);
            background.print(" item rerolls remaining.");
            background.fillRoundRect(60,50,120,40,1,TFT_WHITE);
            background.fillRoundRect(60,100,120,40,1,TFT_WHITE);
            background.fillRoundRect(60,150,120,40,1,TFT_WHITE);
            background.fillRoundRect(20,50,36,40,1,TFT_RED);
            background.setCursor(22,65);
            background.print("Change");
            background.fillRoundRect(20,100,36,40,1,TFT_RED);
            background.setCursor(22,115);
            background.print("Change");
            background.fillRoundRect(20,150,36,40,1,TFT_RED);
            background.setCursor(22,165);
            background.print("Change");
            displayItems();
            
          }
          else if (questDisplayed)
          {
            background.setTextSize(1);
            background.fillRoundRect(40,20,160,20,1,TFT_BLUE);
            background.setTextColor(TFT_WHITE);
            //background.setTextSize(1);
            background.setCursor(90,20);
            background.print("Quest Board");
            background.fillRoundRect(40,200,160,20,1,TFT_RED);
            background.setCursor(110,210);
            background.print("Exit");
            background.setCursor(63,30);
            background.print(p.questRerolls[p.location]);
            background.print(" quests remaining.");
  
            switch(quest_selected)
            {
              case(0):
              {
                background.setTextSize(1);
                background.fillRoundRect(60,50,120,40,1,TFT_WHITE);
                background.fillRoundRect(60,100,120,40,1,TFT_WHITE);
                background.fillRoundRect(60,150,120,40,1,TFT_WHITE);
                break;
              }
              case(1):
              {
                background.setTextSize(1);
                background.fillRoundRect(60,50,120,40,1,TFT_CYAN);
                background.drawRoundRect(60,50,120,40,1,TFT_BLACK);
                background.fillRoundRect(184,50,50,40,1,TFT_GREEN);
                background.fillRoundRect(20,50,36,40,1,TFT_RED);
                background.fillRoundRect(60,100,120,40,1,TFT_WHITE);
                background.fillRoundRect(60,150,120,40,1,TFT_WHITE);
                background.setCursor(22,65);
                background.print("Trash");
                background.setTextColor(TFT_BLACK);
                if (t.curQuests[quest_selected-1].active)
                {
                  background.setCursor(186,55);
                  background.print("Active");
                }
                else
                {
                  background.setCursor(186,55);
                  background.print("Begin");
                }
                background.setTextSize(1);
                background.setCursor(186,80);
                background.print(t.curQuests[0].progress);
                background.print("/");
                background.print(t.curQuests[0].requirement);
                break;
              }
              case(2):
              {
                background.setTextSize(1);
                background.fillRoundRect(60,100,120,40,1,TFT_CYAN);
                background.drawRoundRect(60,100,120,40,1,TFT_BLACK);
                background.fillRoundRect(184,100,50,40,1,TFT_GREEN);
                background.fillRoundRect(20,100,36,40,1,TFT_RED);
                background.fillRoundRect(60,50,120,40,1,TFT_WHITE);
                background.fillRoundRect(60,150,120,40,1,TFT_WHITE);
                background.setCursor(22,115);
                background.print("Trash");
                background.setTextColor(TFT_BLACK);
                if (t.curQuests[quest_selected-1].active)
                {
                  background.setCursor(186,105);
                  background.print("Active");
                }
                else
                {
                  background.setCursor(186,105);
                  background.print("Begin");
                }
                background.setTextSize(1);
                background.setCursor(186,130);
                background.print(t.curQuests[1].progress);
                background.print("/");
                background.print(t.curQuests[1].requirement);
                break;
              }
              case(3):
              {
                background.setTextSize(1);
                background.fillRoundRect(60,150,120,40,1,TFT_CYAN);
                background.drawRoundRect(60,150,120,40,1,TFT_BLACK);
                background.fillRoundRect(184,150,50,40,1,TFT_GREEN);
                background.fillRoundRect(20,150,36,40,1,TFT_RED);
                background.fillRoundRect(60,50,120,40,1,TFT_WHITE);
                background.fillRoundRect(60,100,120,40,1,TFT_WHITE);
                background.setCursor(22,165);
                background.print("Trash");
                background.setTextColor(TFT_BLACK);
                if (t.curQuests[quest_selected-1].active)
                {
                  background.setCursor(186,155);
                  background.print("Active");
                }
                else
                {
                  background.setCursor(186,155);
                  background.print("Begin");
                }
                background.setCursor(186,180);
                background.print(t.curQuests[2].progress);
                background.print("/");
                background.print(t.curQuests[2].requirement);
                break;
              }
              
            } // end switch
            // Display the actual quests which are available to the player
            background.setTextSize(1);
            displayQuests(t.curQuests);
            background.setTextSize(1);
          }
          else
          {
            // create Town menu
            background.setTextSize(1);
            background.fillRoundRect(40,20,160,20,1,TFT_BLUE);
            background.fillRoundRect(60,60,120,40,1,TFT_WHITE);
            background.fillRoundRect(60,120,120,40,1,TFT_WHITE);
            background.setTextColor(TFT_WHITE);
            background.setTextSize(1);
            background.setCursor(110,20, 1);
            background.print("Town");
            background.setTextSize(1);
            background.setTextColor(TFT_BLACK);
            background.setCursor(90,80, 1);
            background.print("Quest Board");
            background.setCursor(110,140, 1);
            background.print("Shop");
          }
        }
        else{ // in a dungeon
          if(prevScreen == DUNGEON){
            screen--;  
          }
          else{
            screen++;
          }
          
        }
        
        break;
      } // End Case 4
      case TIMEMENU:
      {
      background.fillScreen(TFT_BLACK);
      background.setCursor(22, 100, 6);
      background.setTextColor(TFT_WHITE, TFT_BLACK);
      if(timekeeper._hours <10){
        background.print(" ");
      }
      background.print(timekeeper._hours);
      background.print(":");
      if (timekeeper._minutes < 10) {
        background.print('0');
      }
      background.print(timekeeper._minutes);
      background.print(":00");
      if(timekeeper.connection == true){
         background.setCursor(22, 130, 6);
         background.println(timekeeper._days);
      }
      // Draw ok
      background.fillRoundRect(0,200,240,60,5,TFT_CYAN);
      background.setCursor(100,210, 4);
      background.setTextColor(TFT_BLACK, TFT_CYAN);
      background.print("OK");
      // Draw Triangles
      // Leftmost
      background.fillTriangle(35,70, 14, 86, 56, 86, TFT_CYAN);    
      background.fillTriangle(35, (70 + 60+ 31), 14, (86 +60), 56, (86+60), TFT_CYAN);    
      // Rightmost
      background.fillTriangle((35+65),70, (14+65), 86, (56+65), 86, TFT_CYAN);    
      background.fillTriangle((35+65), (70 + 60+ 31), (14+65), (86 +60), (56+65), (86+60), TFT_CYAN);
      break;
      } // End Case TIMEMENU
      case STATUS2:
      {
        background.fillScreen(TFT_BLACK);
        background.setTextColor(TFT_GREEN);
        background.setCursor(69, 30, 4);
        background.print("LEVEL: ");
        background.println(p.level);
        background.setCursor(73, 62, 4);
        //background.setCursor(73, 90, 4);
        //background.setCursor(13, 90, 4);
        background.print("Gold: ");
        background.print(p.gold);
        //background.setCursor(126, 93, 4);
        background.setCursor(75, 90, 4);
        //background.setCursor(75, 120, 4);
        background.print("XP: ");
        background.print((int)p.xp);
       //background.setCursor(73, 150, 4);
        background.setCursor(73, 120, 4);
        background.print("ARMOR:");
        background.println((p.itemLevels[0]+p.itemLevels[1]+p.itemLevels[2]));
        //background.setCursor(73, 178, 4);
        if (inCombat)
        {
          int total = getTotalBattles();
          int wins = getWins();
          int losses = total-wins;
          background.setTextColor(TFT_YELLOW);
          background.setCursor(73, 140, 2);
          background.print("Battles: ");
          background.print(total);
          background.setCursor(73,160,2);
          background.print("Wins: ");
          background.print(wins);
          background.print(" Losses: ");
          background.print(losses);
          background.setCursor(73,180,2);
          background.print("Gold Gain: ");
          background.print(getGold());
          background.setCursor(73,200,2);
          background.print("XP Gain: ");
          background.print(getXp());
        }
        break;
      }
      case DUNGEON:
      {
        background.pushImage(0,0, 240, 240, dungeoncropped);  
        background.setTextSize(1);
        if(!D.defeated){
          
          background.fillRoundRect(60,100,120,40,1,TFT_WHITE);
        switch(D.dungeon_quest_selected)
        {
          case 0:
          {
            background.fillRoundRect(60,100,120,40,1,TFT_WHITE);
            break;
          }
          case 1:
          {
            
            
              background.fillRoundRect(60,100,120,40,1,TFT_CYAN);
              background.drawRoundRect(60,100,120,40,1,TFT_BLACK);
              background.fillRoundRect(184,100,50,40,1,TFT_GREEN);
              
               background.setTextColor(TFT_BLACK);
                if (D.currQuests[D.currFloor].active)
                {
                  background.setCursor(186,105);
                  background.print("Active");
                }
                else
                {
                  background.setCursor(186,105);
                  background.print("Begin");
                }
                background.setTextSize(1);
                background.setCursor(186,130);
                background.print(D.currQuests[D.currFloor].progress);
                background.print("/");
                background.print(D.currQuests[D.currFloor].requirement);
                break;
              }
          }

       drawDungeonQuests(D);
       //break;         
        }// end if!d.defeated
        else{ // if dungeon defeated
          background.fillRoundRect(50,100,137,40,1,TFT_WHITE);
          background.setTextColor(TFT_RED);
          background.setCursor(55, 100, 2);
          background.print("Dungeon will ");
          background.setCursor(55, 120, 2);
          background.print("Re-Open In ");
          int hoursLeft = abs(int(timekeeper._hours - 12));
          background.print(hoursLeft);
          background.print(" Hours");
          
        }
        break;
      }// case Dungeon
    }// end switch(screen)
    // Push Background to screen
    background.pushSprite(0,0);
    readScreenGesture();
    readButtons();
  

    // limits for the screen variable
    if (travelling)
    {
      if(checkingStats == true){
        screen = STATUS2;
      }
      else
      {
      if (screen > SETTINGS) screen = HOMESCREEN;
      if (screen < HOMESCREEN) screen = SETTINGS;
      }
    }
    else if(timekeeper.settingTime == true){
      screen = TIMEMENU;
    }
    else if(checkingStats == true){
      screen = STATUS2;
    }

    else if ((timekeeper.settingTime == false) && !travelling && !checkingStats && (!(p.currStatus == INDUNGEON)))
    {
        
        if(screen>TOWNMENU)
        {
            screen = HOMESCREEN;   
        }
        if(screen<HOMESCREEN){
            screen = TOWNMENU;
        }
    }
    else if(p.currStatus == INDUNGEON){
      Serial.println(screen);
       if(screen>DUNGEON)
        {
          screen = HOMESCREEN;
        }

        if(screen<HOMESCREEN){
            screen = DUNGEON;
        }
    }

    background.deleteSprite();
    Char.deleteSprite();  
// Screen sleeping
  
    
  } 

} // End loop1()

void loop2(void *pvParameters)
{
  while(1)
  {
    // if raised, time to read accel data
    if (stepFlag == 1)
    {
      mpu.getEvent(&a, &g, &temp);

      if (squatTaskActive)
      {
        squats(a);
      }
      else if (jackTaskActive)
      {
        jumpingJacks(a);
      }
      else // just walking
      {
        tempSteps = stepAlg(a);
        steps += tempSteps;

        if (inCombat)
        {
          if (stepsToCombat <= 0)
          {
            combat();
          }
          else
          {
            stepsToCombat -= tempSteps;
          }
        }

        if (tempSteps != 0)
        {
          p.xp += .05; // 1 xp every 20 steps
        }

        if (stepTaskActive) // step task, we are in location, not travelling
        {
          if(p.currStatus != INDUNGEON)
            if (t.curQuests[quest_selected-1].progress < t.curQuests[quest_selected-1].requirement)
            {
              t.curQuests[quest_selected-1].progress += tempSteps;
            }
            else
            {
              stepTaskActive = false;
              completeQuest();
            }
          else{ //in a dungeon
            if (D.currQuests[D.currFloor].progress < D.currQuests[D.currFloor].requirement)
            {
              D.currQuests[D.currFloor].progress += tempSteps;
            }
            else
            {
              stepTaskActive = false;
              completeQuest();
            }
          } // end in dungeon else
        }// end if steptask
        else if (travelSteps > 0) // we are travelling rn
        {
          travelSteps -= tempSteps;
          stepsToNextPath -= tempSteps;

          if (stepsToNextPath <= 0)
          {
            if (travelSteps > 0) // not done travelling
            {
              getStepsToNextPath();
            }
          }
        }
      }
      stepFlag = 0;
    }
    // Timekeeping
    if(!timekeeper.settingTime) // don't count while setting time
    {
      if(timekeeper.connection == false)
      {
      processOfflineTimekeeping(timekeeperPtr); 
      }
      else{
        updateTimeWifi(timekeeperPtr); 
      }
    }
        
    if (timekeeper._hours == 12 && timekeeper._minutes == 0 && timekeeper._seconds == 0)
    {
      refresh = true;
    }
    
  }
} // End loop2()

void loop(){}// TimeKeeping


  //// *********************** Functions **********************************//

void readButtons(){
//is it time to poll the buttons?
  if (millis() - previousMillisButton >= BUTTONDEBOUNCE)  //every 300ms
  {
    //re-initialize the timing
    previousMillisButton = millis();
   
  // Read the buttons
   buttonState1 = digitalRead(button1);
   buttonState2 = digitalRead(button2);
  

    if(buttonState1 == LOW ){
      Serial.println("Button1");

      timekeeper.idleTime = 30; // reset idle timer
      
      screen++;
  
    }
    if(buttonState2 == LOW ){
      Serial.println("Button2");
      timekeeper.idleTime = 30; // reset idle timer
      screen--;  
      
    }
    if(buttonState2 == LOW && buttonState1 ==LOW){
      Serial.println("Both");
      timekeeper.idleTime = 30; // reset idle timer
      
    }
  } 
}

void checkIdleTime(){
      
      
  if (millis() - previousMillisIdle < SECOND){
      return;
    }
    previousMillisIdle = millis();
    timekeeper.idleTime--;
    // fight overflow
    if( timekeeper.idleTime < -30){
       timekeeper.idleTime = -1;
    }
    
    //Serial.println(timekeeper.idleTime);
    if(timekeeper.idleTime ==0)
    {
      setScreenState(true);
      asleep = true;
    }
    if(timekeeper.idleTime > 10 && asleep)
    {
      
   
      setScreenState(false);
      asleep = false;
    
    }
    //Serial.println(asleep);
    
}

void setScreenState(bool state){
  //xSerial.println(state);
  if(state == true){
    tft.writecommand(0x10); // screen sleep
    delay(120);
    blValue = 0;
    TFT_SET_BL(&blValue); //  turn screen backlight off
  }
  if (state == false){
    //
    tft.writecommand(0x11);
    blValue = 100;
    TFT_SET_BL(&blValue); //  turn screen backlight offf
  }
}

void readScreenGesture(){
  // If the screen is in the middle of being touched
  if(touch.available()){ 
    // if its asleep, wakeup
   
    //Get the gesture
    gest = touch.gesture();
    // Print to the screen for debug
    Serial.println(gest);

    // Check if the gesture falls outside of debounce time
    if (millis() - previousMillisScreen < SCREENDEBOUNCE){
      return;
    }
    timekeeper.idleTime = 30; // reset idle timer
    if(gest == "SWIPE LEFT"){ 
      //re-initialize the timing
      previousMillisScreen = millis();
      // advance the screen
      prevScreen = screen;
      screen++;
      
    }
    if(gest == "SWIPE RIGHT"){
      previousMillisScreen = millis();
      prevScreen = screen;
      screen--;
    }
    if (gest == "SINGLE CLICK"){
      previousMillisScreen = millis();

      if (screen == WORLDMAP) // map  screen
      {
        checkMapLocation(touch.data.x, touch.data.y);
      }
      else if (screen == TOWNMENU) // quest testing rn
      {
        if (shopDisplayed)
        {
          checkShopLocation(touch.data.x, touch.data.y);
        }
        else if (questDisplayed)
        {
          checkQuestLocation(touch.data.x, touch.data.y);
        }
        else
        {
          checkTownLocation(touch.data.x, touch.data.y);
        }
      }
      else if (screen == SETTINGS || screen == TIMEMENU)
      {
        checkMenuPress(touch.data.x, touch.data.y, &timekeeper, &blValue); // If returns one we clicked on the set time screen
      
      }
      else if (screen == STATUSSCREEN || screen == STATUS2)
      {
        checkingStats = !checkingStats;
        if(checkingStats==false){
          screen = STATUSSCREEN;
        }
      }
      else if (screen == DUNGEON)
      {
        checkDungeonClick(touch.data.x, touch.data.y, &D);
      }
    }
  
  }
}


void setup() {
  bool status;
  pinMode(button1, INPUT_PULLUP); // config GPIO21 as input pin and enable the internal pull-up resistor
  pinMode(button2, INPUT_PULLUP);

  Serial.begin(115200);
  randomSeed(analogRead(0));
  // Set up Accelerometer.
  // Program will hang on start if accelermoter not detected.
  I2CMPU.begin(I2C_SDA, I2C_SCL, 100000); 
  status =  mpu.begin(0x68, &I2CMPU);
  // Try to initialize!
    if(!status){
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  // mpu6050 setup
  
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
//  Serial.print("Accelerometer range set to: ");
//  switch (mpu.getAccelerometerRange()) {
//  case MPU6050_RANGE_2_G:
//    Serial.println("+-2G");
//    break;
//  case MPU6050_RANGE_4_G:
//    Serial.println("+-4G");
//    break;
//  case MPU6050_RANGE_8_G:
//    Serial.println("+-8G");
//    break;
//  case MPU6050_RANGE_16_G:
//    Serial.println("+-16G");
//    break;
//  }

  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
//  Serial.print("Filter bandwidth set to: ");
//  switch (mpu.getFilterBandwidth()) {
//  case MPU6050_BAND_260_HZ:
//    Serial.println("260 Hz");
//    break;
//  case MPU6050_BAND_184_HZ:
//    Serial.println("184 Hz");
//    break;
//  case MPU6050_BAND_94_HZ:
//    Serial.println("94 Hz");
//    break;
//  case MPU6050_BAND_44_HZ:
//    Serial.println("44 Hz");
//    break;
//  case MPU6050_BAND_21_HZ:
//    Serial.println("21 Hz");
//    break;
//  case MPU6050_BAND_10_HZ:
//    Serial.println("10 Hz");
//    break;
//  case MPU6050_BAND_5_HZ:
//    Serial.println("5 Hz");
//    break;
//  }
  
  // Screen Setup
  tft.init();
  popup.createSprite(160, 120);
  popupText.createSprite(160, 60);

  // create the base for a yes, no popup
  popup.fillScreen(TFT_WHITE);
  popup.fillRoundRect(5,65,150,20,1,TFT_GREEN);
  popup.fillRoundRect(5,90,150,20,1,TFT_RED); // need to add yes and no to the buttons
  popup.setTextColor(TFT_BLACK);
  popupText.fillScreen(TFT_WHITE);
  popupText.setTextColor(TFT_BLACK);
  popupText.setTextWrap(true);
  popup.setCursor(65, 75);
  popup.print("YES");
  popup.setCursor(65,100);
  popup.print("NO");
  
  touch.begin();

  Serial.print(touch.data.version);
  Serial.print("\t");
  Serial.print(touch.data.versionInfo[0]);
  Serial.print("-");
  Serial.print(touch.data.versionInfo[1]);
  Serial.print("-");
  Serial.println(touch.data.versionInfo[2]);
    
  p = setupPlayer();
  setupTown(p.level, p.location);
  p.currStatus = INTOWN;
  
  // Initialize timekeeping struct
  timekeeper._hours = 1;
  timekeeper._minutes = 0;
  timekeeper._seconds = 0;
  timekeeper.previousMillisTime = 0;
  //wifi status
  timekeeper.connection = false;
  timekeeper._days= " ";
  timekeeper.idleTime = 30;
  
  // Time Setting Screen
  timekeeper.settingTime = false;

    setupDungeon(p.level, p.location, &D);
  // Multithreading setup
  xTaskCreatePinnedToCore(attachStepTimerInterruptTask, "attachStepTimerInterruptTask", 4096, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(loop2, "loop2", 8000, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(loop1, "loop1", 8000, NULL, 1, NULL, 1);
//  xTaskCreatePinnedToCore(loop, "loop", 4096, NULL, 2, NULL, 1);
  blValue = 100; // backlight level
  
}
