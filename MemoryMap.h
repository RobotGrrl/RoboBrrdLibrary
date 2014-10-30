/**
 * Memory Map
 * -----------
 *
 * All of the addresses for accessing the
 * proper places in EEPROM memory.
 */


#if ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

// default eye colour, in RGB
static const uint8_t led_addr[] = {0, 1, 2};

// rotational servo: home pos, left, right
static const uint8_t rot_addr[] = {3, 4, 5};

// beak servo: home pos, open, closed
static const uint8_t beak_addr[] = {6, 7, 8};

// right wing servo: home pos, up, down
static const uint8_t rwing_addr[] = {9, 10, 11};

// left wing servo: home pos, up, down
static const uint8_t lwing_addr[] = {12, 13, 14};

// mood: happy/sad, chill/angry
static const uint8_t mood_addr[] = {15, 16};

// state: food, water, play
static const uint8_t state_addr[] = {17, 18, 19};

// used to show that this has been initialised
static const uint8_t init_addr = 99;