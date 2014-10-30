/*
 * Serial API
 * -----------
 * 
 * We use another one of our libraries, 'Promulgate', to help
 * us with an API for controlling RoboBrrd via serial. There is
 * both a hardware serial and software serial version, and they
 * look for the same commands.
 *
 * This means you can control RoboBrrd via the Serial Monitor,
 * and also if you connect an Xbee or BLE modem. We will have
 * some examples on how to make this work on our website, so
 * be sure to check out the learn section on RoboBrrd.com in
 * the near future.
 *
 * Here is an example of what you would type in Serial Monitor
 * if you wanted RoboBrrd to wave both its wings (and then press
 * enter to send it of course)

   #O1,0!

 * Here is the entire list of API commands!

 * If you have any questions, please ask them on the forums:
 * --> http://robobrrd.com/forum




 * Controlling servos, leds, piezos, etc:
 * ---------------------------------------

 * Rotation servo move (where key is 1/10th the required delay
 * time in ms and val is the position of the servo)
   @S<key>,<val>!

 * Beak servo move (see above for parameters)
   @B<key>,<val>!

 * Right wing move (see above for parameters)
   @R<key>,<val>!

 * Left wing move (see above for parameters)
   @L<key>,<val>!

 * Eye LEDs RGB (where key 0 = red, 1 = green, 2 = blue and
 * val is from 0 (dark) to 255 (bright))
   @E<key>,<val>!

 * Eye LEDs HSI (where key 0 = hue, 1 = saturation, 2 = 
 * intensity and val for hue is from 0-360, and val for sat
 * and intensity is 0 (less) to 100 (more))
   @F<key>,<val>!

 * Play tone on speaker (where key is 1/10th the play duration
 * and val is the frequency to play)
   @P<key>,<val>!

 * Get Left LDR reading (where key and val are anything)
   @I<key>,<val>!

 * --> Response will be in the format of this (where val is
 * the sensor reading)
   #I0,<val>!

 * Get Right LDR reading (where key and val are anything)
   @J<key>,<val>!

 * --> Response will be in the format of this (where val is
 * the sensor reading)
   #J0,<val>!

 * Get happy emote (where val is anything)
   @V0,<val>!

 * --> Response will be in the format of this (where val is
 * the happy emote reading)
   #V0,<val>!

 * Set happy emote (where val is from 0 (very sad) to 100
 * (very happy))
   @V1,<val>!

 * Get chill emote (where val is anything)
 * @W0,<val>!

 * --> Response will be in the format of this (where val is
 * the chill emote reading)
   #W0,<val>!

 * Set chill emote (where val is from 0 (very chill) to 100
 * (very hyper))
   @W1,<val>!

 * Get food emote (where val is anything)
   @X0,<val>!

 * --> Response will be in the format of this (where val is
 * the food emote reading)
   #X0,<val>!

 * Set food emote (where val is from 0 (starving) to 100
 * (bloated like a beluga))
   @X1,<val>!

 * Get water emote (where val is anything)
   @Y0,<val>!

 * --> Response will be in the format of this (where val is
 * the water emote reading)
   #Y0,<val>!

 * Set water emote (where val is from 0 (parched) to 100
 * (like an ocean))
   @Y1,<val>!

 * Get play emote (where val is anything)
   @Z0,<val>!

 * --> Response will be in the format of this (where val is
 * the play emote reading)
   #Z0,<val>!

 * Set play emote (where val is from 0 (bored) to 100
 * (hypertastic))
   @Z1,<val>!




 * Movements
 * ---------------------------------------

 * Rotate left
   #S0,0!

 * Rotate right
   #S1,0!

 * Rotate home
   #S2,0!

 * Rotate shake shake
   #S3,0!

 * Rotate bounce
   #S4,0!

 * Rotate position (where val is the position 0-255)
   #S5,<val>!

 * Beak open
   #B0,0!  

 * Beak close
   #B1,0!

 * Beak home
   #B2,0!

 * Beak snip
   #B3,0!

 * Beak laugh
   #B4,0!

 * Beak pos (where val is the position 0-255)
   #B5,<val>!

 * Right wing up
   #R0,0!

 * Right wing down
   #R1,0!

 * Right wing home
   #R2,0!

 * Right wing wave
   #R3,0!

 * Right wing gust
   #R4,0!

 * Right wing pos (where val is the position 0-255)
   #R5,0!

 * Left wing up
   #L0,0!

 * Left wing down
   #L1,0!

 * Left wing home
   #L2,0!

 * Left wing wave
   #L3,0!

 * Left wing gust
   #L4,0!

 * Left wing pos (where val is the position 0-255)
   #L5,0!

 * Servos detach
   #O0,0!

 * Both wings wave (where val is 0 for both wings in the same
 * direction, or 1 for both wings in opposite directions)
   #O1,<val>!

 * Both wings gust (where val is 0 for both wings in the same
 * direction, or 1 for both wings in opposite directions)
   #O2,<val>!




 * EEPROM
 * ---------------------------------------

 * Set rotational servo home (where val is the pos)
   ^E0,<val>!

 * Set beak servo home (where val is the pos)
   ^E1,<val>!

 * Set right wing servo home (where val is the pos)
   ^E2,<val>!

 * Set left wing servo home (where val is the pos)
   ^E3,<val>!

 * Set rotational servo left (where val is the pos)
   ^E4,<val>!

 * Set beak servo open (where val is the pos)
   ^E5,<val>!

 * Set right wing servo up (where val is the pos)
   ^E6,<val>!

 * Set left wing servo up (where val is the pos)
   ^E7,<val>!

 * Set rotational servo right (where val is the pos)
   ^E8,<val>!

 * Set beak servo closed (where val is the pos)
   ^E9,<val>!

 * Set right wing servo down (where val is the pos)
   ^E10,<val>!

 * Set left wing servo down (where val is the pos)
   ^E11,<val>!

 * Save the emote moods (where val is anything)
   ^E12,<val>! 

 * Save the emote states (where val is anything)
   ^E13,<val>!

 * Set current LED colours as default (where val is anything)
   ^E14,<val>!

*/