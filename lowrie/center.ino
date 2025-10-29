/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Robot center motors motion patterns
*/

// turning dividers. divides 20
char mCenter[36] = {50,-5,-4,-3,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-3,-4,-5,50, 5, 4, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 4, 5};
// recovering
char mRecoverCenter[36] = { 0,-10,-20,-30,-40,-40,-40,-40,-40,-40,-40,-40,-40,-40,-40,-40,-40,-40,-20,  0, 20, 50, 50, 50, 50, 50, 50, 50, 50, 50, 40, 30, 20, 10, 0, 0};
// current pattern
unsigned char centerPatternBuffer = 0;
// turn angle
char centerTurnF = 0;
char centerTurnR = 0;
//
motors valueCenter = {0, 0};
// center motor default. range: -40 50
char centerDefault = 0;

// set center value return legs steering flag
bool setCenter(unsigned char currentPattern, char angleTurn, char centerDefaultTarget) {
  // remember current pattern
  centerPatternBuffer = currentPattern;
  // remember center default target
  if (centerDefaultTarget > centerDefault) {
    if ((centerDefaultTarget - centerDefault) > m_maxCenterTurn) {
      centerDefault += m_maxCenterTurn;
    } else {
      centerDefault = centerDefaultTarget;
    }
  }
  if (centerDefaultTarget < centerDefault) {
    if ((centerDefault - centerDefaultTarget) > m_maxCenterTurn) {
      centerDefault -= m_maxCenterTurn;
    } else {
      centerDefault = centerDefaultTarget;
    }
  }
  if ((centerDefault > m_maxCenterTurn) || (centerDefault < -m_maxCenterTurn)) {
    centerTurnF = 0;
    centerTurnR = 0;
    return true;
  }
  // set turn angle
  switch (centerPatternBuffer) {
    case P_STANDGO:
    case P_GOFORWARD:
    {
      // remember turn angle
      centerTurnF = angleTurn;
      centerTurnR = angleTurn;
    }
    break;
    case P_STANDGOLEFT:
    case P_GOLEFT:
    case P_GOBACKLEFT:
    {
      centerTurnF = -m_maxCenterTurn;
      centerTurnR = -m_maxCenterTurn;
    }
    break;
    case P_STANDGORIGHT:
    case P_GORIGHT:
    case P_GOBACKRIGHT:
    {
      centerTurnF = m_maxCenterTurn;
      centerTurnR = m_maxCenterTurn;
    }
    break;
    case P_STANDGOSHIFTLEFT:
    case P_GOSHIFTLEFT:
    {
      centerTurnF = -m_maxCenterTurn;
      centerTurnR = m_maxCenterTurn;
    }
    break;
    case P_STANDGOSHIFTRIGHT:
    case P_GOSHIFTRIGHT:
    {
      centerTurnF = m_maxCenterTurn;
      centerTurnR = -m_maxCenterTurn;
    }
    break;
    default:
    {
      centerTurnF = 0;
      centerTurnR = 0;
    }
    break;
  }
  return false;
}

// get servo motors values
motors getValueCenter(unsigned char counter) {
  // set motors angle values
  if ((centerPatternBuffer == P_RECOVERLEFT) || (centerPatternBuffer == P_RECOVERRIGHT)) {
    valueCenter.motor1 = mRecoverCenter[counter];
    valueCenter.motor2 = mRecoverCenter[counter];
  } else {
    valueCenter.motor1 = centerDefault + centerTurnF / mCenter[counter];
    valueCenter.motor2 = centerDefault + centerTurnR / mCenter[counter];
  }
  return valueCenter;
}
