/*
Walking Robot TurtleV1
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Robot static and dynamic ballance
*/

// ballance enabled flag
bool ballanceEnabled = true;
// static forward ballance value
char staticForward = 0;
char staticForwardTemp = 0;
// dynamic forward ballance value
char dynamicForward = 0;
// speed correction
char speedCorrection = 0;
// final forward shift
char finalForwardShift = 0;

void updateBallance(void) {
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
    _updateStaticBallance();
    // once
    if (m_sequenceCounter.m == 0) {
      if (m_robotState.robotStateNow == ROBOT_NORM) {
        // dynamic ballance
        //_updateDynamicBallance();
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

// static ballance
void _updateStaticBallance(void) {
  if (m_robotState.robotStateNow == ROBOT_NORM) {
    staticForwardTemp = (short)(m_gyroState.aPitchAverage * 4); // 4 has to be tuned
  } else if (m_robotState.robotStateNow == ROBOT_INO) {
    staticForwardTemp = (short)(m_gyroState.aPitchAverage * 2); // 2 has to be tuned
  } else if (m_robotState.robotStateNow == ROBOT_CRAWL) {
    staticForwardTemp = (short)(m_gyroState.aPitchAverage * 2); // 2 has to be tuned
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
}
