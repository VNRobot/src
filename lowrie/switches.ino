/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Robot sensor switches
*/

// pin numbers for switch
enum swPins {
  FL_SWITCH = 2,
  FR_SWITCH = 3
};
// switches state
enum swState {
  SW_NORM,
  SW_FL_BUMP,
  SW_FR_BUMP,
  SW_F_BUMP
};

// switches data
pair swStateNow = {1, 1};
// switches value
pair swStateValue = {1, 1};
// switch state
unsigned char swsState = SW_NORM;

// init Switches
void initSwitches(bool calibrationMode) {
  Serial.println(F("initSwitches"));
  pinMode(FL_SWITCH, INPUT_PULLUP);
  pinMode(FR_SWITCH, INPUT_PULLUP);
  unsigned char counter = 0;
  while (calibrationMode) {
    counter ++;
    if (counter >= 32) {
      counter = 0;
    }
    delay(20);
    readSwitchesCount(counter);
    if (m_getButtonPressed()) {
      calibrationMode = false;
    }
    if (counter == 0) {
      _printSwitchesState(getSwitchesState());
    }
  }
}

// read switches
void readSwitchesCount(unsigned char counter) {
  swStateNow.left = digitalRead(FL_SWITCH);
  swStateNow.right = digitalRead(FR_SWITCH);
  //
  if (counter == 0) {
    swStateValue.left = 1;
    swStateValue.right = 1;
  }
  if (swStateNow.left == 0) {
    swStateValue.left = 0;
  }
  if (swStateNow.right == 0) {
    swStateValue.right = 0;
  }
}

// get switches state
unsigned char getSwitchesState(void) {
  // set state
  if ((swStateValue.left == 0) && (swStateValue.right == 0)) {
    // fl fr bump
    swsState = SW_F_BUMP;
  } else if (swStateValue.left == 0) {
    // fl bump
    swsState = SW_FL_BUMP;
  } else if (swStateValue.right == 0) {
    // fr bump
    swsState = SW_FR_BUMP;
  } else {
    swsState = SW_NORM;
  }
  return swsState;
}

// get switch
pair getSwitch(void) {
  return swStateNow;
}

// print switches state
void _printSwitchesState(unsigned char swState) {
  switch (swState) {
    case SW_F_BUMP:
      Serial.println(F(" SW_F_BUMP "));
    break;
    case SW_FL_BUMP:
      Serial.println(F(" SW_FL_BUMP "));
    break;
    case SW_FR_BUMP:
      Serial.println(F(" SW_FR_BUMP "));
    break;
    case SW_NORM:
      Serial.println(F(" SW_NORM "));
    break;
    default:
      Serial.println(F(" Wrong switch state "));
  }
}
