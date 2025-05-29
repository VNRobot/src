/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Robot static and dynamic ballance
*/

struct surface {
  char FL;
  char FR;
  char RL;
  char RR;
};

// touch map 0 - lifted, 1 - touched, 2 - any
char mTouchFlag[36]     = { 0, 0,  0,  0,  0,  2,  2,  1,  1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0};
//char mLiftHFlag[36]   = { 3, 3,  3,  3,  2,  1,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 3, 3};

// ballance correction
allLegs legCorrect = {0, 0, 0, 0, 0, 0, 0, 0};
// heignt position correction
char correctRollL  = 0;
char correctRollR  = 0;
// shift position correction
char correctPitch = 0;
// bigger the number more weight on front
// forward ballance
char centerForward = 0;
// surface levels
surface surfacelevel = {0, 0, 0, 0};
// surface level step
char surfacelevelStep = 4; // mm

// update roll data
void _updateRoll(int angleX, unsigned char sCounter) {
  if (angleX < -8) {
    if (correctRollR > -10) {
      correctRollR --;
    }
    correctRollL = 0;
  } else if (angleX > 8) {
    if (correctRollL > -10) {
      correctRollL --;
    }
    correctRollR = 0;
  } else {
    // try to return to normal position
    if (sCounter == 0) {
      if (correctRollL < 0) {
        correctRollL ++;
      }
      if (correctRollR < 0) {
        correctRollR ++;
      }
    }
  }
}

// update pitch data
void _updatePitch(int angleY, unsigned char sCounter) {
  if (angleY > 4) {
    if (correctPitch > -20) {
      correctPitch --;
    }
  } else if (angleY < -4) {
    if (correctPitch < 20) {
      correctPitch ++;
    }
  } else {
    // try to return to normal position
    if (sCounter == 0) {
      if (correctPitch > 0) {
        correctPitch --;
      } else if (correctPitch < 0) {
        correctPitch ++;
      }
    }
  }
}

// update forward ballance
void _updateForwardBallance(accRoll gyroState) {
  if (gyroState.rollMax - gyroState.rollMin > 2) {
    // body rolls
    if ((gyroState.rollMinTime < m_halfCycle) && (gyroState.rollMaxTime > m_halfCycle - 1)) {
      // front is too heavy
      if (centerForward > -20) {
        centerForward --;
      }
        }
    if ((gyroState.rollMinTime > m_halfCycle - 1) && (gyroState.rollMaxTime < m_halfCycle)) {
      // rear is too heavy
      if (centerForward < 20) {
        centerForward ++;
      }
    }
  }
}

// check touch
char _updateSurfaceLevel(char level, unsigned char counter, bool touch) {
  // forget when leg is lifted
  if (counter == m_fullCycle - 3) {
    level = 0;
  } else {
    // check touch
    if (mTouchFlag[counter] == 0 && touch) {
      // should be lifted
      if (level > -10) {
        level --;
      }
    } else if (mTouchFlag[counter] == 1 && !touch) {
      // should be touched
      if (level < 10) {
        level ++;
      }
    }
  }
  return level;
}

// update forward ballance
void _updateTouch(phase sCounter, touch touchInputs) {
  // front equilize
  if (surfacelevel.FL < 0 && surfacelevel.FR < 0) {
    surfacelevel.FL ++;
    surfacelevel.FR ++;
  }
  if (surfacelevel.FL > 0 && surfacelevel.FR > 0) {
    surfacelevel.FL --;
    surfacelevel.FR --;
  }
  // rear equilize
  if (surfacelevel.RL < 0 && surfacelevel.RR < 0) {
    surfacelevel.RL ++;
    surfacelevel.RR ++;
  }
  if (surfacelevel.RL > 0 && surfacelevel.RR > 0) {
    surfacelevel.RL --;
    surfacelevel.RR --;
  }
  // check and conpensate touch
  surfacelevel.FL = _updateSurfaceLevel(surfacelevel.FL, sCounter.FL, touchInputs.set1);//set2
  surfacelevel.FR = _updateSurfaceLevel(surfacelevel.FR, sCounter.FR, touchInputs.set1);//set2
  //surfacelevel.RL = _updateSurfaceLevel(surfacelevel.RL, sCounter.RL, touchInputs.set3);
  //surfacelevel.RR = _updateSurfaceLevel(surfacelevel.RR, sCounter.RR, touchInputs.set3);
}

allLegs getStaticBallance(accRoll gyroState, phase sCounter, touch touchInputs) {
  // roll
  if (m_rollBallanceEnabled) {
    _updateRoll(gyroState.accAngleX, sCounter.RR);
  }
  // pitch
  if (m_pitchBallanceEnabled) {
    _updatePitch(gyroState.accAngleY, sCounter.RR);
  }
  // forward ballance
  if (m_forwardBallanceEnabled && sCounter.RR == 0) {
    _updateForwardBallance(gyroState);
  }
  // touch inputs
  if (m_touchBallanceEnabled) {
    _updateTouch(sCounter, touchInputs);
  }
  legCorrect.fl.hight = correctRollL + surfacelevel.FL * surfacelevelStep;
  legCorrect.fl.shift = correctPitch + centerForward;
  legCorrect.fr.hight = correctRollR + surfacelevel.FR * surfacelevelStep;
  legCorrect.fr.shift = correctPitch + centerForward;
  legCorrect.rl.hight = correctRollL + surfacelevel.RL * surfacelevelStep;
  legCorrect.rl.shift = correctPitch + centerForward;
  legCorrect.rr.hight = correctRollR + surfacelevel.RR * surfacelevelStep;
  legCorrect.rr.shift = correctPitch + centerForward;
  return legCorrect;
}

