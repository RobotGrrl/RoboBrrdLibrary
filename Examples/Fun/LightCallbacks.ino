// always make sure you have these callbacks, otherwise the program
// will crash when the library tries to call these functions and
// there is nothing there! yikes!

void ldr_left_dark() {
  Serial << "Left eye dark" << endl;
  robobrrd.setEyesHSI(robobrrd.hue_orange, 1.0, 1.0);
  last_light_sensor = millis();
}

void ldr_left_bright() {
  Serial << "Left eye bright" << endl;
  robobrrd.setEyesHSI(robobrrd.hue_yellow, 1.0, 1.0);
  last_light_sensor = millis();
}

void ldr_right_dark() {
  Serial << "Right eye dark" << endl;
  robobrrd.setEyesHSI(robobrrd.hue_blue, 1.0, 1.0);
  last_light_sensor = millis();
}

void ldr_right_bright() {
  Serial << "Right eye bright" << endl;
  robobrrd.setEyesHSI(robobrrd.hue_violet, 1.0, 1.0);
  last_light_sensor = millis();
}

void addLightCallbacks() {
  robobrrd.setLdrLeftDarkHandler(ldr_left_dark);
  robobrrd.setLdrLeftBrightHandler(ldr_left_bright);
  robobrrd.setLdrRightDarkHandler(ldr_right_dark);
  robobrrd.setLdrRightBrightHandler(ldr_right_bright); 
}
