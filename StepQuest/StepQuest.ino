#include <TFT_eSPI.h>       // Include the graphics library
#include <CST816S.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include "ESP32TimerInterrupt.h"
#include <Wire.h>
#include "Char1.h"
#include "steps.h"

#define I2C_SDA 35
#define I2C_SCL 36
#define SCREENDEBOUNCE 500
#define BUTTONDEBOUNCE 200
#define ANIMINTERVAL 200

Adafruit_MPU6050 mpu;

// Fixed by switching what task is running on which core, now step alg and interrupt run on core 0 instead of the graphics
//E (13501) task_wdt: Task watchdog got triggered. The following tasks did not reset the watchdog in time:
//E (13501) task_wdt:  - IDLE (CPU 0)
//E (13501) task_wdt: Tasks currently running:
//E (13501) task_wdt: CPU 0: loop1
//E (13501) task_wdt: CPU 1: loopTask
//E (13501) task_wdt: Aborting.


TwoWire I2CMPU = TwoWire(1);

// Graphics
TFT_eSPI tft = TFT_eSPI();  // Create object "tft"
TFT_eSprite Char = TFT_eSprite(&tft);
TFT_eSprite background = TFT_eSprite(&tft);
CST816S touch(5, 6, 9, 3);  // sda, scl, rst, irq

uint16_t colors[12];


// Timers
unsigned int _hours = 1;
unsigned int _minutes, _seconds = 0;
unsigned long previousMillisScreen =0;
unsigned long previousMillisButton = 0;
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

unsigned long previousButtonMillis;

// Steps
unsigned int steps = 0;
ESP32Timer StepTimer(0);
//hw_timer_t *StepTimer = NULL;
volatile int stepFlag = 0;
sensors_event_t a, g, temp;

bool IRAM_ATTR StepTimerHandler(void * timerNo)
{
  stepFlag = 1;

  return true;
}

void attachStepTimerInterruptTask(void *pvParameters)
{
  if (StepTimer.attachInterruptInterval(20000, StepTimerHandler))
  {
    Serial.println("StepTimer started");
  }

  vTaskDelete(NULL);
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

  /* Timer setup for step algorithm
  StepTimer = timerBegin(0,80,true);
  timerAttachInterrupt(StepTimer, &StepTimer_ISR, true);
  timerAlarmWrite(StepTimer, 20000, true);
  timerAlarmEnable(StepTimer);*/
  
  // Screen Setup

  tft.init();
  //tft.fillScreen(TFT_BLACK);
  //tft.setSwapBytes(true);
  //tft.setRotation(1);
  Char.createSprite(96,96);
  background.createSprite(240,240);
  Char.setSwapBytes(true);
  touch.begin();

  Serial.print(touch.data.version);
  Serial.print("\t");
  Serial.print(touch.data.versionInfo[0]);
  Serial.print("-");
  Serial.print(touch.data.versionInfo[1]);
  Serial.print("-");
  Serial.println(touch.data.versionInfo[2]);
    
  background.setTextColor(TFT_WHITE, TFT_SKYBLUE);

  // Multithreading setup
  xTaskCreatePinnedToCore(attachStepTimerInterruptTask, "attachStepTimerInterruptTask", 4096, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(loop2, "loop2", 4096, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(loop1, "loop1", 4096, NULL, 1, NULL, 1);
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
  
     // Take inputs
     readScreenGesture();
     readButtons();
     
    // limits for the screen variable
    if(screen>3)
    {
      screen = 0;   
    }
    if(screen<0){
      screen = 3;
    } 
      
     background.setCursor(65, 40, 4);
    // We can now plot text on screen using the "print" class
    // background.println(gest);

    background.setTextColor(TFT_GREEN, TFT_BLACK);
    background.fillSprite(TFT_SKYBLUE);
//    Serial.println(screen);
  
    //// *********************** Screen Handling **********************************//
    // Show the appropriate screen based on the button
    switch(screen){
      case 0:
    {    
      //rial.println("in switch");
    
    // Set the font colour to be white with a black background
    background.setCursor(65, 60, 4);
    background.setTextColor(TFT_RED, TFT_BLACK);
    
    background.println("Screen 0: ");
  
       // Animate
       if (millis() - prevAnim >= ANIMINTERVAL){  //every 300ms
        //re-initialize the timing
        prevAnim = millis();
       charFrame++;
       }
       if(charFrame >2){ 
        charFrame = 0;
       }
       Char.pushImage(0,0,96,96, C1[charFrame] ); // push to the created image at 0, 0, size of 32 x 32, the C1 array from char0.h, charFrame index.
       Char.pushToSprite(&background, 70, 115, TFT_BLACK);
    
    } // End Case 0
      break;
    case 1:
    {
      background.fillScreen(TFT_BLACK);
      background.setCursor(65, 60, 4);
      background.setTextColor(TFT_RED, TFT_BLACK);
      background.print("Steps: ");
      background.print(steps);
      
      break;
    } // End Case 1
    case 2:
    {
      background.fillScreen(TFT_BLACK);
      background.setCursor(65, 60, 4);
      background.setTextColor(TFT_GREEN, TFT_BLACK);
      background.println("SCREEN 2");
      
      break;
    } // End Case 2
    case 3:
    {
      background.fillScreen(TFT_BLACK);
      background.setCursor(65, 60, 4);
      background.setTextColor(TFT_YELLOW, TFT_BLACK);
      background.println("SCREEN 3");
      
      break;  
    } // End Case 3
  } // end switch(screen)
  
    // Push Background to screen
    background.pushSprite(0,0);
   } 
} // End loop()

void loop2(void *pvParameters) // worst case could just use delay 20 ms in this loop instead of trying to use interrupt but that means nothing else can run on this core ever
{
  while(1)
  {
     // if raised check for steps
    if (stepFlag == 1)
    {
      mpu.getEvent(&a, &g, &temp);
      steps = stepAlg(a);
      stepFlag = 0;
    }
  }
}

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
    background.println(gest);

    // Check if the gesture falls outside of debounce time
    if(gest == "SWIPE LEFT" && (millis() - previousMillisScreen >= SCREENDEBOUNCE)){ 
      //re-initialize the timing
      previousMillisScreen = millis();
      // advance the screen
      screen++;
    }
    if(gest == "SWIPE RIGHT" && (millis() - previousMillisScreen >= SCREENDEBOUNCE)){
    screen--;
    }
  }
}