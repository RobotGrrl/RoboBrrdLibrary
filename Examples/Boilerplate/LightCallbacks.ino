// always make sure you have these callbacks, otherwise the program
// will crash when the library tries to call these functions and
// there is nothing there! yikes!

void ldr_left_dark() {
}

void ldr_left_bright() {
}

void ldr_right_dark() {
}

void ldr_right_bright() {
}

void addLightCallbacks() {
  robobrrd.setLdrLeftDarkHandler(ldr_left_dark);
  robobrrd.setLdrLeftBrightHandler(ldr_left_bright);
  robobrrd.setLdrRightDarkHandler(ldr_right_dark);
  robobrrd.setLdrRightBrightHandler(ldr_right_bright); 
}
