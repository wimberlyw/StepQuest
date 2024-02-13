
/*
 * Testing the ESP32-S3 with the Accelerometer and the Touch Screen component. 
 * Attempt to fix step algorithm to be more compatible with pocket walking.
 * Also an attempt to generally clean up the code.
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

void setup() {
  bool status;
 
  Serial.begin(115200);
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

// default threshold (subject to change)
float threshold = 1.0;
int num_steps = 0;

// setting time between each accelerometer reading, shortest valid time between steps, longest valid time between steps
float delay_time_seconds = 0.02;
float low_bound_seconds = 0.24; 
float high_bound_seconds = 0.96; 

// convert time in seconds to milliseconds and cycles
float delay_time = delay_time_seconds * 1000;
float low_bound_cycles = low_bound_seconds / delay_time_seconds;
float high_bound_cycles = high_bound_seconds / delay_time_seconds;

// enter the drift of the accelerometer (unique to each MPU6050)
float x_drift = 0.40;
float y_drift = 0.45;
float z_drift = 10;

// offsets the gravity and x,y,z drift in order to get an amplitude close to 0 regardless of orientation
float correction = sqrt(pow((x_drift),2)+pow((y_drift),2)+pow((z_drift),2));

// num cycles since last step
int last_step = 0;

// steps to be added if user moves into walking state
int potential_steps = 0;

// steps threshold to leave back into moving state
int moving_threshold = 20;

// 0 = still, 1 = moving
int state = 0;

// used to compare adjacent peak values (only over the threshold)
// the last peak (next value went down comparatively) is counted as a step
// will also count as the threshold itself (resets to threshold upon a step counted)
float last_peak = threshold;

void loop() {

  /* Get new sensor events with the readings */
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  float x = a.acceleration.x;
  float y = a.acceleration.y;
  float z = a.acceleration.z;

  // calculate the amplitude
  float amp = sqrt(pow((x),2)+pow((y),2)+pow((z),2)) - correction;

  // check if this value goes up or down compared to previous peak value
  if (amp >= last_peak)
  {
    last_peak = amp;
  }
  else if (last_peak != threshold)// potential step found
  {
    if (last_step > low_bound_cycles) // if fails, likely not a real step
    {
      if (last_step < high_bound_cycles) // continuity maintained
      {
        if (state == 0) // in still state
        {
          potential_steps++;
          if (potential_steps >= 10) // move to moving state
          {
            state = 1;
            num_steps += potential_steps;
          }
        }
        else // in moving state
        {
          num_steps++;
        }
        last_step = 0;
      }
      else // move to still state
      {
        state = 0;
        potential_steps = 1;
        last_step = 0;
      }
    }
    last_peak = threshold; // reset to threshold
  }

  last_step++;
  
  // Set "cursor" at top left corner of display (0,0) and select font 4
  tft.setCursor(65, 10, 4);

  // Set the font colour to be white with a black background
  tft.setTextColor(TFT_WHITE, TFT_BLACK);

  tft.setCursor(65, 40, 4);
  // We can now plot text on screen using the "print" class
  tft.println("Steps");
  
  tft.setCursor(65, 60, 4);
  
  tft.setTextColor(TFT_RED, TFT_BLACK);
  tft.println(num_steps);

  delay(delay_time);


}
