/* RoboBrrd Fun Behaviour Example
 * ------------------------------
 * 
 * RoboBrrd is chirping, moving, and its eyes change according to the light
 * sensor results!
 *
 * This is just a fun little example to show how to do multiple things, and
 * also how to do something based on the light sensor callbacks.
 *
 * If you have any questions, please ask on the forums:
 * --> http://robobrrd.com/forum
 *
 * For more information about RoboBrrd please see:
 * --> http://robobrrd.com
 *
 * By Erin RobotGrrl for RoboBrrd.com
 * Wednesday, October 29, 2014
 */
 

#include <Servo.h>
#include <EEPROM.h>
#include "Streaming.h"
#include "Promulgate.h"
#include "RoboBrrd.h"

RoboBrrd robobrrd;

long last_light_sensor = 0;
long last_move = 0;
long last_chirp = 0;


void setup() {
  
  Serial.begin(9600); // remember to start serial! (doesn't matter what baud for robobrrd)
  
  robobrrd = RoboBrrd(); // creating our robobrrd
  
  robobrrd.LOG_LEVEL = RoboBrrd::INFO; // setting the debug level to info
  
  robobrrd.init(); // starting our robobrrd
  
  robobrrd.setAutoDetach(true); // setting the servos to detach after movements
  
  addPromulgateCallbacks(); // add in our callbacks for the API
  
  addLightCallbacks(); // setting our callbacks for the light sensors
  
  Serial << "Hey there! Chirpy chirp!" << endl;
  
  robobrrd.setEyesHSI(robobrrd.hue_green, 1.0, 1.0);
  
}

void loop() {
  
  robobrrd.update(); // keepin' robobrrd alive each loop iteration
  
  
  // check to make sure there has been no change in the light sensors for the past
  // three seconds. if there was no change, then turn the eyes back to green!
  if(millis()-last_light_sensor >= 3000) {
    robobrrd.setEyesHSI(robobrrd.hue_green, 1.0, 1.0);
  }
  
  
  // perform a different movement every 5 seconds
  if(millis()-last_move >= 5000) {
    performMove((uint8_t)random(0, 11));
    last_move = millis();
  }
  
  
  // let's make robobrrd chirp every 4 seconds
  if(millis()-last_chirp >= 4000) {
    
    uint16_t note = (uint16_t)random(100, 1000);
    
    // either play an ascending or descending melody
    if(robobrrd.headsOrTails()) {
      robobrrd.playTone(note, 50);
      robobrrd.playTone(note+200, 50);
      robobrrd.playTone(note+500, 50);
    } else {
      robobrrd.playTone(note+500, 50);
      robobrrd.playTone(note+200, 50);
      robobrrd.playTone(note, 50);
    }
    
    last_chirp = millis();
    
  }
  
}


void performMove(uint8_t m) {
  
  switch(m) {
    case 0:
      robobrrd.leftWingUp();
    break;
    case 1:
      robobrrd.leftWingDown();
    break;
    case 2:
      robobrrd.rightWingUp();
    break;
    case 3:
      robobrrd.rightWingDown();
    break;
    case 4:
      robobrrd.rotateLeft();
    break;
    case 5:
      robobrrd.rotateRight();
    break;
    case 6:
      robobrrd.rotateHome();
    break;
    case 7:
      robobrrd.bothWingWave(false);
    break;
    case 8:
      robobrrd.bothWingGust(false);
    break;
    case 9:
      robobrrd.beakOpen();
    break;
    case 10:
      robobrrd.beakClose();
    break;
  }
  
}


