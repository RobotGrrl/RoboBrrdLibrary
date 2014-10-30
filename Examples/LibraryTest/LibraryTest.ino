/* RoboBrrd Library Test
 * ---------------------
 *
 * This is a way to test your RoboBrrd with the library.
 *
 * You can test your RoboBrrd's movements by opening the Serial Monitor
 * and entering '>' to progress through the movements. You can enter
 * '<' to go back too.
 *
 * You can also use the API mode to control RoboBrrd! Be sure to check
 * out the API documentation for more info.
 * --> http://robobrrd.com/rb3d/api.php
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

long last_blink = 0;
uint8_t instruction_advance = 0;
uint8_t prev_instr = 0;

void setup() {
  
  Serial.begin(9600); // remember to start serial! (doesn't matter what baud for robobrrd)
  
  robobrrd = RoboBrrd(); // creating our robobrrd
  
  robobrrd.LOG_LEVEL = RoboBrrd::INFO; // setting the debug level to info
  
  robobrrd.init(); // starting our robobrrd
  
  robobrrd.setAutoDetach(true); // setting the servos to detach after movements
  
  addPromulgateCallbacks(); // add in our callbacks for the API
  
  addLightCallbacks(); // setting our callbacks for the light sensors
  
  Serial << "Hey there! Chirpy chirp!" << endl;
  
  // if you need to adjust your servo positions afterwards, uncomment this
  // block of code and add in the proper settings!
  //robobrrd.setServoDefaults(RoboBrrd::ROTATION_SERVO, 90, 0, 180); // home, left, right
  //robobrrd.setServoDefaults(RoboBrrd::BEAK_SERVO, 100, 160, 30); // home, open, closed
  //robobrrd.setServoDefaults(RoboBrrd::RWING_SERVO, 40, 10, 55); // home, up, down
  //robobrrd.setServoDefaults(RoboBrrd::LWING_SERVO, 40, 10, 55); // home, up, down
  
  eepromReadTest();
  
}

void loop() {
  
  if(instruction_advance != prev_instr) {
    
    prev_instr = instruction_advance;
    
    switch(instruction_advance) {
      case 0:
      break;
      case 1:
        Serial << "Beak open" << endl;
        robobrrd.beakOpen();
        delay(100);
      break;
      case 2:
        Serial << "Beak close" << endl;
        robobrrd.beakClose();
        delay(100);
      break;
      case 3:
        Serial << "Beak home" << endl;
        robobrrd.beakHome();
        delay(100);
      break;
      case 4:
        Serial << "Rotate left" << endl;
        robobrrd.rotateLeft();
        delay(100);
      break;
      case 5:
        Serial << "Rotate right" << endl;
        robobrrd.rotateRight();
        delay(100);
      break;
      case 6:
        Serial << "Rotate home" << endl;
        robobrrd.rotateHome();
        delay(100);
      break;
      case 7:
        Serial << "Right wing up" << endl;
        robobrrd.rightWingUp();
        delay(100);
      break;
      case 8:
        Serial << "Right wing down" << endl;
        robobrrd.rightWingDown();
        delay(100);
      break;
      case 9:
        Serial << "Right wing home" << endl;
        robobrrd.rightWingHome();
        delay(100);
      break;
      case 10:
        Serial << "Left wing up" << endl;
        robobrrd.leftWingUp();
        delay(100);
      break;
      case 11:
        Serial << "Left wing down" << endl;
        robobrrd.leftWingDown();
        delay(100);
      break;
      case 12:
        Serial << "Left wing home" << endl;
        robobrrd.leftWingHome();
        delay(100);
      break;
      case 13:
        Serial << "Red eyes" << endl;
        robobrrd.setEyesRGB(255, 0, 0);
        delay(100);
      break;
      case 14:
        Serial << "Green eyes" << endl;
        robobrrd.setEyesRGB(0, 255, 0);
        delay(100);
      break;
      case 15:
        Serial << "Blue eyes" << endl;
        robobrrd.setEyesRGB(0, 0, 255);
        delay(100);
      break;
    }
    
  }
  
  
  // if you want to see the available memory, you can uncomment the below line
  //Serial << robobrrd.availableMemory() << endl;
  
  robobrrd.update(); // keepin' robobrrd alive each loop iteration
  
  
  if(millis()-last_blink >= 1000) {
    robobrrd.setEyesHSI(0.0, 0.0, 0.6);
    delay(50);
    robobrrd.setEyesHSI(0.0, 0.0, 1.0);
    last_blink = millis();
  }
  
  
}


void eepromReadTest() {
 
  uint8_t del = 50;
  
  Serial << "EEPROM Read test" << endl;
  
  Serial << "led_addr: " 
  << EEPROM.read(led_addr[0]) << " "
  << EEPROM.read(led_addr[1]) << " "
  << EEPROM.read(led_addr[2])
  << endl;
  
  delay(100);
  
  Serial << "rot_addr: " 
  << EEPROM.read(rot_addr[0]) << " "
  << EEPROM.read(rot_addr[1]) << " "
  << EEPROM.read(rot_addr[2])
  << endl;
  
  delay(100);
  
  Serial << "beak_addr: " 
  << EEPROM.read(beak_addr[0]) << " "
  << EEPROM.read(beak_addr[1]) << " "
  << EEPROM.read(beak_addr[2])
  << endl;
  
  delay(100);
  
  Serial << "rwing_addr: " 
  << EEPROM.read(rwing_addr[0]) << " "
  << EEPROM.read(rwing_addr[1]) << " "
  << EEPROM.read(rwing_addr[2])
  << endl;
  
  delay(100);
  
  Serial << "lwing_addr: " 
  << EEPROM.read(lwing_addr[0]) << " "
  << EEPROM.read(lwing_addr[1]) << " "
  << EEPROM.read(lwing_addr[2])
  << endl;
  
  delay(100);
  
  Serial << "mood_addr: " 
  << EEPROM.read(mood_addr[0]) << " "
  << EEPROM.read(mood_addr[1])
  << endl;
  
  delay(100);
  
  Serial << "state_addr: " 
  << EEPROM.read(state_addr[0]) << " "
  << EEPROM.read(state_addr[1]) << " "
  << EEPROM.read(state_addr[2])
  << endl;
  
  delay(100);
  
  Serial << "init_addr: " 
  << EEPROM.read(init_addr)
  << endl;
  
  Serial << ".....Done!" << endl;
  
}


