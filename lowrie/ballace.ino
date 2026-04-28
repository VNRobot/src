/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Robot static and dynamic ballance
*/

// ballance correction max value
#define STATIC_BALLANCE_MAX     60
#define DYNAMIC_BALLANCE_MAX    20

// ballance enabled flag
bool ballanceEnabled = false;
// static forward ballance value
char staticForward = 0;
char staticForwardTemp = 0;
// dynamic forward ballance value
char dynamicForward = 0;
// speed correction
char speedCorrection = 0;
// final forward shift
char finalForwardShift = 0;

// static ballance
void _updateStaticBallance(unsigned char robotState) {
  if (robotState == ROBOT_NORM) {
    staticForwardTemp = (short)(m_gyroState.aPitchNow * 4); // 4 has to be tuned
  } else if (robotState == ROBOT_INO) {
    staticForwardTemp = (short)(m_gyroState.aPitchNow * 2); // 2 has to be tuned
  } else if (robotState == ROBOT_CRAWL) {
    staticForwardTemp = (short)(m_gyroState.aPitchNow * 2); // 2 has to be tuned
  }
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

void updateBallanceCount(unsigned char robotState) {
  // hight
  //m_legCorrect.fl.hight = 0;
  //m_legCorrect.fr.hight = 0;
  //m_legCorrect.rl.hight = 0;
  //m_legCorrect.rr.hight = 0;
  //
  // forward ballance
  // bigger the number more weight on front
  // pitch up - positive. require more weight on front
  if (ballanceEnabled) {
    // static ballance
    _updateStaticBallance(robotState);
    // once
    if (m_sequenceCounter.m == 0) {
      // dynamic ballance
      _updateDynamicBallance();
      //
      if (robotState == ROBOT_NORM) {
        // speed correction
        //speedCorrection = m_robotState.speedNow * 2; // 2 has to be tuned
      }
    }
    // correct legs
    finalForwardShift = staticForward + dynamicForward + speedCorrection;
    m_legCorrect.fl.shift = finalForwardShift;
    m_legCorrect.fr.shift = finalForwardShift;
    m_legCorrect.rl.shift = finalForwardShift;
    m_legCorrect.rr.shift = finalForwardShift;
  }
}
