/*
Walking Hexapod Robot
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Robot static and dynamic ballance
*/

// static ballance
allMotors legUp = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
// position correction
char correctRollL  = 0;
char correctRollR  = 0;
char correctPitchF = 0;
char correctPitchR = 0;
// center position in the pattern array
char centerAbsolute = 0; // (range -12 to 12) bigger the number more weight on front
char centerAbsoluteMax = 12;
// static forward ballance
char centerForward = centerAbsolute;

allMotors getStaticBallance(accRoll gyroState, unsigned char sCounter) {
  // roll
  if (m_rollBallanceEnabled) {
    if (gyroState.accAngleX < -8) {
      if (correctRollR < 10) {
        correctRollR ++;
      }
      correctRollL = 0;
    } else if (gyroState.accAngleX > 8) {
      if (correctRollL < 10) {
        correctRollL ++;
      }
      correctRollR = 0;
    } else {
      // try to return to normal position
      if (sCounter == 0) {
        if (correctRollL > 0) {
          correctRollL --;
        }
        if (correctRollR > 0) {
          correctRollR --;
        }
      }
    }
  }
  // pitch
  if (m_pitchBallanceEnabled) {
    if (gyroState.accAngleY > 4) {
      if (correctPitchR < 10) {
        correctPitchR ++;
      }
      correctPitchF = -correctPitchR;
    } else if (gyroState.accAngleY < -4) {
      if (correctPitchF < 10) {
        correctPitchF ++;
      }
      correctPitchR = -correctPitchF;
    } else {
      // try to return to normal position
      if (sCounter == 0) {
        if (correctPitchF > 0) {
          correctPitchF --;
        }
        if (correctPitchR > 0) {
          correctPitchR --;
        }
      }
    }
  }
  legUp.m.fl.motor1 = -correctRollL - correctPitchF;
  legUp.m.fl.motor2 = -correctRollL - correctPitchF;
  legUp.m.fr.motor1 = -correctRollR - correctPitchF;
  legUp.m.fr.motor2 = -correctRollR - correctPitchF;
  legUp.m.sl.motor1 = -correctRollL;
  legUp.m.sl.motor2 = -correctRollL;
  legUp.m.sr.motor1 = -correctRollR;
  legUp.m.sr.motor2 = -correctRollR;
  legUp.m.rl.motor1 = -correctRollL - correctPitchR;
  legUp.m.rl.motor2 = -correctRollL - correctPitchR;
  legUp.m.rr.motor1 = -correctRollR - correctPitchR;
  legUp.m.rr.motor2 = -correctRollR - correctPitchR;
  return legUp;
}

// update forward ballance
char getForwardBallance(accRoll gyroState) {
  if (m_forwardBallanceEnabled) {
    if (gyroState.rollMax - gyroState.rollMin > 2) {
      // body rolls
      if ((gyroState.rollMinTime < m_halfCycle) && (gyroState.rollMaxTime > m_halfCycle - 1)) {
        // front is too heavy
        if (centerForward > -centerAbsoluteMax) {
          centerForward --;
        }
          }
      if ((gyroState.rollMinTime > m_halfCycle - 1) && (gyroState.rollMaxTime < m_halfCycle)) {
        // rear is too heavy
        if (centerForward < centerAbsoluteMax) {
          centerForward ++;
        }
      }
    }
  }
  return centerForward;
}
