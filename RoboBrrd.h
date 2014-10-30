/**
 * RoboBrrd Library v1.0
 * ---------------------
 *
 * Bring your RoboBrrd to life with the help of this library.
 * Squeak chirp weewoo chirp!
 * When we say 'right' or 'left', we mean that this is the
 * *robot's* right or left.
 *
 * Callback Methods
 * ----------------
 *
 * There are a few callback methods that you need to have
 * defined in your Arduino's sketch in order for the
 * library to not crash RoboBrrd (and cause it to reset
 * or freeze). Here is the list:
 *
 * Promulgate Related-
 
   void received_action_hw(char action, char cmd, uint8_t key, uint16_t val, char delim)
 
   void received_action_sw(char action, char cmd, uint8_t key, uint16_t val, char delim)
   
   void transmit_complete()
 
 *
 * Light Sensor Related-

   void ldr_left_dark()

   void ldr_left_bright()

   void ldr_right_dark()

   void ldr_right_bright()
 
 *
 * API
 * ----------------
 *
 * There is an API that you can use to send commands to RoboBrrd!
 * It is implemented in both hardware and software Serial.
 * For more information, please see Serial_API_Info.h.
 *
 * 
 * Questions? Tribbles?
 * ----------------
 * 
 * If you are stuck on something with RoboBrrd, we can try to help!
 * Please post your question in the RoboBrrd Forum:
 * --> http://robobrrd.com/forum
 *
 * For more info about RoboBrrd:
 * --> http://robobrrd.com
 *
 * By Erin RobotGrrl for RoboBrrd.com
 *
 * Similar to the ClydeDev library, thanks to Fabule for the
 * inspiration.
 * 
 * Copyright (c) 2014 Erin Kennedy.
 * Licensed under MIT License, see license.txt for more info.
 *
 */

#ifndef _ROBOBRRD_H_
#define _ROBOBRRD_H_

#include <Servo.h>
#include <EEPROM.h>

#include "Streaming.h"
#include "Promulgate.h"
#include "MemoryMap.h"

#if ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

class RoboBrrd {
	

	public:


		RoboBrrd();
		bool init();
		void update();


		// -- pin setters (just in case their robobrrd is configured differently)
		void setRotationalServoPin(uint8_t pin) { rotational_servo_pin = pin; };
		void setBeakServoPin(uint8_t pin) { beak_servo_pin = pin; };
		void setRwingServoPin(uint8_t pin) { rwing_servo_pin = pin; };
		void setLwingServoPin(uint8_t pin) { lwing_servo_pin = pin; };
		void setRedLedPin(uint8_t pin) { led_pins[0] = pin; };
		void setGreenLedPin(uint8_t pin) { led_pins[1] = pin; };
		void setBlueLedPin(uint8_t pin) { led_pins[2] = pin; };
		void setSpkrPin(uint8_t pin) { spkr_pin = pin; };
		void setLdrLeftPin(uint8_t pin) { ldr_left_pin = pin; };
		void setLdrRightPin(uint8_t pin) { ldr_right_pin = pin; };



		// -- servo related
		enum ServoNum {
    	ROTATION_SERVO,
    	BEAK_SERVO,
    	RWING_SERVO,
    	LWING_SERVO
    };

    void setAutoDetach(bool tf);
		void setServoHome(uint8_t ser, uint16_t pos);
		void setServoDefaults(uint8_t ser, uint16_t p1, uint16_t p2, uint16_t p3);
		void setServoDefaultP2(uint8_t ser, uint16_t pos);
		void setServoDefaultP3(uint8_t ser, uint16_t pos);

		void initServos();
    void servosAttach();
    void servoAttach(uint8_t ser);
    void servosDetach();
    void servoDetach(uint8_t ser);
    void servosHome();
		void servoMove(uint8_t ser, uint8_t pos, uint16_t del);
		

	
		// -- leds
		float current_hsi[3];
		uint8_t current_rgb[3];

		void setMaxBrightness(float b) { MAX_BRIGHTNESS = b; }

		void ledsDefault();
		void saveLedsDefault();
		void setEyesRGB(uint8_t r, uint8_t g, uint8_t b);
    void setEyesHSI(float H, float S, float I);
  


    // -- sensors
    void enableLightSensors(bool tf) { light_sensors_enabled = tf; }

    uint16_t getLeftLDR() { return current_ldr_left_val; }
		uint16_t getRightLDR() { return current_ldr_right_val; }

    void setLdrLeftDarkHandler( void(*function)() ) { ldrLeftDark = function; }
    void setLdrLeftBrightHandler( void(*function)() ) { ldrLeftBright = function; }
    void setLdrRightDarkHandler( void(*function)() ) { ldrRightDark = function; }
    void setLdrRightBrightHandler( void(*function)() ) { ldrRightBright = function; }

    void setBrightThresh(uint8_t t) { BRIGHT_THRESH = t; }
    void setDarkThresh(uint8_t t) { DARK_THRESH = t; }

    void initLightSensors();
    uint8_t isLeftLDRTriggered();
    uint8_t isRightLDRTriggered();



    // -- emote
    void setAutoSave(bool yesno) { emote_auto_save = yesno; }
    void setEmoteHappy(uint8_t v) { emote_happy = check8Bit(v); }
    void setEmoteChill(uint8_t v) { emote_chill = check8Bit(v); }
		void setEmoteFood(uint8_t v) { emote_food = check8Bit(v); }
    void setEmoteWater(uint8_t v) { emote_water = check8Bit(v); }
    void setEmotePlay(uint8_t v) { emote_play = check8Bit(v); }
    void setMood(uint8_t happy, uint8_t chill);
    void setState(uint8_t food, uint8_t water, uint8_t play);

    uint8_t getEmoteHappy() { return emote_happy; }
    uint8_t getEmoteChill() { return emote_chill; }
    uint8_t getEmoteFood() { return emote_food; }
    uint8_t getEmoteWater() { return emote_water; }
    uint8_t getEmotePlay() { return emote_play; }

    void saveMood();
    void saveState();



    // -- speaker
    void robotgrrlSong();
		void playTone(uint16_t tone, uint16_t duration);



		// -- movements
		void beakOpen();
		void beakClose();
		void beakHome();
		void beakSnip();
		void beakLaugh();
		void beakPos(uint8_t pos);

		void rotateLeft();
		void rotateRight();
		void rotateHome();
		void shakeShake();
		void rotateBounce();
		void rotatePos(uint8_t pos);

		void rightWingUp();
		void rightWingDown();
		void rightWingHome();
		void rightWingWave();
		void rightWingGust();
		void rightWingPos(uint8_t pos);

		void leftWingUp();
		void leftWingDown();
		void leftWingHome();
		void leftWingWave();
		void leftWingGust();
		void leftWingPos(uint8_t pos);

		void bothWingsUp(bool up);
		void bothWingWave(bool opposite);
		void bothWingGust(bool opposite);



    // -- misc
    enum Level {
      DEBUG,
      INFO,
      WARN,
      ERROR_
    };

    Level LOG_LEVEL;

    void setDebugStream(Stream *d, int l) { debug_stream = d; LOG_LEVEL = (Level)l; }

    uint8_t easterEgg();

    int availableMemory();
    bool headsOrTails();



    // -- promulgate
    Promulgate promulgate_hw;
    Promulgate promulgate_sw;

    bool apiModeHw() { return api_mode_hw; }
		bool apiModeSw() { return api_mode_sw; }

    void received_action_hw(char action, char cmd, uint8_t key, uint16_t val, char delim);
		

    void initPromulgateHw(Stream *in, Stream *out);
    void initPromulgateSw(Stream *in, Stream *out);
    void transmit_message(uint8_t stream, char action, char cmd, uint8_t key, uint16_t val, char delim);
    void parse_action(uint8_t stream, char action, char cmd, uint8_t key, uint16_t val, char delim);




		// -- common colours
		static const float hue_red = 330.0;
		static const float hue_orange = 20.0;
		static const float hue_yellow = 45.0;
		static const float hue_green = 90.0;
		static const float hue_blue = 210.0;
		static const float hue_violet = 270.0;



	private:

		// -- pins
		uint8_t rotational_servo_pin;
		uint8_t beak_servo_pin;
		uint8_t rwing_servo_pin;
		uint8_t lwing_servo_pin;
		uint8_t led_pins[3];
		uint8_t spkr_pin;
		uint8_t ldr_left_pin;
		uint8_t ldr_right_pin;



		// -- servos
		bool auto_detach;

		Servo servo[4];
		long last_servo_move[4];
		uint8_t last_servo_pos[4];
		uint8_t last_servos_moved[5];

		uint8_t rot_pos[3];
		uint8_t beak_pos[3];
		uint8_t rwing_pos[3];
		uint8_t lwing_pos[3];



		// -- leds
		float MAX_BRIGHTNESS;

		float last_led_hsi[3];
		uint8_t last_led_rgb[3];

		void hsi2rgb(float H, float S, float I, int *rgb);
    void rgb2hsv(uint8_t r, uint8_t g, uint8_t b, double *hsv);



		// -- sensors
		
		// sample every x milliseconds
		static const uint16_t TIME_THRESH = 250;

		// size of the sample window to determine the current
		// sensor value (when averaged)
		static const uint16_t SAMPLE_SIZE = 10;

		// the threshold for ignoring raw data from the
		// difference between current and previous value
		static const uint16_t DELTA_THRESH = 80;

		// the number of changes detected to count for
		// is_changed to be set to true
		static const uint16_t CHANGE_THRESH = 6;

		// the amount of time (ms) from the last trigger to reset the
    // trigger count to 0 -- specifically when the trigger is 'fresh'
    static const uint16_t LDR_LAST_RESET = 250;

    // the amount of time (ms) for a servo to be auto
    // detached after it has last moved
    static const uint16_t AUTO_DETACH_TIMER = 3000;

    // the change has to go up by this much to be said
		// that there is now more brightness
		uint8_t BRIGHT_THRESH;

		// the change has to go down by this much to be said
		// that there is now more darkness
		uint8_t DARK_THRESH;

		// enabled
		bool light_sensors_enabled;

		// sample related
		long last_sampled;
		uint16_t sample_count;
		uint16_t ldr_left_total;
		uint16_t ldr_right_total;
		bool done_calibration;

		// raw vals
		uint16_t current_ldr_left_raw;
		uint16_t current_ldr_right_raw;
		uint16_t previous_ldr_left_raw;
		uint16_t previous_ldr_right_raw;
		uint16_t ldr_left_min_raw;
		uint16_t ldr_left_max_raw;
		uint16_t ldr_right_min_raw;
		uint16_t ldr_right_max_raw;

		// averaged vals
		uint16_t current_ldr_left_val;
		uint16_t current_ldr_right_val;
		uint16_t previous_ldr_left_val;
		uint16_t previous_ldr_right_val;

		// trigger related
		uint16_t left_dark_thresh;
		uint16_t left_bright_thresh;
		uint16_t right_dark_thresh;
		uint16_t right_bright_thresh;

		uint16_t math_abs(uint16_t a, uint16_t b);
		double threeway_max(double a, double b, double c);
    double threeway_min(double a, double b, double c);

    void calibrateLightSensors();

    void (*ldrLeftDark)();
		void (*ldrLeftBright)();
		void (*ldrRightDark)();
		void (*ldrRightBright)();



		// -- emote
		bool emote_auto_save;
		long last_emote_save;

		uint8_t emote_happy;
		uint8_t emote_chill;
		uint8_t emote_food;
		uint8_t emote_water;
		uint8_t emote_play;

		void initEmotes();



		// -- misc
		Stream *debug_stream;
		bool isMemInit();
		uint8_t check8Bit(uint16_t v);


		// -- promulgate
		bool api_mode_sw;
		bool api_mode_hw;

};

#endif
