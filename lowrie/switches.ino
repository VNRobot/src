/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Robot legs switches
*/

#define SW_MARGINE_CYCLE     1

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
  SW_FRONT_HEAVY,
  SW_REAR_HEAVY
};
// leg switch 
struct switches {
  short fl;
  short fr;
  short rl;
  short rr;
};

// switches data
switches swStateNow = {1, 1, 1, 1};
// switches data
switches swSum = {0, 0, 0, 0};
switches switchCount = {0, 0, 0, 0};
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
    if (counter >= SERVO_FULL_CYCLE) {
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
    switchCount.fl = swSum.fl;
    swSum.fl = 0;
    switchCount.fr = swSum.fr;
    swSum.fr = 0;
    switchCount.rl = swSum.rl;
    swSum.rl = 0;
    switchCount.rr = swSum.rr;
    swSum.rr = 0;
  }
  if (m_legsValue.fl.lifted) {
    swSum.fl += swStateNow.fl;
  }
  if (m_legsValue.fr.lifted) {
    swSum.fr += swStateNow.fr;
  }
  if (m_legsValue.rl.lifted) {
    swSum.rl += swStateNow.rl;
  }
  if (m_legsValue.rr.lifted) {
    swSum.rr += swStateNow.rr;
  }
}

// get switches state
unsigned char getSwitchesState(void) {
  // set state
  if ((switchCount.fl + SW_MARGINE_CYCLE) < ((switchCount.fr + switchCount.rl + switchCount.rr) / 3)) {
    // fl bump
    swsState = SW_FL_BUMP;
  } else if ((switchCount.fr + SW_MARGINE_CYCLE) < ((switchCount.fl + switchCount.rl + switchCount.rr) / 3)) {
    // fr bump
    swsState = SW_FR_BUMP;
  } else if ((switchCount.fl + switchCount.fr) > (switchCount.rl + switchCount.rr + SW_MARGINE_CYCLE)) {
    // rear too heavy
    swsState = SW_REAR_HEAVY;
  } else if ((switchCount.fl + switchCount.fr + SW_MARGINE_CYCLE) < (switchCount.rl + switchCount.rr)) {
    // front too heavy
    swsState = SW_FRONT_HEAVY;
  } else {
    swsState = SW_NORM;
  }
  _printSwitchesState(swsState);
  return swsState;
}

// print switches state
void _printSwitchesState(unsigned char swState) {
  switch (swState) {
    case SW_FL_BUMP:
      Serial.print(F(" SW_FL_BUMP "));
    break;
    case SW_FR_BUMP:
      Serial.print(F(" SW_FR_BUMP "));
    break;
    case SW_FRONT_HEAVY:
      Serial.print(F(" SW_FRONT_HEAVY "));
    break;
    case SW_REAR_HEAVY:
      Serial.print(F(" SW_REAR_HEAVY "));
    break;
    case SW_NORM:
      Serial.print(F(" SW_NORM "));
    break;
    default:
      Serial.print(F(" Wrong switch state "));
  }
    Serial.print(F(" Count "));
    Serial.print((int)switchCount.fl);
    Serial.print(F(" "));
    Serial.print((int)switchCount.fr);
    Serial.print(F(" "));
    Serial.print((int)switchCount.rl);
    Serial.print(F(" "));
    Serial.println((int)switchCount.rr);
}
