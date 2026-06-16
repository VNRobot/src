/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Robot static and dynamic ballance
*/

// ballance correction max value
#define STATIC_BALLANCE_MAX     60
#define DYNAMIC_BALLANCE_MAX    20
#define SWITCH_BALLANCE_MAX     60

// pin numbers for switch
enum swPins {
  FL_SWITCH = 2,
  FR_SWITCH = 3,
  RL_SWITCH = 4,
  RR_SWITCH = 5
};

// leg switch 
struct switches {
  unsigned char fl;
  unsigned char fr;
  unsigned char rl;
  unsigned char rr;
};

// robot state structure
typedef struct roboBallanceState {
  unsigned char multiplierNow;
  bool dynamicEnabled;
  bool switchEnabled;
} roboBallanceState;

// robot state
roboBallanceState rlState = {
  4,              // unsigned char multiplierNow;
  false,          // bool dynamicEnabled
  true            // bool switchEnabled
};

// ballance enabled flag
bool ballanceEnabled = false;
// static forward ballance value
char staticForward = 0;
char staticForwardTemp = 0;
// dynamic forward ballance value
char dynamicForward = 0;
// switches data
switches swState = {1, 1, 1, 1};
switches swSum = {0, 0, 0, 0};
switches swCount = {0, 0, 0, 0};
// switch forward ballance value
char switchForward = 0;

// static ballance
void _updateStaticBallance(void) {
    staticForwardTemp = (short)(m_gyroState.aPitchNow * rlState.multiplierNow); // 4 or 2 has to be tuned
  // 
  if ((staticForward > staticForwardTemp) && (staticForward > -STATIC_BALLANCE_MAX)) {
    staticForward --;
  } else if ((staticForward < staticForwardTemp) && (staticForward < STATIC_BALLANCE_MAX)) {
    staticForward ++;
  }
}

// dynamic ballance
void _updateDynamicBallance(void) {
  // get diving value
  if (m_legsValue.fl.lifted) {
    if (m_gyroState.aLiftFL < m_gyroState.aLiftRR) {
      // fl leg too heavy
      dynamicForward --;
    }
  } else if (m_legsValue.fr.lifted) {
    if (m_gyroState.aLiftFR < m_gyroState.aLiftRL) {
      // fr leg too heavy
      dynamicForward --;
    }
  } else if (m_legsValue.rl.lifted) {
    if (m_gyroState.aLiftRL < m_gyroState.aLiftFR) {
      // rl leg too heavy
      dynamicForward ++;
    }
  } else if (m_legsValue.rr.lifted) {
    if (m_gyroState.aLiftRR < m_gyroState.aLiftFL) {
      // rr leg too heavy
      dynamicForward ++;
    }
  }
  if (dynamicForward > DYNAMIC_BALLANCE_MAX) {
    dynamicForward = DYNAMIC_BALLANCE_MAX;
  }
  if (dynamicForward < -DYNAMIC_BALLANCE_MAX) {
    dynamicForward = -DYNAMIC_BALLANCE_MAX;
  }
}

// process switches for ballnce
void _updateSwitchBallance(void) {
  if ((swCount.fl + swCount.fr) > (swCount.rl + swCount.rr)) {
    // rear too heavy
    switchForward ++;
  } else if ((swCount.fl + swCount.fr) < (swCount.rl + swCount.rr)) {
    // front too heavy
    switchForward --;
  }
  if (switchForward > SWITCH_BALLANCE_MAX) {
    switchForward = SWITCH_BALLANCE_MAX;
  }
  if (switchForward < -SWITCH_BALLANCE_MAX) {
    switchForward = -SWITCH_BALLANCE_MAX;
  }
}

// init switches
void _initSwitches(void) {
  pinMode(FL_SWITCH, INPUT_PULLUP);
  pinMode(FR_SWITCH, INPUT_PULLUP);
  pinMode(RL_SWITCH, INPUT_PULLUP);
  pinMode(RR_SWITCH, INPUT_PULLUP);
}

// init Switches
void initSwitches(bool calibrationMode) {
  Serial.println(F("initSwitches"));
  _initSwitches();
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
      Serial.print(F(" Count fl "));
      Serial.print((int)swCount.fl);
      Serial.print(F(" fr "));
      Serial.print((int)swCount.fr);
      Serial.print(F(" rl "));
      Serial.print((int)swCount.rl);
      Serial.print(F(" rr "));
      Serial.println((int)swCount.rr);
    }
  }
}

// read switches
void readSwitchesCount(unsigned char counter) {
  swState.fl = digitalRead(FL_SWITCH);
  swState.fr = digitalRead(FR_SWITCH);
  swState.rl = digitalRead(RL_SWITCH);
  swState.rr = digitalRead(RR_SWITCH);
  //
  if (counter == 0) {
    swCount.fl = swSum.fl;
    swSum.fl = 0;
    swCount.fr = swSum.fr;
    swSum.fr = 0;
    swCount.rl = swSum.rl;
    swSum.rl = 0;
    swCount.rr = swSum.rr;
    swSum.rr = 0;
    //Serial.println(" ");
  }
  //Serial.print(F(" "));
  //Serial.print((int)swState.fl);
  swSum.fl += swState.fl;
  swSum.fr += swState.fr;
  swSum.rl += swState.rl;
  swSum.rr += swState.rr;
  //
}

char updateBallanceCount(unsigned char counter, char speedLNow, char speedRNow) {
  // forward ballance
  // bigger the number more weight on front
  // pitch up - positive. require more weight on front
  if (ballanceEnabled) {
    // static ballance
    _updateStaticBallance();
    // once
    if (counter == 0) {
      // switch ballance
      if (rlState.switchEnabled) {
        _updateSwitchBallance();
      }
      // dynamic ballance
      if (rlState.dynamicEnabled) {
        _updateDynamicBallance();
      }
    }
    return staticForward + dynamicForward + switchForward;
  }
  return 0;
}

// set robot state
void setStateBallance(unsigned char newState) {
  switch (newState) {
    case ROBOT_NORM:
    {
      rlState.multiplierNow = 4;
      rlState.dynamicEnabled = false;
      rlState.switchEnabled = true;
    }
    break;
    case ROBOT_INO:
    {
      rlState.multiplierNow = 4;
      rlState.dynamicEnabled = false;
      rlState.switchEnabled = true;
    }
    break;
    case ROBOT_CRAWL:
    {
      rlState.multiplierNow = 4;
      rlState.dynamicEnabled = false;
      rlState.switchEnabled = true;
    }
    break;
    default:
    break;
  }
}

// enable ballance
void enableBallance(bool ballance) {
  ballanceEnabled = ballance;
}
