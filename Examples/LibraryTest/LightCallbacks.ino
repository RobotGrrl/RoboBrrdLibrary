// always make sure you have these callbacks, otherwise the program
// will crash when the library tries to call these functions and
// there is nothing there! yikes!

void ldr_left_dark() {
  Serial << "Left is dark!" << endl; 
}

void ldr_left_bright() {
  Serial << "Left is bright!" << endl;
}

void ldr_right_dark() {
 Serial << "Right is dark!" << endl; 
}

void ldr_right_bright() {
 Serial << "Right is bright!" << endl;
}

void addLightCallbacks() {
  robobrrd.setLdrLeftDarkHandler(ldr_left_dark);
  robobrrd.setLdrLeftBrightHandler(ldr_left_bright);
  robobrrd.setLdrRightDarkHandler(ldr_right_dark);
  robobrrd.setLdrRightBrightHandler(ldr_right_bright); 
}
