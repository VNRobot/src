/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Robot static and dynamic ballance
*/

// dynamic ballance
allMotors legUp = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
// position correction
int correctRoll  = 0;
int correctPitch = 0;
// center motor default position for dynamic ballance
char centerF = 0; // positive more distance between legs
char centerR = 0; // positive more distance between legs
// center position in the pattern array
char centerAbsolute = 0; // (range -16 to 16) bigger the number more weight on front
// static forward ballance
char centerStatic = centerAbsolute;

// get center static ballance point
char getCenterStaticBallance(void) {
  return centerStatic;
}


void updateStaticBallance(accRoll gyroState) {
  if (gyroState.accAngleX > 2) {
    if (correctRoll < 10) {
      correctRoll ++;
    }
  } else if (gyroState.accAngleX < -2) {
    if (correctRoll > -10) {
      correctRoll --;
    }
  }
  if (gyroState.accAngleY > 2) {
    if (correctPitch < 10) {
      correctPitch ++;
    }
  } else if (gyroState.accAngleY < -2) {
    if (correctPitch > -10) {
      correctPitch --;
    }
  }
  legUp.m.fl.motor1 = -correctRoll + correctPitch;
  legUp.m.fl.motor2 = -correctRoll + correctPitch;
  legUp.m.fr.motor1 = correctRoll + correctPitch;
  legUp.m.fr.motor2 = correctRoll + correctPitch;
  legUp.m.rl.motor1 = -correctRoll - correctPitch;
  legUp.m.rl.motor2 = -correctRoll - correctPitch;
  legUp.m.rr.motor1 = correctRoll - correctPitch;
  legUp.m.rr.motor2 = correctRoll - correctPitch;
  // nose down increase waight on rear
  // nose up increase waight on front
  centerStatic = centerAbsolute - gyroState.accAngleY / 2;
  if (centerStatic > 16) {
    centerStatic = 16;
  }
  if (centerStatic < 8) {
    centerStatic = 8;
  }
}

allMotors getDynamicBallance(accRoll gyroState) {
  if (gyroState.rollMax - gyroState.rollMin > 2) {
    // body rolls
    if ((gyroState.rollMinTime < m_halfCycle) && (gyroState.rollMaxTime > m_halfCycle - 1)) {
      // front is too heavy
      // increase weight on rear
      legUp.m.st.motor1 -= 1;
    }
    if ((gyroState.rollMinTime > m_halfCycle - 1) && (gyroState.rollMaxTime < m_halfCycle)) {
      // rear is too heavy
      //increase wight on front
      legUp.m.st.motor1 += 1;
    }
  }
  if (legUp.m.st.motor1 < -10) {
    legUp.m.st.motor1 = -10;
    // change static ballance
  } else if (legUp.m.st.motor1 > 10) {
    legUp.m.st.motor1 = 10;
    // change static ballance
  }
  legUp.m.st.motor2 = - legUp.m.st.motor1;
  return legUp;
}
