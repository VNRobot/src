/*
Walking Robot TurtleV1
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Robot static and dynamic ballance
*/

// static forward ballance value
short staticForward = 0;
short staticForwardTemp = 0;
short staticForwardMax = 40;
bool ballanceEnabled = true;
// dynamic forward ballance value
short dynamicForward = 0;
short dynamicForwardMax = 20;

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
    staticForwardTemp = (short)(m_gyroState.accPitchY * 4); // 15 deg is 28
    // 
    if ((staticForward > staticForwardTemp) && (staticForward > -staticForwardMax)) {
      staticForward --;
    } else if ((staticForward < staticForwardTemp) && (staticForward < staticForwardMax)) {
      staticForward ++;
    }
    // dynamic ballance
    if (m_sequenceCounter.m == 0) {
      _updateDynamicBallance();
    }
    //
    m_legCorrect.fl.shift = staticForward + dynamicForward;
    m_legCorrect.fr.shift = staticForward + dynamicForward;
    m_legCorrect.rl.shift = staticForward + dynamicForward;
    m_legCorrect.rr.shift = staticForward + dynamicForward;
  }
}

// update forward ballance
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
