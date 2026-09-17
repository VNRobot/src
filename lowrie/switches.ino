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
quad swStateNow = {1, 1, 1, 1, false, false};

// init Switches
void initSwitches(bool calibrationMode, bool swFrontEnable, bool swRearEnable) {
  swStateNow.enabledF = swFrontEnable;
  swStateNow.enabledR = swRearEnable;
  Serial.println(F("initSwitches"));
  if (swStateNow.enabledF) {
    pinMode(FL_SWITCH, INPUT_PULLUP);
    pinMode(FR_SWITCH, INPUT_PULLUP);
  }
  if (swStateNow.enabledR) {
    pinMode(RL_SWITCH, INPUT_PULLUP);
    pinMode(RR_SWITCH, INPUT_PULLUP);
  }
  if (swStateNow.enabledF || swStateNow.enabledR) {
    while (calibrationMode) {
      delay(200);
      readSwitchesCount();
      if (m_getButtonPressed()) {
        calibrationMode = false;
      }
      _printSwitchesState();
    }
  }
}

// read switches
quad readSwitchesCount(void) {
  if (swStateNow.enabledF) {
    swStateNow.fl = digitalRead(FL_SWITCH);
    swStateNow.fr = digitalRead(FR_SWITCH);
  }
  if (swStateNow.enabledR) {
    swStateNow.rl = digitalRead(RL_SWITCH);
    swStateNow.rr = digitalRead(RR_SWITCH);
  }
  return swStateNow;
}

// print switches state
void _printSwitchesState(void) {
  Serial.print(F(" Switches fl "));
  Serial.print((int)swStateNow.fl);
  Serial.print(F(" fr "));
  Serial.print((int)swStateNow.fr);
  Serial.print(F(" rl "));
  Serial.print((int)swStateNow.rl);
  Serial.print(F(" rr "));
  Serial.println((int)swStateNow.rr);
}
