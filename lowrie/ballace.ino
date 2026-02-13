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
char staticForwardMax = 40;
// dynamic forward ballance value
char dynamicForward = 0;
char dynamicForwardMax = 20;
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
      // dynamic ballance
      _updateDynamicBallance();
      // speed correction
      speedCorrection = m_robotState.speedNow * 2; // 2 has to be tuned
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
  staticForwardTemp = (short)(m_gyroState.accPitchY * 4); // 4 has to be tuned // 15 deg is 28
  // 
  if ((staticForward > staticForwardTemp) && (staticForward > -staticForwardMax)) {
    staticForward --;
  } else if ((staticForward < staticForwardTemp) && (staticForward < staticForwardMax)) {
    staticForward ++;
  }
}

// dynamic ballance
void _updateDynamicBallance(void) {
  if (m_gyroState.rollMax - m_gyroState.rollMin > 2) {
    // body rolls
    if ((m_gyroState.rollMinTime < SERVO_HALF_CYCLE) && (m_gyroState.rollMaxTime > SERVO_HALF_CYCLE - 1)) {
      // front is too heavy
      if (dynamicForward > -dynamicForwardMax) {
        dynamicForward --;
      }
    }
    if ((m_gyroState.rollMinTime > SERVO_HALF_CYCLE - 1) && (m_gyroState.rollMaxTime < SERVO_HALF_CYCLE)) {
      // rear is too heavy
      if (dynamicForward < dynamicForwardMax) {
        dynamicForward ++;
      }
    }
  }
}
