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
#include "settingsMenu.h"
//#include "wifitime.h"
#include "quests.h"
#include "towns.h"
#include "timekeeping.h"

#define I2C_SDA 35
#define I2C_SCL 36
#define SCREENDEBOUNCE 500
#define BUTTONDEBOUNCE 200
#define ANIMINTERVAL 200
#define STEP_TIMER_INTERVAL_MS 20

Adafruit_MPU6050 mpu;

/*TODO: Use .setTextSize(2) and reorganize things to make it a bit more legible*/


TwoWire I2CMPU = TwoWire(1);

// Graphics
TFT_eSPI tft = TFT_eSPI();  // Create object "tft"
TFT_eSprite Char = TFT_eSprite(&tft);
TFT_eSprite background = TFT_eSprite(&tft);
//TFT_eSprite worldmap = TFT_eSprite(&tft);
// TFT_eSprite foregroud = TFT_eSprite(&tft);
TFT_eSprite popup = TFT_eSprite(&tft);
TFT_eSprite popupText = TFT_eSprite(&tft);
//TFT_eSprite Dungeon = TFT_eSprite(&tft);
//TFT_eSprite Forest = TFT_eSprite(&tft);
//TFT_eSprite Castle = TFT_eSprite(&tft);
//TFT_eSprite townMenu = TFT_eSprite(&tft);
CST816S touch(5, 6, 9, 3);  // sda, scl, rst, irq

// Timers

unsigned long previousMillisScreen =0;
unsigned long previousMillisButton = 0;

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
String gest = "";

int buttonState1 =0;
int buttonState2=0;

enum states {
  TOWN = 0,
  TRAVEL,
  DUNGEON,
};

enum screen {
  HOMESCREEN = 0,
  STATUSSCREEN,
  WORLDMAP,
   SETTINGS,
   TOWNMENU,
};



// Steps
volatile unsigned int steps = 0;
ESP32Timer StepTimer(0);
//hw_timer_t *StepTimer = NULL;
volatile int stepFlag = 0;
sensors_event_t a, g, temp;
boolean travelling = false;
volatile int travelSteps = 0;
volatile int tempSteps = 0;
volatile int totalTravelSteps = 0;
int player_location = 0; // start at town 1
//volatile int fractionTravelStep = 0;
//volatile int stepsToChangePos = 0;
//int travelPath = 0; // which path (0-3)
//int travelPoint = 0; // which point on that path (0-7)

// Shop and Quest items
Town t;
boolean shopDisplayed = false;
boolean questDisplayed = false;
int quest_selected = 0;
int player_level = 50;

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
    
     // Take inputs
     readScreenGesture();
     readButtons();

     if (travelling && travelSteps == 0)
    {
      finishTravel();
      // setup the location
      if (player_location == 0 || player_location == 1 || player_location == 3)
      {
        t = setupTown(player_level,player_location); // player level is currently hardcoded because it doesn't exist yet!
      }
      else
      {
        //setupDungeon();
      }
    }
//    else if (travelling)
//    {
//      // check if position on map has moved
//      if (steps > fractionTravelSteps)
//      {
//        if (travelDirection()) // if moving forward
//        {
//          
//        }
//        else // moving backwards
//        {
//          
//        }
//        fractionTravelSteps += travelSteps
//      }
//    }
     
    // limits for the screen variable
    
    if (travelling)
    {
      if (screen > SETTINGS) screen = HOMESCREEN;
      if (screen < HOMESCREEN) screen = SETTINGS;
    }
    else
    {
        if(screen>TOWNMENU)
        {
            screen = HOMESCREEN;   
        }
        if(screen<HOMESCREEN){
            screen = TOWNMENU;
        }
    } 

    //background.fillSprite(TFT_SKYBLUE);
  
    //// *********************** Screen Handling **********************************/
    // Show the appropriate screen based on the button
    switch(screen)
    {
      case HOMESCREEN:
      {   
        background.setColorDepth(8);
        background.createSprite(240,240);
        background.setSwapBytes(true);
        background.fillScreen(TFT_BLACK);
        background.setCursor(65, 60, 4);
        background.setTextColor(TFT_RED, TFT_BLACK);
        background.print("Steps: ");
        background.print(steps);
        background.setCursor(62, 100, 4);
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
           background.setCursor(62, 130, 4);
           background.println(timekeeper._days);
        }
        break;
        
      } // End Case 0
      case STATUSSCREEN:
      {
        background.setColorDepth(8);
        background.createSprite(240,240);
        background.setSwapBytes(true);
        Char.setColorDepth(8);
        Char.createSprite(96,96);
        Char.setSwapBytes(true);
        // Set the font colour to be white with a black background
        background.setCursor(65, 60, 4);
        background.setTextColor(TFT_RED, TFT_BLACK);
        
        background.println("Status: ");
      
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
        Char.pushToSprite(&background, 70, 115, TFT_BLACK);
        break;
      } // End Case 1
      case WORLDMAP: // map screen
      {
        background.setColorDepth(8);
        background.createSprite(240,240);
        background.setSwapBytes(true);
        background.pushImage(0,0, 240, 240, map1);
        // background.pushToSprite(&background, 0, 0);
        break;
      } // End Case 2
      case SETTINGS:
      {
        background.setColorDepth(8);
        background.createSprite(240,240);
        background.setSwapBytes(true);
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
        background.setCursor(86, 140, 4);
        background.println("Time");
        // fillRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h, int16_t radius, uint16_t color) 
        background.fillRoundRect(50, 88, 8, 8, 2, TFT_YELLOW);
        break;  
      } // End Case 3
      case TOWNMENU:
      {
        //background.setColorDepth(8);
        background.createSprite(240,240);
        background.setSwapBytes(true);
        if (player_location == 0 || player_location == 1 || player_location == 3) // we are in a town
        {
          background.pushImage(0,0, 240, 240, castlecropped);
          if (shopDisplayed)
          {
            
            
          }
          else if (questDisplayed)
          {
            background.fillRoundRect(40,20,160,20,1,TFT_BLUE);
            background.setTextColor(TFT_WHITE);
            //background.setTextSize(1);
            background.setCursor(90,20);
            background.print("Quest Board");
            background.fillRoundRect(40,200,160,20,1,TFT_RED);
            background.setCursor(110,210);
            background.print("Exit");
            background.setCursor(63,30);
            background.print(t.quests_per_12hr);
            background.print(" quests remaining.");
  
            switch(quest_selected)
            {
              case(0):
              {
                background.fillRoundRect(60,50,120,40,1,TFT_WHITE);
                background.fillRoundRect(60,100,120,40,1,TFT_WHITE);
                background.fillRoundRect(60,150,120,40,1,TFT_WHITE);
                break;
              }
              case(1):
              {
                background.fillRoundRect(60,50,120,40,1,TFT_CYAN);
                background.drawRoundRect(60,50,120,40,1,TFT_BLACK);
                background.fillRoundRect(184,50,50,40,1,TFT_GREEN);
                background.fillRoundRect(20,50,36,40,1,TFT_RED);
                background.fillRoundRect(60,100,120,40,1,TFT_WHITE);
                background.fillRoundRect(60,150,120,40,1,TFT_WHITE);
                background.setCursor(22,65);
                background.print("Trash");
                background.setCursor(186,55);
                background.setTextColor(TFT_BLACK);
                background.print("Begin");
                background.setCursor(186,80);
                background.print(t.curQuests[0].progress);
                background.print("/");
                background.print(t.curQuests[0].requirement);
                break;
              }
              case(2):
              {
                background.fillRoundRect(60,100,120,40,1,TFT_CYAN);
                background.drawRoundRect(60,100,120,40,1,TFT_BLACK);
                background.fillRoundRect(184,100,50,40,1,TFT_GREEN);
                background.fillRoundRect(20,100,36,40,1,TFT_RED);
                background.fillRoundRect(60,50,120,40,1,TFT_WHITE);
                background.fillRoundRect(60,150,120,40,1,TFT_WHITE);
                background.setCursor(22,115);
                background.print("Trash");
                background.setCursor(186,105);
                background.setTextColor(TFT_BLACK);
                background.print("Begin");
                background.setCursor(186,130);
                background.print(t.curQuests[1].progress);
                background.print("/");
                background.print(t.curQuests[1].requirement);
                break;
              }
              case(3):
              {
                background.fillRoundRect(60,150,120,40,1,TFT_CYAN);
                background.drawRoundRect(60,150,120,40,1,TFT_BLACK);
                background.fillRoundRect(184,150,50,40,1,TFT_GREEN);
                background.fillRoundRect(20,150,36,40,1,TFT_RED);
                background.fillRoundRect(60,50,120,40,1,TFT_WHITE);
                background.fillRoundRect(60,100,120,40,1,TFT_WHITE);
                background.setCursor(22,165);
                background.print("Trash");
                background.setCursor(186,155);
                background.setTextColor(TFT_BLACK);
                background.print("Begin");
                background.setCursor(186,180);
                background.print(t.curQuests[2].progress);
                background.print("/");
                background.print(t.curQuests[2].requirement);
                break;
              }
            } // end switch
            // Display the actual quests which are available to the player
            displayQuests(t.curQuests);
          }
          else
          {
            // create Town menu
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
          //worldmap.pushToSprite(&background, 0, 0);
        }
        else // we are in dungeon
        {
          background.pushImage(0,0, 240, 240, dungeoncroppped);
          //background.pushToSprite(&background, 0, 0);
        }
        
      } // End Case 4
     
    } // end switch(screen)
  
    // Push Background to screen
    background.pushSprite(0,0);
    background.deleteSprite();
    Char.deleteSprite();
  } 
} // End loop1()

void loop2(void *pvParameters)
{
  while(1)
  {
    // if raised check for steps
    if (stepFlag == 1)
    {
      mpu.getEvent(&a, &g, &temp);
      tempSteps = stepAlg(a);
      steps += tempSteps;

      if (travelSteps > 0)
      {
        travelSteps -= tempSteps;
        totalTravelSteps += tempSteps;
      }
      stepFlag = 0;
    }

     // TimeKeeping
    if(timekeeper.connection ==false)
    {
      
      processOfflineTimekeeping(timekeeperPtr); 
    }
    else{
      updateTimeWifi(timekeeperPtr); 
    }
  }

 
} // End loop2()

void loop(){}


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
    screen++;  
    Serial.println("Button1");
  }
  if(buttonState2 == LOW ){
    screen--;  
    Serial.println("Button2");
  }
  } 
}

void readScreenGesture(){
  // If the screen is in the middle of being touched
  if(touch.available()){ 
    //Get the gesture
    gest = touch.gesture();
    // Print to the screen for debug
    Serial.println(gest);
    //background.println(gest); // I don't think this does anything

    // Check if the gesture falls outside of debounce time
    if (millis() - previousMillisScreen < SCREENDEBOUNCE){
      return;
    }
    
    if(gest == "SWIPE LEFT"){ 
      //re-initialize the timing
      previousMillisScreen = millis();
      // advance the screen
      screen++;
    }
    if(gest == "SWIPE RIGHT"){
      previousMillisScreen = millis();
      screen--;
    }
    if (gest == "SINGLE CLICK"){
      previousMillisScreen = millis();
//      Serial.print(touch.data.x);
//      Serial.print(" ");
//      Serial.println(touch.data.y);
      if (screen == WORLDMAP) // map  screen
      {
        checkMapLocation(touch.data.x, touch.data.y);
      }
      if (screen == TOWNMENU) // quest testing rn
      {
        if (shopDisplayed)
        {
          
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
      if (screen == SETTINGS)
      {
        checkMenuPress(touch.data.x, touch.data.y, &timekeeper);
      }
    }
  }
}

void setup() {
  bool status;
  pinMode(button1, INPUT_PULLUP); // config GPIO21 as input pin and enable the internal pull-up resistor
  pinMode(button2, INPUT_PULLUP);

  Serial.begin(115200);
  
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
  Serial.print("Accelerometer range set to: ");
  switch (mpu.getAccelerometerRange()) {
  case MPU6050_RANGE_2_G:
    Serial.println("+-2G");
    break;
  case MPU6050_RANGE_4_G:
    Serial.println("+-4G");
    break;
  case MPU6050_RANGE_8_G:
    Serial.println("+-8G");
    break;
  case MPU6050_RANGE_16_G:
    Serial.println("+-16G");
    break;
  }

  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  Serial.print("Filter bandwidth set to: ");
  switch (mpu.getFilterBandwidth()) {
  case MPU6050_BAND_260_HZ:
    Serial.println("260 Hz");
    break;
  case MPU6050_BAND_184_HZ:
    Serial.println("184 Hz");
    break;
  case MPU6050_BAND_94_HZ:
    Serial.println("94 Hz");
    break;
  case MPU6050_BAND_44_HZ:
    Serial.println("44 Hz");
    break;
  case MPU6050_BAND_21_HZ:
    Serial.println("21 Hz");
    break;
  case MPU6050_BAND_10_HZ:
    Serial.println("10 Hz");
    break;
  case MPU6050_BAND_5_HZ:
    Serial.println("5 Hz");
    break;
  }
  
  // Screen Setup
  tft.init();
  //tft.fillScreen(TFT_BLACK);
  //tft.setSwapBytes(true);
  //tft.setRotation(1);
  popup.createSprite(160, 120);
  popupText.createSprite(160, 60);
  /*Char.createSprite(96,96);
  background.createSprite(240,240);
  worldmap.createSprite(240,240);
  
  townMenu.createSprite(240,240);
  Char.setSwapBytes(true);
  worldmap.setSwapBytes(true);
  
  townMenu.setSwapBytes(true);
*/

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
    
  t = setupTown(player_level,0);

  // Initialize timekeeping struct
  timekeeper._hours = 1;
  timekeeper._minutes = 0;
  timekeeper._seconds = 0;
  timekeeper.previousMillisTime = 0;
  //wifi status
  timekeeper.connection = false;
  timekeeper._days= " ";

 // connectToWifi(&connection);
  // Multithreading setup
  xTaskCreatePinnedToCore(attachStepTimerInterruptTask, "attachStepTimerInterruptTask", 4096, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(loop2, "loop2", 4096, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(loop1, "loop1", 4096, NULL, 1, NULL, 1);

  
}
