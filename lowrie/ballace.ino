/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Robot static and dynamic ballance
*/

// static ballance
allMotors legUp = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
// position correction
char correctRollL  = 0;
char correctRollR  = 0;
char correctPitchF = 0;
char correctPitchR = 0;
// center position in the pattern array
char centerAbsolute = 0; // (range -16 to 16) bigger the number more weight on front
// static forward ballance
char centerForward = centerAbsolute;
// roll ballance flag
bool rollBallanceEnabled = false;
// pitch ballance flag
bool pitchBallanceEnabled = true;
// forward ballance flag
bool forwardBallanceEnabled = true;

allMotors getStaticBallance(accRoll gyroState, unsigned char sCounter) {
  // roll
  if (rollBallanceEnabled) {
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
  if (pitchBallanceEnabled) {
    if (gyroState.accAngleY > 2) {
      if (correctPitchR < 10) {
        correctPitchR ++;
      }
      correctPitchF = 0;
    } else if (gyroState.accAngleY < -2) {
      if (correctPitchF < 10) {
        correctPitchF ++;
      }
      correctPitchR = 0;
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
  legUp.m.rl.motor1 = -correctRollL - correctPitchR;
  legUp.m.rl.motor2 = -correctRollL - correctPitchR;
  legUp.m.rr.motor1 = -correctRollR - correctPitchR;
  legUp.m.rr.motor2 = -correctRollR - correctPitchR;
  return legUp;
}

// update forward ballance
char getForwardBallance(accRoll gyroState) {
  if (gyroState.rollMax - gyroState.rollMin > 2) {
    // body rolls
    if ((gyroState.rollMinTime < m_halfCycle) && (gyroState.rollMaxTime > m_halfCycle - 1)) {
      // front is too heavy
      if (centerForward > -8) {
        centerForward --;
      }
        }
    if ((gyroState.rollMinTime > m_halfCycle - 1) && (gyroState.rollMaxTime < m_halfCycle)) {
      // rear is too heavy
      if (centerForward < 8) {
        centerForward ++;
      }
    }
  }
  return centerForward;
}
