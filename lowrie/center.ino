/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Robot center motors motion patterns
*/

// turning dividers
char mCenter[32] = {50,-4, -2, -2, -2, -2, -2,-2,-2,-2,-2,-2,-2,-2,-2,-4,50, 4, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 4};
// recovering
char mRecoverCenter[32] = {-40,-40,-40,-40,-40,-40,-40,-40,-40,-40,-40,-40,-40,-40,-40,-40,-40,-40, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 20, 0};
// current pattern
unsigned char centerPatternBuffer = 0;
// turn angle
char centerTurnF = 0;
char centerTurnR = 0;
//
motors valueCenter = {0, 0};

// get next sequence
void setCenter(unsigned char currentPattern, char angleTurn) {
  // remember current pattern
  centerPatternBuffer = currentPattern;
  // set turn angle
  switch (centerPatternBuffer) {
    case P_STANDGO:
    case P_GOFORWARD:
    case P_GOFORWARDSLOW:
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
      centerTurnF = -20;
      centerTurnR = -20;
    }
    break;
    case P_STANDGORIGHT:
    case P_GORIGHT:
    case P_GOBACKRIGHT:
    {
      centerTurnF = 20;
      centerTurnR = 20;
    }
    break;
    case P_STANDGOSHIFTLEFT:
    case P_GOSHIFTLEFT:
    {
      centerTurnF = -20;
      centerTurnR = 20;
    }
    break;
    case P_STANDGOSHIFTRIGHT:
    case P_GOSHIFTRIGHT:
    {
      centerTurnF = 20;
      centerTurnR = -20;
    }
    break;
    default:
    {
      centerTurnF = 0;
      centerTurnR = 0;
    }
    break;
  }
}

// get servo motors values
motors getValueCenter(unsigned char counter) {
  // set motors angle values
  if ((centerPatternBuffer == P_RECOVERLEFT) || (centerPatternBuffer == P_RECOVERRIGHT)) {
    valueCenter.motor1 = mRecoverCenter[counter];
    valueCenter.motor2 = mRecoverCenter[counter];
  } else {
    valueCenter.motor1 = centerTurnF / mCenter[counter];
    valueCenter.motor2 = centerTurnR / mCenter[counter];
  }
  return valueCenter;
}
