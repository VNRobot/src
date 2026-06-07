/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Robot static and dynamic ballance
*/

// ballance correction max value
#define STATIC_BALLANCE_MAX     60
#define DYNAMIC_BALLANCE_MAX    20

// robot state structure
typedef struct roboBallanceState {
  unsigned char multiplierNow;
  unsigned char speedNow;
  bool dynamicEnabled;
} roboBallanceState;

// robot state
roboBallanceState rlState = {
  4,              // unsigned char multiplierNow;
  2,              // unsigned char speedNow
  true            // bool dynamicEnabled
};

// ballance enabled flag
bool ballanceEnabled = false;
// static forward ballance value
char staticForward = 0;
char staticForwardTemp = 0;
// dynamic forward ballance value
char dynamicForward = 0;

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
  if (dynamicForward > 20) {
    dynamicForward = 20;
  }
  if (dynamicForward < -20) {
    dynamicForward = -20;
  }
  dynamicForward = 0;
}

char updateBallanceCount(unsigned char counter) {
  // forward ballance
  // bigger the number more weight on front
  // pitch up - positive. require more weight on front
  if (ballanceEnabled) {
    // static ballance
    _updateStaticBallance();
    // once
    if (counter == 0) {
      // dynamic ballance
      if (rlState.dynamicEnabled) {
        _updateDynamicBallance();
      }
    }
    return staticForward + dynamicForward + rlState.speedNow;
  }
  return 0;
}

// set robot state
void setStateBallance(unsigned char newState) {
  switch (newState) {
    case ROBOT_NORM:
    {
      rlState.multiplierNow = 4;
      rlState.speedNow = 4;
      rlState.dynamicEnabled = true;
    }
    break;
    case ROBOT_INO:
    {
      rlState.multiplierNow = 4;
      rlState.speedNow = 4;
      rlState.dynamicEnabled = true;
    }
    break;
    case ROBOT_CRAWL:
    {
      rlState.multiplierNow = 4;
      rlState.speedNow = 4;
      rlState.dynamicEnabled = true;
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
