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
#include "quests.h"
#include "towns.h"

#define I2C_SDA 35
#define I2C_SCL 36
#define SCREENDEBOUNCE 500
#define BUTTONDEBOUNCE 200
#define ANIMINTERVAL 200
#define SECONDINTERVAL 1000
#define STEP_TIMER_INTERVAL_MS 20

Adafruit_MPU6050 mpu;

/*TODO: Use .setTextSize(2) and reorganize things to make it a bit more legible*/


TwoWire I2CMPU = TwoWire(1);

// Graphics
TFT_eSPI tft = TFT_eSPI();  // Create object "tft"
TFT_eSprite Char = TFT_eSprite(&tft);
TFT_eSprite background = TFT_eSprite(&tft);
TFT_eSprite image = TFT_eSprite(&tft);
TFT_eSprite Map = TFT_eSprite(&tft);
TFT_eSprite popup = TFT_eSprite(&tft);
TFT_eSprite popupText = TFT_eSprite(&tft);
TFT_eSprite Dungeon = TFT_eSprite(&tft);
TFT_eSprite Forest = TFT_eSprite(&tft);
TFT_eSprite Castle = TFT_eSprite(&tft);
TFT_eSprite Town_menu = TFT_eSprite(&tft);
CST816S touch(5, 6, 9, 3);  // sda, scl, rst, irq

uint16_t colors[12];


// Timers
unsigned int _hours = 1;
unsigned int _minutes, _seconds = 0;
unsigned long previousMillisScreen =0;
unsigned long previousMillisButton = 0;
unsigned long previousMillisTime = 0;
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

unsigned long previousButtonMillis;

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
     * // Touch Screen data
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
    // Doesn't provide 24 hour time atm?
    // TimeKeeping
      if ((unsigned long)(millis() - previousMillisTime) >= SECONDINTERVAL) 
      {
        _seconds++;
        //idletime--;
        previousMillisTime = millis();
      }
       //time keeping
      if (_seconds > 59)
      {
        _minutes++;
        _seconds = 0;
      }
      if (_minutes > 59)
      {
        _hours++;
        _minutes = 0;
      }
      if (_hours > 12)
      {
        _hours = 1;
      }
  
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
      if (screen > 2) screen = 0;
      if (screen < 0) screen = 2;
    }
    else
    {
      if (screen > 3) screen = 0;
      if (screen < 0) screen = 3;
    }

    background.fillSprite(TFT_SKYBLUE);
  
    //// *********************** Screen Handling **********************************//
    // Show the appropriate screen based on the button
    switch(screen)
    {
      case 0:
      {   
        background.fillScreen(TFT_BLACK);
        background.setCursor(65, 60, 4);
        background.setTextColor(TFT_RED, TFT_BLACK);
        background.print("Steps: ");
        background.print(steps);
        break;
        
      } // End Case 0
      case 1:
      {

        // Set the font colour to be white with a black background
        background.setCursor(65, 60, 4);
        background.setTextColor(TFT_RED, TFT_BLACK);
        
        background.println("Screen 0: ");
      
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
        Char.pushImage(0,0,96,96, C1[charFrame] ); // push to the created image at 0, 0, size of 32 x 32, the C1 array from char0.h, charFrame index.
        Char.pushToSprite(&background, 70, 115, TFT_BLACK);
        
        break;
      } // End Case 1
      case 2: // map screen
      {
        image.pushImage(0,0, 240, 240, map1);
        image.pushToSprite(&background, 0, 0);
        
        break;
      } // End Case 2
      case 3: // Town/Dungeon Screen
      {
        if (player_location == 0 || player_location == 1 || player_location == 3) // we are in a town
        {
          image.pushImage(0,0, 240, 240, castlecropped);
          if (shopDisplayed)
          {
            
            
          }
          else if (questDisplayed)
          {
            image.fillRoundRect(40,20,160,20,1,TFT_BLUE);
            image.setTextColor(TFT_WHITE);
            //image.setTextSize(2);
            image.setCursor(90,20);
            image.print("Quest Board");
            image.fillRoundRect(40,200,160,20,1,TFT_RED);
            image.setCursor(110,210);
            image.print("Exit");
            image.setCursor(63,30);
            image.print(t.quests_per_12hr);
            image.print(" quests remaining.");
  
            switch(quest_selected)
            {
              case(0):
              {
                image.fillRoundRect(60,50,120,40,1,TFT_WHITE);
                image.fillRoundRect(60,100,120,40,1,TFT_WHITE);
                image.fillRoundRect(60,150,120,40,1,TFT_WHITE);
                break;
              }
              case(1):
              {
                image.fillRoundRect(60,50,120,40,1,TFT_CYAN);
                image.drawRoundRect(60,50,120,40,1,TFT_BLACK);
                image.fillRoundRect(184,50,50,40,1,TFT_GREEN);
                image.fillRoundRect(20,50,36,40,1,TFT_RED);
                image.fillRoundRect(60,100,120,40,1,TFT_WHITE);
                image.fillRoundRect(60,150,120,40,1,TFT_WHITE);
                image.setCursor(22,65);
                image.print("Trash");
                image.setCursor(186,55);
                image.setTextColor(TFT_BLACK);
                image.print("Begin");
                image.setCursor(186,80);
                image.print(t.curQuests[0].progress);
                image.print("/");
                image.print(t.curQuests[0].requirement);
                break;
              }
              case(2):
              {
                image.fillRoundRect(60,100,120,40,1,TFT_CYAN);
                image.drawRoundRect(60,100,120,40,1,TFT_BLACK);
                image.fillRoundRect(184,100,50,40,1,TFT_GREEN);
                image.fillRoundRect(20,100,36,40,1,TFT_RED);
                image.fillRoundRect(60,50,120,40,1,TFT_WHITE);
                image.fillRoundRect(60,150,120,40,1,TFT_WHITE);
                image.setCursor(22,115);
                image.print("Trash");
                image.setCursor(186,105);
                image.setTextColor(TFT_BLACK);
                image.print("Begin");
                image.setCursor(186,130);
                image.print(t.curQuests[1].progress);
                image.print("/");
                image.print(t.curQuests[1].requirement);
                break;
              }
              case(3):
              {
                image.fillRoundRect(60,150,120,40,1,TFT_CYAN);
                image.drawRoundRect(60,150,120,40,1,TFT_BLACK);
                image.fillRoundRect(184,150,50,40,1,TFT_GREEN);
                image.fillRoundRect(20,150,36,40,1,TFT_RED);
                image.fillRoundRect(60,50,120,40,1,TFT_WHITE);
                image.fillRoundRect(60,100,120,40,1,TFT_WHITE);
                image.setCursor(22,165);
                image.print("Trash");
                image.setCursor(186,155);
                image.setTextColor(TFT_BLACK);
                image.print("Begin");
                image.setCursor(186,180);
                image.print(t.curQuests[2].progress);
                image.print("/");
                image.print(t.curQuests[2].requirement);
                break;
              }
            } // end switch
            // Display the actual quests which are available to the player
            displayQuests(t.curQuests);
          }
          else
          {
            // create Town menu
            image.fillRoundRect(40,20,160,20,1,TFT_BLUE);
            image.fillRoundRect(60,60,120,40,1,TFT_WHITE);
            image.fillRoundRect(60,120,120,40,1,TFT_WHITE);
            image.setTextColor(TFT_WHITE);
            //image.setTextSize(2);
            image.setCursor(110,20);
            image.print("Town");
            image.setTextColor(TFT_BLACK);
            image.setCursor(90,80);
            image.print("Quest Board");
            image.setCursor(110,140);
            image.print("Shop");
          }
          image.pushToSprite(&background, 0, 0);
        }
        else // we are in dungeon
        {
          image.pushImage(0,0, 240, 240, dungeoncroppped);
          image.pushToSprite(&background, 0, 0);
        }
        
        break;  
      } // End Case 3
    } // end switch(screen)
  
    // Push Background to screen
    background.pushSprite(0,0);
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
      if (screen == 2) // map  screen
      {
        checkMapLocation(touch.data.x, touch.data.y);
      }
      if (screen == 3) // quest testing rn
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
  Char.createSprite(96,96);
  background.createSprite(240,240);
  image.createSprite(240,240);
  popup.createSprite(160, 120);
  popupText.createSprite(160, 60);
  Char.setSwapBytes(true);
  image.setSwapBytes(true);
  

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
    
  background.setTextColor(TFT_WHITE, TFT_SKYBLUE);
  t = setupTown(player_level,0);

  // Multithreading setup
  xTaskCreatePinnedToCore(attachStepTimerInterruptTask, "attachStepTimerInterruptTask", 4096, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(loop2, "loop2", 4096, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(loop1, "loop1", 4096, NULL, 1, NULL, 1);
}
