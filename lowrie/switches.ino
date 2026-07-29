/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Robot sensor switches
*/

// pin numbers for switch
enum swPins {
  FL_SWITCH = 2,
  FR_SWITCH = 3,
  RL_SWITCH = 4,
  RR_SWITCH = 5
};
// switches state
enum swState {
  SW_NORM,
  SW_FL_BUMP,
  SW_FR_BUMP,
  SW_RL_BUMP,
  SW_RR_BUMP,
  SW_F_BUMP
};

// switches data
quad swStateNow = {1, 1, 1, 1};
// switches value
quad swStateValue = {1, 1, 1, 1};
// switch state
unsigned char swsState = SW_NORM;

// init Switches
void initSwitches(bool calibrationMode) {
  Serial.println(F("initSwitches"));
  pinMode(FL_SWITCH, INPUT_PULLUP);
  pinMode(FR_SWITCH, INPUT_PULLUP);
  pinMode(RL_SWITCH, INPUT_PULLUP);
  pinMode(RR_SWITCH, INPUT_PULLUP);
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
  swStateNow.fl = digitalRead(FL_SWITCH);
  swStateNow.fr = digitalRead(FR_SWITCH);
  swStateNow.rl = digitalRead(RL_SWITCH);
  swStateNow.rr = digitalRead(RR_SWITCH);
  //
  if (counter == 0) {
    swStateValue.fl = 1;
    swStateValue.fr = 1;
    swStateValue.rl = 1;
    swStateValue.rr = 1;
  }
  if (swStateNow.fl == 0) {
    swStateValue.fl = 0;
  }
  if (swStateNow.fr == 0) {
    swStateValue.fr = 0;
  }
  if (swStateNow.rl == 0) {
    swStateValue.rl = 0;
  }
  if (swStateNow.rr == 0) {
    swStateValue.rr = 0;
  }
}

// get switches state
unsigned char getSwitchesState(void) {
  // set state
  if ((swStateValue.fl == 0) && (swStateValue.fr == 0)) {
    // fl fr bump
    swsState = SW_F_BUMP;
  } else if (swStateValue.fl == 0) {
    // fl bump
    swsState = SW_FL_BUMP;
  } else if (swStateValue.fr == 0) {
    // fr bump
    swsState = SW_FR_BUMP;
  } else if (swStateValue.rl == 0) {
    // rl bump
    swsState = SW_RL_BUMP;
  } else if (swStateValue.rr == 0) {
    // rr bump
    swsState = SW_RR_BUMP;
  } else {
    swsState = SW_NORM;
  }
  return swsState;
}

// get switch
quad getSwitches(void) {
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
    case SW_RL_BUMP:
      Serial.println(F(" SW_RL_BUMP "));
    break;
    case SW_RR_BUMP:
      Serial.println(F(" SW_RR_BUMP "));
    break;
    case SW_NORM:
      Serial.println(F(" SW_NORM "));
    break;
    default:
      Serial.println(F(" Wrong switch state "));
  }
}
