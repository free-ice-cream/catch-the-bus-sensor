#include <Adafruit_NeoPixel.h>

#include <ComponentObject.h>
#include <RangeSensor.h>
#include <SparkFun_VL53L1X.h>
#include <vl53l1x_class.h>
#include <vl53l1_error_codes.h>
#include <Wire.h>
#include "SparkFun_VL53L1X.h"
SFEVL53L1X distanceSensor;

#define PIN            6

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      24
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);


bool testmode = false;


int distanceLimit = 3000;// the max read distance of teh sensor. it actually reads greater distances but this is the safe limit
int innerLimit = 500;// inner maximum
int pix = 24;// an int for the number of pix 
int basePixel = (distanceLimit-innerLimit)/ pix;// our base unit hw many mm are represented by one pixel
//
//SMOOTHING
const int numReadings = 10;     // default 10
int readings[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
int total = 0;                  // the running total
int average = 0;                // the average

void setup(void){
  Wire.begin();
  Serial.begin(9600);
  Serial.println("VL53L1X Qwiic Test");

  if (distanceSensor.begin() == 0) //Begin returns 0 on a good init
  {
    Serial.println("Sensor online!");
  }
  pixels.begin(); // This initializes the NeoPixel library.
   //
  //SMOOTHING
  //
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
  }
}
//

void loop(void){
//SMOOTHING
  // subtract the last reading:
  total = total - readings[readIndex];
  // read from the sensor:
  
  distanceSensor.startRanging(); //Write configuration bytes to initiate measurement
  int distance = distanceSensor.getDistance(); //Get the result of the measurement from the sensor
  distanceSensor.stopRanging();

//
  readings[readIndex] = distance;
  
  // add the reading to the total:
  total = total + readings[readIndex];
  // advance to the next position in the array:
  readIndex = readIndex + 1;

  // if we're at the end of the array...
  if (readIndex >= numReadings) {
    // ...wrap around to the beginning:
    readIndex = 0;
  }

  // calculate the average:
  average = total / numReadings;
 
 
 
  for(int i=0;i<NUMPIXELS;i++){
    if(average < distanceLimit){
      
      
      
      if((average + innerLimit) >= ( ((i+1) * basePixel)+innerLimit) ){  
        pixels.setPixelColor(i, pixels.Color(0,150,0));
      }else{
        pixels.setPixelColor(i, pixels.Color(150,0,0));
      }
    }else{
      if(testmode){
      pixels.setPixelColor(i, pixels.Color(150,0,150));
      }else{
        pixels.setPixelColor(i, pixels.Color(0,150,0));
      }
    }
   
   
   
   
   pixels.show(); // This sends the updated pixel color to the hardware.
   delay(1);        // delay in between reads for stability
  
  }
Serial.print(average);
Serial.println();
//Serial.print("basePixel = ");
//Serial.print(basePixel);
//Serial.println();

}
