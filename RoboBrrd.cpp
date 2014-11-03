/**
 * Copyright (c) 2014 Erin Kennedy, All rights reserved.
 * Licensed under MIT License, see license.txt for more info.
 */ 

 #include "RoboBrrd.h"

/**
 * Initialisation
 */

RoboBrrd::RoboBrrd() {

 	debug_stream = &Serial;
 	LOG_LEVEL = ERROR_;
 	light_sensors_enabled = true;
 	
 }



bool RoboBrrd::init() {

	if(LOG_LEVEL <= DEBUG) *debug_stream << "Beginning initialisation of RoboBrrd" << endl; 


	rotational_servo_pin = 4;
	beak_servo_pin = 11;
	rwing_servo_pin = 10;
	lwing_servo_pin = 9;
	led_pins[0] = 3;
	led_pins[1] = 6;
	led_pins[2] = 5;
	spkr_pin = A4;
	ldr_left_pin = A0;
	ldr_right_pin = A1;


	auto_detach = false;

	MAX_BRIGHTNESS = 1.0;


	// add all the pins as outputs
	pinMode(led_pins[0], OUTPUT);
	pinMode(led_pins[1], OUTPUT);
	pinMode(led_pins[2], OUTPUT);
	pinMode(spkr_pin, OUTPUT);
	pinMode(ldr_left_pin, INPUT);
	pinMode(ldr_right_pin, INPUT);


	// if the eeprom memory isn't loaded yet, we should put some data there
	// just to make everything run smoothly. they can later use the
	// robobrrd dashboard to make adjustments (or the api).
	if(!isMemInit()) {

		if(LOG_LEVEL <= DEBUG) *debug_stream << "EEPROM Memory"; 

		current_rgb[0] = 128;
		current_rgb[1] = 10;
		current_rgb[2] = 128;
		saveLedsDefault();

		setServoDefaults(ROTATION_SERVO, 90, 0, 180);
		setServoDefaults(BEAK_SERVO, 100, 160, 30);
		setServoDefaults(RWING_SERVO, 40, 10, 55);
		setServoDefaults(LWING_SERVO, 40, 10, 55);

		emote_happy = 80;
		emote_chill = 50;
		saveMood();

		emote_food = 80;
		emote_water = 80;
		emote_play = 60;
		saveState();

		// it is initialised now, let's flip the switch!
		EEPROM.write(init_addr, true);

		if(LOG_LEVEL <= DEBUG) Serial << "......Done" << endl; 

	} else {
		if(LOG_LEVEL <= DEBUG) *debug_stream << "Did not require EEPROM memory initialisation" << endl; 
	}


	// servos
	for(uint8_t i=0; i<4; i++) {
		last_servo_move[i] = 0;
		last_servo_pos[i] = 0;
		last_servos_moved[i] = 0;
	}
	last_servos_moved[4] = 0; // because this is the size of 5 elements, not 4

	for(uint8_t i=0; i<3; i++) {
		rot_pos[i] = 0;
		beak_pos[i] = 0;
		rwing_pos[i] = 0;
		lwing_pos[i] = 0;
	}
	

	// leds
	for(uint8_t i=0; i<3; i++) {
		last_led_hsi[i] = 0.0;
		current_hsi[i] = 0.0;
		last_led_rgb[i] = 0;
		current_rgb[i] = 0;
	}


	// ldrs

	BRIGHT_THRESH = 10;
	DARK_THRESH = 8;

	// sample related
	last_sampled = 0;
	sample_count = 0;
	ldr_left_total = 0;
	ldr_right_total = 0;
	done_calibration = false;

	// raw vals
	current_ldr_left_raw = 0;
	current_ldr_right_raw = 0;
	previous_ldr_left_raw = 0;
	previous_ldr_right_raw = 0;
	ldr_left_min_raw = 0;
	ldr_left_max_raw = 0;
	ldr_right_min_raw = 0;
	ldr_right_max_raw = 0;

	// averaged vals
	current_ldr_left_val = 0;
	current_ldr_right_val = 0;
	previous_ldr_left_val = 0;
	previous_ldr_right_val = 0;

	// trigger related
	left_dark_thresh = 0;
	left_bright_thresh = 0;
	right_dark_thresh = 0;
	right_bright_thresh = 0;


	// emote
	last_emote_save = 0;
	emote_auto_save = false;


	// promulgate
	api_mode_sw = false;
	api_mode_hw = false;


	// let's begin now!

	initPromulgateHw(&Serial, &Serial);
	initEmotes();
	if(light_sensors_enabled) initLightSensors();
	initServos();


	// subsequent calls to servoMove will assume servosAttach
	// has been called at the very first moment in time, to then
	// work with auto_detach.
	servosAttach();
	servosHome();

	ledsDefault();
	robotgrrlSong();
	
	if(LOG_LEVEL <= DEBUG) Serial << "Completed initialisation!" << endl;

	return true;

}


void RoboBrrd::update() {

	if(light_sensors_enabled) {
		calibrateLightSensors();
		isLeftLDRTriggered();
		isRightLDRTriggered();
	}

	if(millis()-last_emote_save > 120000UL && emote_auto_save == true) {
		saveState();
		saveMood();
		last_emote_save = millis();
	}

	for(uint8_t i=0; i<4; i++) {
		if(millis()-last_servos_moved[i] >= AUTO_DETACH_TIMER) {
			servoDetach(i);
		}
	}

}



/**
 * Servos
 */

void RoboBrrd::setAutoDetach(bool tf) {
	auto_detach = tf;
	servosDetach();
}

void RoboBrrd::initServos() {

	// we choose to load the servo positions from eeprom into ram,
	// just in case their eeprom is blown or something. they could
	// edit this function with the proper values, and continue on
	// as normal. otherwise, they would have to edit every single
	// function. we have the ram available to do this, so no reason
	// to not do it.

	if(LOG_LEVEL <= DEBUG) Serial << "Initialising the servos from eeprom";

	rot_pos[0] = EEPROM.read(rot_addr[0]);
	rot_pos[1] = EEPROM.read(rot_addr[1]);
	rot_pos[2] = EEPROM.read(rot_addr[2]);

	beak_pos[0] = EEPROM.read(beak_addr[0]);
	beak_pos[1] = EEPROM.read(beak_addr[1]);
	beak_pos[2] = EEPROM.read(beak_addr[2]);

	rwing_pos[0] = EEPROM.read(rwing_addr[0]);
	rwing_pos[1] = EEPROM.read(rwing_addr[1]);
	rwing_pos[2] = EEPROM.read(rwing_addr[2]);

	lwing_pos[0] = EEPROM.read(lwing_addr[0]);
	lwing_pos[1] = EEPROM.read(lwing_addr[1]);
	lwing_pos[2] = EEPROM.read(lwing_addr[2]);

	if(LOG_LEVEL <= DEBUG) Serial << "......Done" << endl;

}

void RoboBrrd::setServoHome(uint8_t ser, uint16_t pos) {

	switch(ser) {
		case ROTATION_SERVO:
			EEPROM.write(rot_addr[0], pos);
			rot_pos[0] = pos;
		break;
		case BEAK_SERVO:
			EEPROM.write(beak_addr[0], pos);
			beak_pos[0] = pos;
		break;
		case RWING_SERVO:
			EEPROM.write(rwing_addr[0], pos);
			rwing_pos[0] = pos;
		break;
		case LWING_SERVO:
			EEPROM.write(lwing_addr[0], pos);
			lwing_pos[0] = pos;
		break;
	}

}


void RoboBrrd::setServoDefaults(uint8_t ser, uint16_t p1, uint16_t p2, uint16_t p3) {

	switch(ser) {
		case ROTATION_SERVO:
			EEPROM.write(rot_addr[0], p1);
			EEPROM.write(rot_addr[1], p2);
			EEPROM.write(rot_addr[2], p3);
			rot_pos[0] = p1;
			rot_pos[1] = p2;
			rot_pos[2] = p3;
		break;
		case BEAK_SERVO:
			EEPROM.write(beak_addr[0], p1);
			EEPROM.write(beak_addr[1], p2);
			EEPROM.write(beak_addr[2], p3);
			beak_pos[0] = p1;
			beak_pos[1] = p2;
			beak_pos[2] = p3;
		break;
		case RWING_SERVO:
			EEPROM.write(rwing_addr[0], p1);
			EEPROM.write(rwing_addr[1], p2);
			EEPROM.write(rwing_addr[2], p3);
			rwing_pos[0] = p1;
			rwing_pos[1] = p2;
			rwing_pos[2] = p3;
		break;
		case LWING_SERVO:
			EEPROM.write(lwing_addr[0], p1);
			EEPROM.write(lwing_addr[1], p2);
			EEPROM.write(lwing_addr[2], p3);
			lwing_pos[0] = p1;
			lwing_pos[1] = p2;
			lwing_pos[2] = p3;
		break;
	}

}


void RoboBrrd::setServoDefaultP2(uint8_t ser, uint16_t pos) {

	switch(ser) {
		case ROTATION_SERVO:
			EEPROM.write(rot_addr[1], pos);
			rot_pos[1] = pos;
		break;
		case BEAK_SERVO:
			EEPROM.write(beak_addr[1], pos);
			beak_pos[1] = pos;
		break;
		case RWING_SERVO:
			EEPROM.write(rwing_addr[1], pos);
			rwing_pos[1] = pos;
		break;
		case LWING_SERVO:
			EEPROM.write(lwing_addr[1], pos);
			lwing_pos[1] = pos;
		break;
	}

}


void RoboBrrd::setServoDefaultP3(uint8_t ser, uint16_t pos) {

	switch(ser) {
		case ROTATION_SERVO:
			EEPROM.write(rot_addr[2], pos);
			rot_pos[2] = pos;
		break;
		case BEAK_SERVO:
			EEPROM.write(beak_addr[2], pos);
			beak_pos[2] = pos;
		break;
		case RWING_SERVO:
			EEPROM.write(rwing_addr[2], pos);
			rwing_pos[2] = pos;
		break;
		case LWING_SERVO:
			EEPROM.write(lwing_addr[2], pos);
			lwing_pos[2] = pos;
		break;
	}

}


void RoboBrrd::servosAttach() {

	if(!servo[0].attached()) servo[0].attach(rotational_servo_pin);
	if(!servo[1].attached()) servo[1].attach(beak_servo_pin);
	if(!servo[2].attached()) servo[2].attach(rwing_servo_pin);
	if(!servo[3].attached()) servo[3].attach(lwing_servo_pin);

}


void RoboBrrd::servoAttach(uint8_t ser) {

	if(ser == ROTATION_SERVO) {
		if(!servo[0].attached()) servo[0].attach(rotational_servo_pin);
	} else if(ser == BEAK_SERVO) {
		if(!servo[1].attached()) servo[1].attach(beak_servo_pin);
	} else if(ser == RWING_SERVO) {
		if(!servo[2].attached()) servo[2].attach(rwing_servo_pin);
	} else if(ser == LWING_SERVO) {
		if(!servo[3].attached()) servo[3].attach(lwing_servo_pin);
	}

}


void RoboBrrd::servosDetach() {

	for(uint8_t i=0; i<4; i++) {
		if(servo[i].attached()) servo[i].detach();
	}

}


void RoboBrrd::servoDetach(uint8_t ser) {

	if(ser == ROTATION_SERVO) {
		if(servo[0].attached()) servo[0].detach();
	} else if(ser == BEAK_SERVO) {
		if(servo[1].attached()) servo[1].detach();
	} else if(ser == RWING_SERVO) {
		if(servo[2].attached()) servo[2].detach();
	} else if(ser == LWING_SERVO) {
		if(servo[3].attached()) servo[3].detach();
	}

}


void RoboBrrd::servosHome() {

	uint8_t del = 50;

	servoMove(ROTATION_SERVO, rot_pos[0], del);
	servoMove(BEAK_SERVO, beak_pos[0], del);
	servoMove(RWING_SERVO, rwing_pos[0], del);
	servoMove(LWING_SERVO, lwing_pos[0], del);
	
}


void RoboBrrd::servoMove(uint8_t ser, uint8_t pos, uint16_t del) {

	if(auto_detach) servoAttach(ser);

	servo[ser].write(pos);
	delay(del);

	if(auto_detach) servoDetach(ser);

	last_servo_move[ser] = millis();
	last_servo_pos[ser] = pos;

	for(uint8_t i=0; i<5; i++) {
		last_servos_moved[i+1] = last_servos_moved[i];
	}

	last_servos_moved[0] = ser;

}



/*
 * Light Sensors
 */

void RoboBrrd::initLightSensors() {

	if(LOG_LEVEL <= DEBUG) *debug_stream << "Calibrating light sensors" << endl;

	bool blinky = false;

	while(!done_calibration) {

		calibrateLightSensors();

		if(blinky) {
			setEyesHSI(hue_blue, 0.0, 1.0);
		} else {
			setEyesHSI(hue_blue, 0.0, 0.8);
		}

		blinky = !blinky;
		delay(100);

	}

	if(LOG_LEVEL <= DEBUG) Serial << "......Done" << endl;

}


void RoboBrrd::calibrateLightSensors() {

	// check if it is time to calibrate or not
	if(millis()-last_sampled < TIME_THRESH) return;

	// let's read the sensors now
	previous_ldr_left_raw = current_ldr_left_raw;
	previous_ldr_right_raw = current_ldr_right_raw;

	current_ldr_left_raw = analogRead(ldr_left_pin);
	current_ldr_right_raw = analogRead(ldr_right_pin);

	//if(LOG_LEVEL <= DEBUG) *debug_stream << "current raw L: " << current_ldr_left_raw << " R: "  << current_ldr_right_raw << " previous raw L: " << previous_ldr_left_raw << " R: " << previous_ldr_right_raw << endl; 


	// let's see what the change in the raw vals is. if the delta is not stable,
	// we will not calibrate!
	uint16_t raw_left_delta = 0;
	uint16_t raw_right_delta = 0;

	if(previous_ldr_left_raw != current_ldr_left_raw) {
		raw_left_delta = math_abs(previous_ldr_left_raw, current_ldr_left_raw);
	}

	if(raw_left_delta >= DELTA_THRESH) {
		if(LOG_LEVEL <= DEBUG) *debug_stream << "too much delta in raw left val: " << raw_left_delta << endl; 
		return;
	}

	if(raw_right_delta >= DELTA_THRESH) {
		if(LOG_LEVEL <= DEBUG) *debug_stream << "too much delta in raw right val: " << raw_right_delta << endl; 
		return;
	}


	// capture the min and max vals
	if(sample_count == 0) {
		ldr_left_min_raw = current_ldr_left_raw;
		ldr_left_max_raw = current_ldr_left_raw;
		ldr_right_min_raw = current_ldr_right_raw;
		ldr_right_max_raw = current_ldr_right_raw;
	}


	// set any new min's and max's
	if(current_ldr_left_raw < ldr_left_min_raw) ldr_left_min_raw = current_ldr_left_raw;
	if(current_ldr_left_raw > ldr_right_max_raw) ldr_left_max_raw = current_ldr_left_raw;
	if(current_ldr_right_raw < ldr_right_min_raw) ldr_right_min_raw = current_ldr_right_raw;
	if(current_ldr_right_raw > ldr_right_max_raw) ldr_right_max_raw = current_ldr_right_raw;


	// increment
	ldr_left_total += current_ldr_left_raw;
	ldr_right_total += current_ldr_right_raw;
	sample_count++;

	//if(LOG_LEVEL <= DEBUG) *debug_stream << "sample count: " << sample_count << endl;
  

  // now it is time to calculate the average val
  if(sample_count >= SAMPLE_SIZE) {


  	// calculate average
  	previous_ldr_left_val = current_ldr_left_val;
  	previous_ldr_right_val = current_ldr_left_val;

  	float temp1 = (float)( (float)(ldr_left_total) / SAMPLE_SIZE );
  	current_ldr_left_val = (uint16_t)temp1;

  	float temp2 = (float)( (float)(ldr_right_total) / SAMPLE_SIZE );
  	current_ldr_right_val = (uint16_t)temp2;


  	// print it out
  	if(LOG_LEVEL <= INFO) *debug_stream << "current val L: " << current_ldr_left_val << " R: " << current_ldr_right_val << endl; 
    if(LOG_LEVEL <= INFO) *debug_stream << "L min: " << ldr_left_min_raw << " L max: " << ldr_left_max_raw << endl; 
    if(LOG_LEVEL <= INFO) *debug_stream << "R min: " << ldr_right_min_raw << " R max: " << ldr_right_max_raw << endl; 


    // reset!
		ldr_left_total = 0;
		ldr_right_total = 0;
		sample_count = 0;

		// calculate the new threshold values
		left_bright_thresh = previous_ldr_left_val + BRIGHT_THRESH;
		right_bright_thresh = previous_ldr_right_val + BRIGHT_THRESH;
		left_dark_thresh = previous_ldr_left_val - DARK_THRESH;
		right_dark_thresh = previous_ldr_right_val - DARK_THRESH;
		
		// let's hope these will save us some day
		if( (int)previous_ldr_left_val + (int)BRIGHT_THRESH > 1023 ) left_bright_thresh = 1023;
		if( (int)previous_ldr_right_val + (int)BRIGHT_THRESH > 1023 ) right_bright_thresh = 1023;
		
		if( (int)previous_ldr_left_val - (int)DARK_THRESH < 0 ) left_dark_thresh = 0;
		if( (int)previous_ldr_right_val - (int)DARK_THRESH < 0 ) right_dark_thresh = 0;

		// done for now!
		done_calibration = true;

  }

	last_sampled = millis();

}


// ldr states: 0 = normal, 1 = dark, 2 = bright

uint8_t RoboBrrd::isLeftLDRTriggered() {

	uint8_t state = 0;

	if(current_ldr_left_val < left_dark_thresh) {
		state = 1;
	} else if(current_ldr_left_val > left_bright_thresh) {
		state = 2;
	} else {
		state = 0;
	}


	if(state == 1) {

		if(LOG_LEVEL <= DEBUG) *debug_stream << "dark! (L) " << current_ldr_left_val << " < " << left_dark_thresh << endl;

		if(ldrLeftDark) ldrLeftDark();

	}


	if(state == 2) {

		if(LOG_LEVEL <= DEBUG) *debug_stream << "bright! (L) " << current_ldr_left_val << " > " << left_bright_thresh << endl;

		if(ldrLeftBright) ldrLeftBright();

	}


	return state;

	/*
	// this is just some old code that might be useful, if you ever want to 
	// have the light and dark readings less sensitive.

	// reset the counter if this is the first trigger in quite some time
	if(millis()-last_left_trigger >= LDR_LAST_RESET && last_left_trigger != 0) {
		trigger_left_sample = 0;
	}

	// increment and record the trigger...
	trigger_left_sample++;
	last_left_trigger = millis();

	// if we exceed the specific number of threshold changes, then set the new state!
	if(trigger_left_sample > CHANGE_THRESH) {

		trigger_left_sample = 0;

		ldr_left_state = 1;
		return ldr_left_state;

	}
	*/

}


uint8_t RoboBrrd::isRightLDRTriggered() {

	uint8_t state = 0;

	if(current_ldr_right_val < right_dark_thresh) {
		state = 1;
	} else if(current_ldr_right_val > right_bright_thresh) {
		state = 2;
	} else {
		state = 0;
	}


	if(state == 1) {

		if(LOG_LEVEL <= DEBUG) *debug_stream << "dark! (L) " << current_ldr_right_val << " < " << right_dark_thresh << endl;

		if(ldrRightDark) ldrRightDark();

	}


	if(state == 2) {

		if(LOG_LEVEL <= DEBUG) *debug_stream << "bright! (L) " << current_ldr_right_val << " > " << right_bright_thresh << endl;

		if(ldrRightBright) ldrRightBright();

	}


	return state;

}


uint16_t RoboBrrd::math_abs(uint16_t a, uint16_t b) {
  
  // for some reason the abs() function is giving us a negative number,
  // even though we are not passing a function to the macro. weird behaviour.
  // it is seemingly because it is not ordering the numbers properly.
  
  if(a > b) return abs(a-b);
  
  if(b > a) return abs(b-a);
  
}




/**
 * Emote
 */

void RoboBrrd::saveMood() {
	EEPROM.write(mood_addr[0], emote_happy);
	EEPROM.write(mood_addr[1], emote_chill);
}

void RoboBrrd::saveState() {
	EEPROM.write(state_addr[0], emote_food);
	EEPROM.write(state_addr[1], emote_water);
	EEPROM.write(state_addr[2], emote_play);
}

void RoboBrrd::setMood(uint8_t happy, uint8_t chill) {
	emote_happy = happy;
	emote_chill = chill;
}

void RoboBrrd::setState(uint8_t food, uint8_t water, uint8_t play) {
	emote_food = food;
	emote_water = water;
	emote_play = play;
}

void RoboBrrd::initEmotes() {

	if(LOG_LEVEL <= DEBUG) Serial << "Starting emotes";

	emote_happy = EEPROM.read(mood_addr[0]);
	emote_chill = EEPROM.read(mood_addr[1]);

	emote_food = EEPROM.read(state_addr[0]);
	emote_water = EEPROM.read(state_addr[1]);
	emote_play = EEPROM.read(state_addr[2]);

	setEmotePlay(emote_play+20); // here's a treat for being initialised, yum yum

	if(LOG_LEVEL <= DEBUG) Serial << "......Done" << endl;

}




/*
 * LED Eyes Functions
 */

void RoboBrrd::ledsDefault() {
	uint8_t def_red = EEPROM.read(led_addr[0]);
	uint8_t def_green = EEPROM.read(led_addr[1]);
	uint8_t def_blue = EEPROM.read(led_addr[2]);

	setEyesRGB(def_red, def_green, def_blue);
}


void RoboBrrd::saveLedsDefault() {
	EEPROM.write(led_addr[0], current_rgb[0]);
	EEPROM.write(led_addr[1], current_rgb[1]);
	EEPROM.write(led_addr[2], current_rgb[2]);
}


void RoboBrrd::setEyesRGB(uint8_t r, uint8_t g, uint8_t b) {
  
	double hsv[3];

	rgb2hsv(r, g, b, hsv);

  current_rgb[0] = r;
  current_rgb[1] = g;
  current_rgb[2] = b;
  current_hsi[0] = (float)hsv[0];
  current_hsi[1] = (float)hsv[1];
  current_hsi[2] = (float)hsv[2];
  
  analogWrite(led_pins[0], r * MAX_BRIGHTNESS);
  analogWrite(led_pins[1], g * MAX_BRIGHTNESS);
  analogWrite(led_pins[2], b * MAX_BRIGHTNESS);
  
}


void RoboBrrd::setEyesHSI(float h, float s, float i) {
  
  int rgb[3];
  
  hsi2rgb(h, s, i, rgb);
  
  
  uint8_t new_r = (uint8_t)(rgb[0] * MAX_BRIGHTNESS); 
  uint8_t new_g = (uint8_t)(rgb[1] * MAX_BRIGHTNESS);
  uint8_t new_b = (uint8_t)(rgb[2] * MAX_BRIGHTNESS);
  
  current_rgb[0] = new_r;
  current_rgb[1] = new_g;
  current_rgb[2] = new_b;
  current_hsi[0] = h;
  current_hsi[1] = s;
  current_hsi[2] = i;
  
  analogWrite(led_pins[0], new_r);
  analogWrite(led_pins[1], new_g);
  analogWrite(led_pins[2], new_b);
  
}


// This function is by Brian Neltner from Saikoled
// http://blog.saikoled.com/post/43693602826/why-every-led-light-should-be-using-hsi-colorspace
void RoboBrrd::hsi2rgb(float H, float S, float I, int *rgb) {
  
  int r, g, b;
  H = fmod(H,360); // cycle H around to 0-360 degrees
  H = 3.14159*H/(float)180; // Convert to radians.
  S = S>0?(S<1?S:1):0; // clamp S and I to interval [0,1]
  I = I>0?(I<1?I:1):0;
    
  // Math! Thanks in part to Kyle Miller.
  if(H < 2.09439) {
    r = 255*I/3*(1+S*cos(H)/cos(1.047196667-H));
    g = 255*I/3*(1+S*(1-cos(H)/cos(1.047196667-H)));
    b = 255*I/3*(1-S);
  } else if(H < 4.188787) {
    H = H - 2.09439;
    g = 255*I/3*(1+S*cos(H)/cos(1.047196667-H));
    b = 255*I/3*(1+S*(1-cos(H)/cos(1.047196667-H)));
    r = 255*I/3*(1-S);
  } else {
    H = H - 4.188787;
    b = 255*I/3*(1+S*cos(H)/cos(1.047196667-H));
    r = 255*I/3*(1+S*(1-cos(H)/cos(1.047196667-H)));
    g = 255*I/3*(1-S);
  }
  
  rgb[0]=r;
  rgb[1]=g;
  rgb[2]=b;
  
}


// This function is by Robert Atkins
// https://github.com/ratkins/RGBConverter
void RoboBrrd::rgb2hsv(uint8_t r, uint8_t g, uint8_t b, double *hsv) {
    double rd = (double) r/255;
    double gd = (double) g/255;
    double bd = (double) b/255;
    double max = threeway_max(rd, gd, bd), min = threeway_min(rd, gd, bd);
    double h, s, v = max;

    double d = max - min;
    s = max == 0 ? 0 : d / max;

    if (max == min) { 
        h = 0; // achromatic
    } else {
        if (max == rd) {
            h = (gd - bd) / d + (gd < bd ? 6 : 0);
        } else if (max == gd) {
            h = (bd - rd) / d + 2;
        } else if (max == bd) {
            h = (rd - gd) / d + 4;
        }
        h /= 6;
    }

    hsv[0] = h;
    hsv[1] = s;
    hsv[2] = v;
}


double RoboBrrd::threeway_max(double a, double b, double c) {
    return max(a, max(b, c));
}


double RoboBrrd::threeway_min(double a, double b, double c) {
    return min(a, min(b, c));
}




/**
 * Speaker
 */

void RoboBrrd::robotgrrlSong() {
  for(uint8_t i=0; i<5; i++) {
    playTone(260, 70);
    playTone(280, 70);
    playTone(300, 70);
    delay(100);
  } 
}


void RoboBrrd::playTone(uint16_t tone, uint16_t duration) {
	
  for (long i = 0; i < duration * 1000L; i += tone * 2) {
    digitalWrite(spkr_pin, HIGH);
    delayMicroseconds(tone);
    digitalWrite(spkr_pin, LOW);
    delayMicroseconds(tone);
  }
	
}



/**
 * Movements
 */


// -- beak

void RoboBrrd::beakOpen() {
	servoMove(BEAK_SERVO, beak_pos[1], 100);
}

void RoboBrrd::beakClose() {
	servoMove(BEAK_SERVO, beak_pos[2], 100);
}

void RoboBrrd::beakHome() {
	servoMove(BEAK_SERVO, beak_pos[0], 100);
}

void RoboBrrd::beakSnip() {

	for(uint8_t i=0; i<2; i++) {
		servoMove(BEAK_SERVO, beak_pos[1], 100);
		servoMove(BEAK_SERVO, beak_pos[2], 100);
	}

}

void RoboBrrd::beakLaugh() {

	for(uint8_t i=0; i<3; i++) {
		servoMove(BEAK_SERVO, beak_pos[1], 100);
		servoMove(BEAK_SERVO, beak_pos[2], 50);
	}
	
}

void RoboBrrd::beakPos(uint8_t pos) {
	servoMove(BEAK_SERVO, pos, 100);
}



// -- rotation

void RoboBrrd::rotateLeft() {
	servoMove(ROTATION_SERVO, rot_pos[1], 200);
}

void RoboBrrd::rotateRight() {
	servoMove(ROTATION_SERVO, rot_pos[2], 200);
}

void RoboBrrd::rotateHome() {
	servoMove(ROTATION_SERVO, rot_pos[0], 150);
}

void RoboBrrd::shakeShake() {

	for(uint8_t i=0; i<4; i++) {
		servoMove(ROTATION_SERVO, rot_pos[1], 300);
		servoMove(ROTATION_SERVO, rot_pos[2], 300);
	}

	servoMove(ROTATION_SERVO, rot_pos[0], 150);

}

void RoboBrrd::rotateBounce() {

	uint8_t boing1 = 0;
	uint8_t boing2 = 0;

	if(rot_pos[1] < rot_pos[0]) { // 0 < 90
		boing1 = rot_pos[1] + 20;
	} else {
		boing1 = rot_pos[1] - 20;
	}

	if(rot_pos[2] > rot_pos[0]) { // 180 > 90
		boing2 = rot_pos[2] - 20;
	} else {
		boing2 = rot_pos[2] + 20;
	}

	servoMove(ROTATION_SERVO, rot_pos[1], 300);

	for(uint8_t i=0; i<3; i++) {
		servoMove(ROTATION_SERVO, boing1, 50);
		servoMove(ROTATION_SERVO, rot_pos[1], 50);
	}

	servoMove(ROTATION_SERVO, rot_pos[2], 300);

	for(uint8_t i=0; i<3; i++) {
		servoMove(ROTATION_SERVO, boing2, 50);
		servoMove(ROTATION_SERVO, rot_pos[2], 50);
	}

	servoMove(ROTATION_SERVO, rot_pos[0], 150);

}

void RoboBrrd::rotatePos(uint8_t pos) {
	servoMove(ROTATION_SERVO, pos, 100);	
}



// -- right wing

void RoboBrrd::rightWingUp() {
	servoMove(RWING_SERVO, rwing_pos[1], 100);
}

void RoboBrrd::rightWingDown() {
	servoMove(RWING_SERVO, rwing_pos[2], 100);
}

void RoboBrrd::rightWingHome() {
	servoMove(RWING_SERVO, rwing_pos[0], 100);
}

void RoboBrrd::rightWingWave() {

	for(uint8_t i=0; i<3; i++) {
		servoMove(RWING_SERVO, rwing_pos[1], 150);
		servoMove(RWING_SERVO, rwing_pos[2], 150);
	}

	servoMove(RWING_SERVO, rwing_pos[0], 80);

}

void RoboBrrd::rightWingGust() {

	uint8_t gust = 0;

	if(rwing_pos[2] < rwing_pos[0]) {
		gust = rwing_pos[2] + 20;
	} else {
		gust = rwing_pos[2] - 20;
	}

	for(uint8_t i=0; i<3; i++) {
		servoMove(RWING_SERVO, rwing_pos[2], 50);
		servoMove(RWING_SERVO, gust, 50);
	}

	servoMove(RWING_SERVO, rwing_pos[0], 80);

}

void RoboBrrd::rightWingPos(uint8_t pos) {
	servoMove(RWING_SERVO, pos, 80);
}



// -- left wing

void RoboBrrd::leftWingUp() {
	servoMove(LWING_SERVO, lwing_pos[1], 50);
}

void RoboBrrd::leftWingDown() {
	servoMove(LWING_SERVO, lwing_pos[2], 50);
}

void RoboBrrd::leftWingHome() {
	servoMove(LWING_SERVO, lwing_pos[0], 50);
}

void RoboBrrd::leftWingWave() {

	for(uint8_t i=0; i<3; i++) {
		servoMove(LWING_SERVO, lwing_pos[1], 150);
		servoMove(LWING_SERVO, lwing_pos[2], 150);
	}

	servoMove(LWING_SERVO, lwing_pos[0], 80);

}

void RoboBrrd::leftWingGust() {

	uint8_t gust = 0;

	if(lwing_pos[2] < lwing_pos[0]) {
		gust = lwing_pos[2] + 20;
	} else {
		gust = lwing_pos[2] - 20;
	}

	for(uint8_t i=0; i<3; i++) {
		servoMove(LWING_SERVO, lwing_pos[2], 50);
		servoMove(LWING_SERVO, gust, 50);
	}

	servoMove(LWING_SERVO, lwing_pos[0], 80);

}

void RoboBrrd::leftWingPos(uint8_t pos) {
	servoMove(LWING_SERVO, pos, 80);
}



// -- wings

void RoboBrrd::bothWingsUp(bool up) {

	if(auto_detach) {
		servoAttach(RWING_SERVO);
		servoAttach(LWING_SERVO);
	}

	if(up) {

		servo[RWING_SERVO].write(rwing_pos[1]);
		servo[LWING_SERVO].write(lwing_pos[1]);
		delay(150);

		last_servo_pos[RWING_SERVO] = rwing_pos[1];
		last_servo_pos[LWING_SERVO] = lwing_pos[1];

	} else {

		servo[RWING_SERVO].write(rwing_pos[2]);
		servo[LWING_SERVO].write(lwing_pos[2]);
		delay(150);

		last_servo_pos[RWING_SERVO] = rwing_pos[2];
		last_servo_pos[LWING_SERVO] = lwing_pos[2];

	}

	if(auto_detach) {
		servoDetach(RWING_SERVO);
		servoDetach(LWING_SERVO);
	}

	last_servo_move[RWING_SERVO] = millis();
	last_servo_move[LWING_SERVO] = millis();

	for(uint8_t i=0; i<5; i++) {
		last_servos_moved[i+1] = last_servos_moved[i];
	}

	last_servos_moved[0] = RWING_SERVO;

	for(uint8_t i=0; i<5; i++) {
		last_servos_moved[i+1] = last_servos_moved[i];
	}

	last_servos_moved[0] = LWING_SERVO;

}


void RoboBrrd::bothWingWave(bool opposite) {

	if(auto_detach) {
		servoAttach(RWING_SERVO);
		servoAttach(LWING_SERVO);
	}

	if(!opposite) {

		for(uint8_t i=0; i<4; i++) {
			servo[RWING_SERVO].write(rwing_pos[1]);
			servo[LWING_SERVO].write(lwing_pos[1]);
			delay(150);
			servo[RWING_SERVO].write(rwing_pos[2]);
			servo[LWING_SERVO].write(lwing_pos[2]);
			delay(150);	
		}

	} else {

		for(uint8_t i=0; i<4; i++) {
			servo[RWING_SERVO].write(rwing_pos[1]);
			servo[LWING_SERVO].write(lwing_pos[2]);
			delay(150);
			servo[RWING_SERVO].write(rwing_pos[2]);
			servo[LWING_SERVO].write(lwing_pos[1]);
			delay(150);	
		}

	}

	servo[RWING_SERVO].write(rwing_pos[0]);
	servo[LWING_SERVO].write(lwing_pos[0]);
	delay(80);

	if(auto_detach) {
		servoDetach(RWING_SERVO);
		servoDetach(LWING_SERVO);
	}

	last_servo_move[RWING_SERVO] = millis();
	last_servo_move[LWING_SERVO] = millis();

	last_servo_pos[RWING_SERVO] = rwing_pos[0];
	last_servo_pos[LWING_SERVO] = lwing_pos[0];

	for(uint8_t i=0; i<5; i++) {
		last_servos_moved[i+1] = last_servos_moved[i];
	}

	last_servos_moved[0] = RWING_SERVO;

	for(uint8_t i=0; i<5; i++) {
		last_servos_moved[i+1] = last_servos_moved[i];
	}

	last_servos_moved[0] = LWING_SERVO;

}


void RoboBrrd::bothWingGust(bool opposite) {

	if(auto_detach) {
		servoAttach(RWING_SERVO);
		servoAttach(LWING_SERVO);
	}

	uint8_t gust_l = 0;
	uint8_t gust_r = 0;

	if(lwing_pos[2] < lwing_pos[0]) {
		gust_l = lwing_pos[2] + 20;
	} else {
		gust_l = lwing_pos[2] - 20;
	}

	if(rwing_pos[2] < rwing_pos[0]) {
		gust_r = rwing_pos[2] + 20;
	} else {
		gust_r = rwing_pos[2] - 20;
	}		

	if(!opposite) {

		for(uint8_t i=0; i<3; i++) {
			servo[LWING_SERVO].write(lwing_pos[2]);
			servo[RWING_SERVO].write(rwing_pos[2]);
			delay(50);

			servo[LWING_SERVO].write(gust_l);
			servo[RWING_SERVO].write(gust_r);
			delay(50);
		}

	} else {

		for(uint8_t i=0; i<3; i++) {
			servo[LWING_SERVO].write(lwing_pos[2]);
			servo[RWING_SERVO].write(gust_r);
			delay(50);

			servo[LWING_SERVO].write(gust_l);
			servo[RWING_SERVO].write(rwing_pos[2]);
			delay(50);
		}

	}

	servo[RWING_SERVO].write(rwing_pos[0]);
	servo[LWING_SERVO].write(lwing_pos[0]);
	delay(80);

	if(auto_detach) {
		servoDetach(RWING_SERVO);
		servoDetach(LWING_SERVO);
	}

	last_servo_move[RWING_SERVO] = millis();
	last_servo_move[LWING_SERVO] = millis();

	last_servo_pos[RWING_SERVO] = rwing_pos[0];
	last_servo_pos[LWING_SERVO] = lwing_pos[0];

	for(uint8_t i=0; i<5; i++) {
		last_servos_moved[i+1] = last_servos_moved[i];
	}

	last_servos_moved[0] = RWING_SERVO;

	for(uint8_t i=0; i<5; i++) {
		last_servos_moved[i+1] = last_servos_moved[i];
	}

	last_servos_moved[0] = LWING_SERVO;

}




/**
 * Misc
 */

bool RoboBrrd::isMemInit() {
	bool mem = EEPROM.read(init_addr);
	return mem;
}

uint8_t RoboBrrd::check8Bit(uint16_t v) {
	if(v > 256) {
		uint8_t temp = 0;
		temp = (uint8_t)(v/256);
		return temp;
	}
	return (uint8_t)v;
}

uint8_t RoboBrrd::easterEgg() {
	uint8_t r = (uint8_t)random(3, 256);
	Serial << "RoboBrrd has 2^" << r << " robot friends! Including YOU! :)" << endl;
	return r; 
}

// this function is by David A. Mellis
// http://playground.arduino.cc/Code/AvailableMemory
// based on code by Rob Faludi
// http://www.faludi.com
int RoboBrrd::availableMemory() {
  int size = 1024; // Use 2048 with ATmega328
  byte *buf;

  while ((buf = (byte *) malloc(--size)) == NULL)
    ;

  free(buf);

  return size;
}


// this is a useful helper function that is like a coin toss,
// it either gives a true or false. in the code, it is used to help
// 'enhance' behaviours, by keeping it fresh and not repeating the
// same exact thing each time.
bool RoboBrrd::headsOrTails() {

  if((int)random(0, 2) == 0) {
    return true;
  }
  
  return false; 
}



/**
 * Promulgate
 */

void RoboBrrd::initPromulgateHw(Stream *in, Stream *out) {
	if(LOG_LEVEL <= DEBUG) Serial << "Starting API mode (HW)";
	promulgate_hw = Promulgate(in, out);
	promulgate_hw.LOG_LEVEL = Promulgate::ERROR_;
	api_mode_hw = true;
	if(LOG_LEVEL <= DEBUG) Serial << "......Done" << endl;
}


void RoboBrrd::initPromulgateSw(Stream *in, Stream *out) {
	if(LOG_LEVEL <= DEBUG) Serial << "Starting API mode (SW)";
	promulgate_sw = Promulgate(in, out);
	promulgate_sw.LOG_LEVEL = Promulgate::ERROR_;
	api_mode_sw = true;
	if(LOG_LEVEL <= DEBUG) Serial << "......Done" << endl;
}


void RoboBrrd::parse_action(uint8_t stream, char action, char cmd, uint8_t key, uint16_t val, char delim) {

  // action specifier list
	// @ - servos, leds, piezo, sensors, emotes
	// # - movements
	// ^ - eeprom
	// & - reserved for specific apps


	if(LOG_LEVEL <= DEBUG) {
    Serial << "\n---CALLBACK---" << endl;
    Serial << "action: " << action << endl;
    Serial << "command: " << cmd << endl;
    Serial << "key: " << key << endl;
    Serial << "val: " << val << endl;
    Serial << "delim: " << delim << endl;

    if(stream == 0) {
    	Serial << "hw (0)" << endl;
    } else if(stream == 1) {
    	Serial << "sw (1)" << endl;
    }

  }

	switch(action) {

		case '@': { // servos, leds, piezo, etc.

			switch(cmd) {
				
				case 'S': // rotation
					servoMove(ROTATION_SERVO, val, key*10);
				break;
				
				case 'B': // beak
					servoMove(BEAK_SERVO, val, key*10);
				break;
				
				case 'R': // right wing
					servoMove(RWING_SERVO, val, key*10);
				break;
				
				case 'L': // left wing
					servoMove(LWING_SERVO, val, key*10);
				break;
				
				case 'E': // eye (rgb led)

					if(key == 0) {
						setEyesRGB(val, current_rgb[1], current_rgb[2]);
					} else if(key == 1) {
						setEyesRGB(current_rgb[0], val, current_rgb[2]);
					} else if(key == 2) {
						setEyesRGB(current_rgb[0], current_rgb[1], val);
					}

				break;
				
				case 'F': // eye (hsi)

					if(key == 0) {
						setEyesHSI(val, current_hsi[1], current_hsi[2]);
					} else if(key == 1) {
						setEyesHSI(current_hsi[0], (float)(val)/100.0, current_hsi[2]);
					} else if(key == 2) {
						setEyesHSI(current_hsi[0], current_hsi[1], (float)(val)/100.0);
					}

				break;
				
				case 'P': // piezo / speaker
					playTone(val, key*10);
				break;

				case 'I': // left ldr
					transmit_message(stream, '#', 'I', 0, getLeftLDR(), '!');
				break;

				case 'J': // right ldr
					transmit_message(stream, '#', 'J', 0, getRightLDR(), '!');
				break;
				
				case 'V': // emote happy

					if(key == 0) { // 0 = get
						transmit_message(stream, '#', 'V', 0, getEmoteHappy(), '!');
					} else if(key == 1) { // 1 = set
						setEmoteHappy(val);
					}

				break;
				
				case 'W': // emote chill

					if(key == 0) { // 0 = get
						transmit_message(stream, '#', 'W', 0, getEmoteChill(), '!');
					} else if(key == 1) { // 1 = set
						setEmoteChill(val);
					}

				break;
				
				case 'X': // emote food

					if(key == 0) { // 0 = get
						transmit_message(stream, '#', 'X', 0, getEmoteFood(), '!');
					} else if(key == 1) { // 1 = set
						setEmoteFood(val);
					}

				break;
				
				case 'Y': // emote water

					if(key == 0) { // 0 = get
						transmit_message(stream, '#', 'Y', 0, getEmoteWater(), '!');
					} else if(key == 1) { // 1 = set
						setEmoteWater(val);
					}

				break;
				
				case 'Z': // emote play

					if(key == 0) { // 0 = get
						transmit_message(stream, '#', 'Z', 0, getEmotePlay(), '!');
					} else if(key == 1) { // 1 = set
						setEmotePlay(val);
					}

				break;
			}

		break;
		}

		case '#': { // movements

			switch(cmd) {

				case 'S': // rotation

					switch(key) {
						case 0:
							rotateLeft();
						break;
						case 1:
							rotateRight();
						break;
						case 2:
							rotateHome();
						break;
						case 3:
							shakeShake();
						break;
						case 4:
							rotateBounce();
						break;
						case 5:
							rotatePos( check8Bit(val) );
						break;
					}

				break;

				case 'B': // beak

					switch(key) {
						case 0:
							beakOpen();
						break;
						case 1:
							beakClose();
						break;
						case 2:
							beakHome();
						break;
						case 3:
							beakSnip();
						break;
						case 4:
							beakLaugh();
						break;
						case 5:
							beakPos( check8Bit(val) );
						break;
					}

				break;

				case 'R': // right wing

					switch(key) {
						case 0:
							rightWingUp();
						break;
						case 1:
							rightWingDown();
						break;
						case 2:
							rightWingHome();
						break;
						case 3:
							rightWingWave();
						break;
						case 4:
							rightWingGust();
						break;
						case 5:
							rightWingPos( check8Bit(val) );
						break;
					}

				break;

				case 'L': // left wing

					switch(key) {
						case 0:
							leftWingUp();
						break;
						case 1:
							leftWingDown();
						break;
						case 2:
							leftWingHome();
						break;
						case 3:
							leftWingWave();
						break;
						case 4:
							leftWingGust();
						break;
						case 5:
							leftWingPos( check8Bit(val) );
						break;
					}

				break;

				case 'O': // extra extra!

					switch(key) {
						case 0:
							servosDetach();
						break;
						case 1:
							if(val == 0) {
								bothWingWave(false);
							} else if(val == 1) {
								bothWingWave(true);
							}
						break;
						case 2:
							if(val == 0) {
								bothWingGust(false);
							} else if(val == 1) {
								bothWingGust(true);
							}
						break;
					}

				break;

			}

		break;
		}

		case '^': { // eeprom

			if(cmd == 'E') {

				switch(key) {

					case 0:
						setServoHome(ROTATION_SERVO, val);
					break;

					case 1:
						setServoHome(BEAK_SERVO, val);
					break;

					case 2:
						setServoHome(RWING_SERVO, val);
					break;

					case 3:
						setServoHome(LWING_SERVO, val);
					break;

					case 4:
						setServoDefaultP2(ROTATION_SERVO, val);
					break;

					case 5:
						setServoDefaultP2(BEAK_SERVO, val);
					break;

					case 6:
						setServoDefaultP2(RWING_SERVO, val);
					break;

					case 7:
						setServoDefaultP2(LWING_SERVO, val);
					break;

					case 8:
						setServoDefaultP3(ROTATION_SERVO, val);
					break;

					case 9:
						setServoDefaultP3(BEAK_SERVO, val);
					break;

					case 10:
						setServoDefaultP3(RWING_SERVO, val);
					break;

					case 11:
						setServoDefaultP3(LWING_SERVO, val);
					break;

					case 12:
						saveMood();
					break;

					case 13:
						saveState();
					break;

					case 14:
						saveLedsDefault();
					break;

				}

			}

		}
		break;

	}

}


void RoboBrrd::transmit_message(uint8_t stream, char action, char cmd, uint8_t key, uint16_t val, char delim) {

	if(stream == 0) {
		promulgate_hw.transmit_action(action, cmd, key, val, delim);
	} else if(stream == 1) {
		promulgate_sw.transmit_action(action, cmd, key, val, delim);
	}

}

