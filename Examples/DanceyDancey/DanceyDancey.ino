/* RoboBrrd Dancey Dancey
 * -----------------------
 *
 * A little example of how you could set up a sequence of moves
 * to perform in a dance routine or something!
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

const uint8_t num_moves = 7; // total number of moves in the dance

// this array is comprised of multiple moves. each move is an id number followed
// by a time (in ms) to wait before starting the next move. the id number for
// each move refers to the moves in the function performMove, located at the
// bottom of this page!
uint16_t wave_dance[num_moves][2] = { {0, 500},
                                      {1, 500},
                                      {2, 500},
                                      {3, 500},
                                      {8, 1000},
                                      {0, 500},
                                      {2, 500} };

long last_move = 0;
uint16_t move_duration = 0;
uint8_t move_num = 0;

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
  
  
  // before starting a new move, make sure that the previous move has occured
  if(millis()-last_move >= move_duration) {
    
    performMove(wave_dance[move_num][0]);
    move_duration = wave_dance[move_num][1];
    
    // now we increment the move_num, so that on the next loop iteration it
    // will perform the next move, but move_duration is still referring to the
    // time of the previous move, so we know when to start the new move. it all
    // works out!
    move_num++;
    if(move_num >= num_moves) move_num = 0;
    
    last_move = millis();
  }
  
  
}


void performMove(uint8_t m) {
  
  Serial << "Performing move: " << m << endl;
  
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

