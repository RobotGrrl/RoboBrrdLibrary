void serialEvent() {
  char c = Serial.read();
  
  if(c == '>') {
    if(instruction_advance < 12) instruction_advance++;
  } else if(c == '<') {
    if(instruction_advance > 0) instruction_advance--;
  } else {
    if(robobrrd.apiModeHw()) robobrrd.promulgate_hw.organize_message(c);
    if(robobrrd.apiModeSw()) robobrrd.promulgate_sw.organize_message(c);
  }
  
}

void received_action_hw(char action, char cmd, uint8_t key, uint16_t val, char delim) {
  robobrrd.parse_action(0, action, cmd, key, val, delim);
}

void received_action_sw(char action, char cmd, uint8_t key, uint16_t val, char delim) {
  robobrrd.parse_action(0, action, cmd, key, val, delim);
}

void transmit_complete() {
  if(robobrrd.LOG_LEVEL <= RoboBrrd::DEBUG) {
    Serial << "transmit complete!" << endl;
  }
}

void addPromulgateCallbacks() {
  robobrrd.promulgate_hw.set_rx_callback(received_action_hw);
  robobrrd.promulgate_hw.set_tx_callback(transmit_complete);
  robobrrd.promulgate_sw.set_rx_callback(received_action_sw);
  robobrrd.promulgate_sw.set_tx_callback(transmit_complete); 
}

