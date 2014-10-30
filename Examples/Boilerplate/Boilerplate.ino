/* RoboBrrd Boilerplate
 * --------------------
 *
 * This is a boilerplate sketch for RoboBrrd! You can use it to
 * easily create new sketches as you please.
 *
 * If you have any questions, please ask on the forums:
 * --> http://robobrrd.com/forum
 *
 * For more information about RoboBrrd please see:
 * --> http://robobrrd.com
 *
 * By Erin RobotGrrl for RoboBrrd.com
 * Thursday, October 23, 2014
 */

#include <Servo.h>
#include <EEPROM.h>
#include "Streaming.h"
#include "Promulgate.h"
#include "RoboBrrd.h"

RoboBrrd robobrrd;


void setup() {
  
  Serial.begin(9600); // remember to start serial! (doesn't matter what baud for robobrrd)
  
  robobrrd = RoboBrrd(); // creating our robobrrd
  
  robobrrd.LOG_LEVEL = RoboBrrd::INFO; // setting the debug level to info
  
  robobrrd.init(); // starting our robobrrd
  
  robobrrd.setAutoDetach(true); // setting the servos to detach after movements
  
  addPromulgateCallbacks(); // add in our callbacks for the API
  
  addLightCallbacks(); // setting our callbacks for the light sensors
  
  Serial << "Hey there! Chirpy chirp!" << endl;
  
}

void loop() {
  
  robobrrd.update(); // keepin' robobrrd alive each loop iteration
  
}

