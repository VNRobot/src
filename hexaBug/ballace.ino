/*
Walking Hexapod Robot
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Robot static and dynamic ballance
*/

// dynamic ballance
allMotors legUp = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
// position correction
char correctRollL  = 0;
char correctRollR  = 0;
char correctPitch = 0;
// center position in the pattern array
char centerAbsolute = 0; // (range -16 to 16) bigger the number more weight on front
// static forward ballance
char centerStatic = centerAbsolute;
// roll ballance flag
bool rollBallanceEnabled = true;
// pitch ballance flag
bool pitchBallanceEnabled = true;
// forward ballance flag
bool forwardBallanceEnabled = true;

// get center static ballance point
char getCenterStaticBallance(void) {
  return centerStatic;
}

allMotors getStaticBallance(accRoll gyroState, unsigned char sCounter) {
  // roll
  if (rollBallanceEnabled) {
    if (gyroState.accAngleX < -2) {
      if (correctRollR < 10) {
        correctRollR ++;
        correctRollL = 0;
      }
    } else if (gyroState.accAngleX > 2) {
      if (correctRollL < 10) {
        correctRollL ++;
        correctRollR = 0;
      }
    } else {
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
      if (correctPitch < 10) {
        correctPitch ++;
      }
    } else if (gyroState.accAngleY < -2) {
      if (correctPitch > -10) {
        correctPitch --;
      }
    } else {
      if (sCounter == 0) {
        if (correctPitch > 0) {
          correctPitch --;
        }
        if (correctPitch < 0) {
          correctPitch ++;
        }
      }
    }
  }
  legUp.m.fl.motor1 = 0;
  legUp.m.fl.motor2 = -correctRollL + correctPitch;
  legUp.m.fr.motor1 = 0;
  legUp.m.fr.motor2 = -correctRollR + correctPitch;
  legUp.m.sl.motor1 = 0;
  legUp.m.sl.motor2 = -correctRollL;
  legUp.m.sr.motor1 = 0;
  legUp.m.sr.motor2 = -correctRollR;
  legUp.m.rl.motor1 = - correctPitch;
  legUp.m.rl.motor2 = -correctRollL;
  legUp.m.rr.motor1 = - correctPitch;
  legUp.m.rr.motor2 = -correctRollR;
  // forward
  if (forwardBallanceEnabled) {
    // nose down increase waight on rear
    // nose up increase waight on front
    centerStatic = centerAbsolute - gyroState.accAngleY / 4;
    if (centerStatic > 4) {
      centerStatic = 4;
    }
    if (centerStatic < -4) {
      centerStatic = -4;
    }
  }
  return legUp;
}

allMotors getDynamicBallance(accRoll gyroState) {
  return legUp;
}
