/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Robot static and dynamic ballance
*/
/*
struct surface {
  char fl;
  char fr;
  char rl;
  char rr;
};

// touch map 0 - lifted, 1 - touched, 2 - any, 3 - reset
char mTouchFlag[36]     = { 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 0};
//char mLiftHFlag[36]   = { 3, 3, 3, 3, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 3, 3};
//----------------configuration flags-----------------
// roll ballance enable flag
bool rollBallanceEnabled = false;
// pitch ballance enable flag 
bool pitchBallanceEnabled = false;
// forward ballance enable flag
bool forwardBallanceEnabled = false; 
// touch enable flag
bool touchBallanceEnabled = false; 
//----------------------------------------------------

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
// surface level step multiplier
char surfacelevelStep = 10; // mm
char surfacelevelMax = 50; // mm

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
    if ((gyroState.rollMinTime < SERVO_HALF_CYCLE) && (gyroState.rollMaxTime > SERVO_HALF_CYCLE - 1)) {
      // front is too heavy
      if (centerForward > -20) {
        centerForward --;
      }
    }
    if ((gyroState.rollMinTime > SERVO_HALF_CYCLE - 1) && (gyroState.rollMaxTime < SERVO_HALF_CYCLE)) {
      // rear is too heavy
      if (centerForward < 20) {
        centerForward ++;
      }
    }
  }
}

// check touch
char _updateSurfaceLevel(char level, unsigned char counter, bool touch) {
  // check touch
  if (mTouchFlag[counter] == 0 && touch) {
    // should be lifted
    if (level > -surfacelevelMax) {
      level -=surfacelevelStep;
    }
  } else if (mTouchFlag[counter] == 1 && !touch) {
    // should be touched
    if (level < surfacelevelMax) {
      level +=surfacelevelStep;
    }
  } else if (mTouchFlag[counter] == 3) {
    // reset level
    level = 0;
  }
  return level;
}

// update touch
void _updateTouch(phase sCounter) {
  // head equilize
  // front equilize
  if (surfacelevel.fl < 0 && surfacelevel.fr < 0) {
    surfacelevel.fl ++;
    surfacelevel.fr ++;
  }
  if (surfacelevel.fl > 0 && surfacelevel.fr > 0) {
    surfacelevel.fl --;
    surfacelevel.fr --;
  }
  // rear equilize
  if (surfacelevel.rl < 0 && surfacelevel.rr < 0) {
    surfacelevel.rl ++;
    surfacelevel.rr ++;
  }
  if (surfacelevel.rl > 0 && surfacelevel.rr > 0) {
    surfacelevel.rl --;
    surfacelevel.rr --;
  }
  // check and conpensate touch
  surfacelevel.fl = _updateSurfaceLevel(surfacelevel.fl, sCounter.fl, touchInputs.set1);
  surfacelevel.fr = _updateSurfaceLevel(surfacelevel.fr, sCounter.fr, touchInputs.set2);
  //surfacelevel.rl = _updateSurfaceLevel(surfacelevel.rl, sCounter.rl, touchInputs.set3);
  //surfacelevel.rr = _updateSurfaceLevel(surfacelevel.rr, sCounter.rr, touchInputs.set3);
}

allLegs getStaticBallance(accRoll gyroState, phase sCounter, bool walkingMode) {
  // roll
  if (rollBallanceEnabled) {
    _updateRoll(gyroState.accAngleX, sCounter.m);
  }
  // pitch
  if (pitchBallanceEnabled) {
    _updatePitch(gyroState.accAngleY, sCounter.m);
  }
  // forward ballance
  if (forwardBallanceEnabled && sCounter.m == 0 && walkingMode) {
    _updateForwardBallance(gyroState);
  }
  // touch inputs
  if (touchBallanceEnabled && walkingMode) {
    _updateTouch(sCounter, touchInputs);
  }
  legCorrect.fl.hight = correctRollL + surfacelevel.fl;
  legCorrect.fl.shift = correctPitch + centerForward;
  legCorrect.fr.hight = correctRollR + surfacelevel.fr;
  legCorrect.fr.shift = correctPitch + centerForward;
  legCorrect.rl.hight = correctRollL + surfacelevel.rl;
  legCorrect.rl.shift = correctPitch + centerForward;
  legCorrect.rr.hight = correctRollR + surfacelevel.rr;
  legCorrect.rr.shift = correctPitch + centerForward;
  return legCorrect;
}
*/