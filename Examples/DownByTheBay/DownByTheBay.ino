/* RoboBrrd Down By The Bay Example
 * --------------------------------
 *
 * Down by the bay, where the watermelons grow!
 * Have fun with this example program for RoboBrrd.
 * 
 * To begin the experience, upload the code to your RoboBrrd,
 * and then open the Serial Monitor.
 *
 * Every 20 seconds, RoboBrrd will output a line of the song
 * as well as change its eye colour and do a movement of some
 * sort. This all happens in the downByTheBay() function below,
 * so check it out if you want to learn more!
 * 
 * If you have any questions, please ask on the forums:
 * --> http://robobrrd.com/forum
 *
 * For more information about RoboBrrd please see:
 * --> http://robobrrd.com
 * 
 * By Erin RobotGrrl for RoboBrrd.com
 * Monday, October 27, 2014
 */
 
#include <Servo.h>
#include <EEPROM.h>
#include "Streaming.h"
#include "Promulgate.h"
#include "RoboBrrd.h"

RoboBrrd robobrrd;

long last_poem = 0;
uint8_t num_phrases = 11;
String phrases[11] = {
  "tree, singing with a bee?",
  "fishstick, wearing ketchup lipstick?",
  "fox, hiding in a box?",
  "fish do a hula in a dish?",
  "duck in a pickup truck?",
  "bee, with a sunburnt knee?",
  "snake baking a cake?",
  "cat, wearing a hat?",
  "ant, climbing a plant?",
  "whale, with a polka-dot tail?",
  "bear, combing his hair?" };
  


void setup() {
  
  Serial.begin(9600); // remember to start serial! (doesn't matter what baud for robobrrd)
  
  robobrrd = RoboBrrd(); // creating our robobrrd
  
  robobrrd.LOG_LEVEL = RoboBrrd::DEBUG; // setting the debug level to info
  
  robobrrd.enableLightSensors(false); // we do not need the light sensors for this demo
  
  robobrrd.init(); // starting our robobrrd
  
  robobrrd.setAutoDetach(true); // setting the servos to detach after movements
  
  addPromulgateCallbacks(); // add in our callbacks for the API
  
  addLightCallbacks(); // setting our callbacks for the light sensors
  
  Serial << F("Hey there! Chirpy chirp!") << endl; // wrapping the string in F() stores it in flash rather than RAM
  
}

void loop() {
  
  robobrrd.update(); // keepin' robobrrd alive each loop iteration
  
  
  if(millis()-last_poem >= 20000) {
    Serial << "\n\n\n\n";
    downByTheBay();
    last_poem = millis();
  }
  
}

void downByTheBay() {
 
  Serial << F("Down by the bay,") << endl;
  robobrrd.setEyesHSI(10.0, 1.0, 1.0);
  robobrrd.bothWingsUp(true);
  delay(1000);
  
  Serial << F("Where the watermelons grow,") << endl;
  robobrrd.setEyesHSI(30.0, 1.0, 1.0);
  robobrrd.leftWingDown();
  delay(1000);
  
  Serial << F("Back to my home,") << endl;
  robobrrd.setEyesHSI(50.0, 1.0, 1.0);
  robobrrd.rightWingDown();
  delay(1000);
  
  Serial << F("I dare not go,") << endl;
  robobrrd.setEyesHSI(70.0, 1.0, 1.0);
  robobrrd.rotateLeft();
  delay(1000);
  
  Serial << F("or if I do,") << endl;
  robobrrd.setEyesHSI(90.0, 1.0, 1.0);
  robobrrd.rotateHome();
  robobrrd.rightWingUp();
  delay(1000);
  
  Serial << F("My mother will say:") << endl;
  robobrrd.setEyesHSI(110.0, 1.0, 1.0);
  robobrrd.rotateRight();
  delay(1000);
  
  Serial << F("Did you ever see a ") << endl;
  robobrrd.setEyesHSI(130.0, 1.0, 1.0);
  robobrrd.rotateHome();
  robobrrd.leftWingUp();
  delay(1000);
  
  int random_phrase = (int)random(0, num_phrases);
  Serial.println(phrases[random_phrase]);
  robobrrd.setEyesHSI(150.0, 1.0, 1.0);
  robobrrd.shakeShake();
  delay(1000);
  
  Serial << F("...down by the bay?") << endl;
  robobrrd.setEyesHSI(170.0, 1.0, 1.0);
  robobrrd.bothWingWave(true);
  robobrrd.bothWingGust(false);
  delay(1000);
  
}
