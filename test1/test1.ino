
/*
 * Testing the ESP32-S3 with the Accelerometer and the Touch Screen component. 
 * 
 */
#include <TFT_eSPI.h>       // Include the graphics library
#include <CST816S.h>

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

#define I2C_SDA 35
#define I2C_SCL 36

Adafruit_MPU6050 mpu;

TwoWire I2CMPU = TwoWire(1);

TFT_eSPI tft = TFT_eSPI();  // Create object "tft"
CST816S touch(5, 6, 9, 3);  // sda, scl, rst, irq
uint16_t colors[12];

const int button1 = 4;
const int button2 = 7;
 int buttonState1 =0;
 int buttonState2=0;

void setup() {
  bool status;
pinMode(4, INPUT_PULLUP); // config GPIO21 as input pin and enable the internal pull-up resistor
;
 pinMode(button2, INPUT_PULLUP);

  Serial.begin(115200);
 /* I2CMPU.begin(I2C_SDA, I2C_SCL, 100000); 
  status =  mpu.begin(0x68, &I2CMPU);
  // Try to initialize!
    if(!status){
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }*/
  Serial.println("MPU6050 Found!");
  tft.init();
  tft.fillScreen(TFT_BLACK);
  
  touch.begin();

  Serial.print(touch.data.version);
  Serial.print("\t");
  Serial.print(touch.data.versionInfo[0]);
  Serial.print("-");
  Serial.print(touch.data.versionInfo[1]);
  Serial.print("-");
  Serial.println(touch.data.versionInfo[2]);
  

}


void loop() {
     buttonState1 = digitalRead(button1);
   buttonState2 = digitalRead(button2);
    
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

     /* Get new sensor events with the readings */
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  
  // Set "cursor" at top left corner of display (0,0) and select font 4
  tft.setCursor(65, 10, 4);

  // Set the font colour to be white with a black background
  tft.setTextColor(TFT_WHITE, TFT_BLACK);

  tft.setCursor(65, 40, 4);
  // We can now plot text on screen using the "print" class
  tft.println("BUTTS");
  
  tft.setCursor(65, 60, 4);
  
  tft.setTextColor(TFT_RED, TFT_BLACK);
  
  tft.println("Accel X: ");
  //tft.setCursor(65, 80, 4);
  //tft.println(a.acceleration.x);
 
  
  tft.setTextColor(TFT_GREEN, TFT_BLACK);

  tft.setCursor(65, 100, 4);
  //tft.println("Accel Y:");
    if(buttonState1 == LOW){
    tft.println("Button 1");  
    Serial.println("Button1");
  }
  else{
    tft.println("________");
    Serial.println("  ");  
  }
  
  tft.setCursor(65, 120, 4);
  //tft.println(a.acceleration.y);
  
  tft.setTextColor(TFT_BLUE, TFT_BLACK);
  tft.setCursor(65, 140, 4);
      if(buttonState2 == LOW){
    tft.println("Button 2");  
    Serial.println("Button2");
  }
  else{
    tft.println("________");  
    Serial.println("   ");
  }
  
  //tft.println("Accel Z:");
  //tft.setCursor(65, 160, 4);
  //tft.println(a.acceleration.z);
 


}
