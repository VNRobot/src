/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Robot legs motion patterns
*/

// main counter
unsigned char mainCounter = 0;
// leg values for 4 legs
allLegs legsValue = {125, 0, 125, 0, 125, 0, 125, 0, 125, 0, 125, 0};
// leg phase
phase legPhase = {0, 0, 0, 0, 0, 0, 0};
// walking mode
bool walkingMode = false;
// speed relative value from -1 to 2
char speed = 0;
char speedL = 0;
char speedR = 0;
// walk and lift                                                                                                 -
short mLiftHFlag[36]       = {  1,  1,  1,  1,  3,  5,  10,  10,  10,  10,  10,  10,  10,  10,  20,1000,1000,1000,1000,1000,1000,1000,1000,1000, 20, 10, 10, 10, 10, 10, 10,  10,  10,   2,  1,   1};
char mPointWalk[36]        = {  0, -5,-10,-15,-14,-13, -12, -11, -10,  -9,  -8,  -7,  -6,  -5,  -4,  -3,  -2,  -1,   0,   1,   2,   3,   4,   5,  6,  7,  8,  9, 10, 11, 12,  13,  14,  15, 10,   5};
// ino
short mLiftHFlagIno[36]       = {  1,  1,  1,  1, 10, 10,  10,  10,  10,  10,  10,  10,  10,  10,  10,  10,  10,  10,  10,  10,  10,  10,  10,  10, 10, 10, 10, 10, 10, 10, 10,  10,  10,  10,  1,   1};
char mPointWalkIno[36]        = {  0, -5,-10,-15,-14,-13, -12, -11, -10,  -9,  -8,  -7,  -6,  -5,  -4,  -3,  -2,  -1,   0,   1,   2,   3,   4,   5,  6,  7,  8,  9, 10, 11, 12,  13,  14,  15, 10,   5};
// swim
short mLiftHFlagSwim[36]   = {  1,  1,  1,  1,  1,  1,   1,   1,   1,  10,  10,  10,  10,  10,  20,1000,1000,1000,1000,1000,1000,1000,1000,1000, 20, 10, 10, 10,  2,  1,  1,   1,   1,   1,  1,   1};
char mPointWalkSwim[36]    = {  0, -2, -4, -6, -8,-10, -12, -14, -16, -18, -16, -14, -12, -10,  -8,  -6,  -4,  -2,   0,   2,   4,   6,   8,  10, 12, 14, 16, 18, 16, 14, 12,  10,   8,   6,  4,   2};
// recover
short mRecoverDown[36]     = {100, 70, 50, 40, 30, 30,  30,  30,  30,  30,  30,  30,  30,  30,  30,  30,  30,  30,  30,  30,  30,  30,  30,  30, 30, 30, 40, 50, 60, 70, 80,  90, 100, 110,120, 125};
short mRecoverUp[36]       = {100, 80, 80, 80, 80, 80, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150,  80,  80,  80,  80, 80, 80, 80, 80, 80, 80, 80,  90, 100, 110,120, 125};
// pattern item buffer
unsigned char taskItemBuffer = P_DOSTAND;
// legshift temp value for transition
unsigned char legShiftTemp = m_robotState.shiftCycleNow;

// get walking mode
bool getWalkingMode(void) {
  return walkingMode;
}

// get next sequence, mode and speed
void setPattern(unsigned char currentPattern, char angleTurn) {
  taskItemBuffer = currentPattern;
  // set mode
  switch (currentPattern) {
    case P_STANDGO:
    {
      speed = 0;
      speedL = speed;
      speedR = speed;
      if (angleTurn > 4) {
        speedL = 1;
        speedR = 0; //-1;
      } else if (angleTurn < -4) {
        speedL = 0; //-1;
        speedR = 1;
      }
      walkingMode = true;
    }
    break;
    case P_STANDGOLEFT:
    case P_STANDGOSHIFTLEFT:
    {
      speed = 0;
      speedL = 0; //-1;
      speedR = 1;
      walkingMode = true;
    }
    break;
    case P_STANDGORIGHT:
    case P_STANDGOSHIFTRIGHT:
    {
      speed = 0;
      speedL = 1;
      speedR = 0; //-1;
      walkingMode = true;
    }
    break;
    case P_GOFORWARD:
    {
      // increase speed
      if (speed < 2) {
        speed ++;
      }
      speedL = speed;
      speedR = speed;
      if (angleTurn > 4) {
        speedR = speedL - 1;
      } else if (angleTurn < -4) {
        speedL = speedR - 1;
      }
      walkingMode = true;
    }
    break;
    case P_GOLEFT:
    case P_GOSHIFTLEFT:
    {
      // increase speed
      if (speed < 2) {
        speed ++;
      }
      speedL = speed - 1;
      speedR = speed;
      walkingMode = true;
    }
    break;
    case P_GORIGHT:
    case P_GOSHIFTRIGHT:
    {
      // increase speed
      if (speed < 2) {
        speed ++;
      }
      speedL = speed;
      speedR = speed - 1;
      walkingMode = true;
    }
    break;
    case P_GOBACK:
    {
      speed = -1;
      speedL = speed;
      speedR = speed;
      walkingMode = true;
    }
    break;
    case P_GOBACKLEFT:
    {
      speed = -1;
      speedL = -1;
      speedR = 0;
      walkingMode = true;
    }
    break;
    case P_GOBACKRIGHT:
    {
      speed = -1;
      speedL = 0;
      speedR = -1;
      walkingMode = true;
    }
    break;
    default:
    {
      speed = 0;
      speedL = speed;
      speedR = speed;
      walkingMode = false;
    }
    break;
  }
  if (!m_robotState.stepSteeringNow) {
    speedL = speed;
    speedR = speed;
  }
}

// update servo motors values
phase updateCountPatterns(unsigned char legShift, unsigned char fullCycle) {
  // update sequence shift 
  mainCounter ++;
  if (mainCounter >= fullCycle) {
    mainCounter = 0;
    // update shift
    if (legShiftTemp < legShift) {
      legShiftTemp ++;
    } else if (legShiftTemp > legShift) {
      legShiftTemp --;
    }
  }
  legPhase.m = mainCounter;
  if (m_robotState.robotStateNow == ROBOT_INO) {
    // ino walk
    legPhase.rr = mainCounter;
    legPhase.rl = legPhase.rr + m_robotState.halfCycleNow;
    legPhase.fl = mainCounter + legShift; 
    legPhase.fr = legPhase.fl + m_robotState.halfCycleNow;
  } else {
    // regular walk
    legPhase.rr = mainCounter;
    legPhase.rl = legPhase.rr + m_robotState.halfCycleNow;
    legPhase.fl = mainCounter + legShiftTemp; 
    legPhase.fr = legPhase.fl + m_robotState.halfCycleNow;
  }
  if (m_init.motorsCount == 12) {
    // 12 motors
    legPhase.hr = mainCounter + legShiftTemp * 2;
    legPhase.hl = legPhase.hr + m_robotState.halfCycleNow;
    if (legPhase.hl >= fullCycle) {
      legPhase.hl -= fullCycle;
    }
    if (legPhase.hr >= fullCycle) {
      legPhase.hr -= fullCycle;
    }
  }
  if (legPhase.fl >= fullCycle) {
    legPhase.fl -= fullCycle;
  }
  if (legPhase.fr >= fullCycle) {
    legPhase.fr -= fullCycle;
  }
  if (legPhase.rl >= fullCycle) {
    legPhase.rl -= fullCycle;
  }
  if (legPhase.rr >= fullCycle) {
    legPhase.rr -= fullCycle;
  }
  // no need for rr
  return legPhase;
}

// set same values for legs
void _setLegsValuesBySide (short hightL, short shiftL, short hightR, short shiftR) {
  legsValue.hl.hight = hightL;
  legsValue.hl.shift = shiftL;
  legsValue.hr.hight = hightR;
  legsValue.hr.shift = shiftR;
  legsValue.fl.hight = hightL;
  legsValue.fl.shift = shiftL;
  legsValue.fr.hight = hightR;
  legsValue.fr.shift = shiftR;
  legsValue.rl.hight = hightL;
  legsValue.rl.shift = shiftL;
  legsValue.rr.hight = hightR;
  legsValue.rr.shift = shiftR;
}

// get servo motor steps
allLegs getWalkPatterns(void) {
  leg legSet;
  switch (taskItemBuffer) {
    case P_STANDTOGO:
    {
      m_robotState.inputDistanceNow = m_init.normalInputDistance;
      _setLegsValuesBySide(m_init.defaultHight, 0, m_init.defaultHight, 0);
    }
    break;
    case P_GOTOSTAND:
    {
      m_robotState.inputDistanceNow = m_init.normalInputDistance / 2;
      _setLegsValuesBySide(m_init.defaultHight, 0, m_init.defaultHight, 0);
    }
    break;
    case P_RECOVERLEFT:
    {
      _setLegsValuesBySide(mRecoverDown[mainCounter], 0, mRecoverUp[mainCounter], 0);
    }
    break;
    case P_RECOVERRIGHT:
    {
      _setLegsValuesBySide(mRecoverUp[mainCounter], 0, mRecoverDown[mainCounter], 0);
    }
    break;
    default:
    {
      if (walkingMode) {
        if (m_init.motorsCount == 12) {
          legSet = _getWalkStep(legPhase.hl, speedL);
          legsValue.hl.hight = legSet.hight;
          legsValue.hl.shift = legSet.shift;
          legSet = _getWalkStep(legPhase.hr, speedR);
          legsValue.hr.hight = legSet.hight;
          legsValue.hr.shift = legSet.shift;
        }
        legSet = _getWalkStep(legPhase.fl, speedL);
        legsValue.fl.hight = legSet.hight;
        legsValue.fl.shift = legSet.shift;
        legSet = _getWalkStep(legPhase.fr, speedR);
        legsValue.fr.hight = legSet.hight;
        legsValue.fr.shift = legSet.shift;
        legSet = _getWalkStep(legPhase.rl, speedL);
        legsValue.rl.hight = legSet.hight;
        legsValue.rl.shift = legSet.shift;
        legSet = _getWalkStep(legPhase.rr, speedR);
        legsValue.rr.hight = legSet.hight;
        legsValue.rr.shift = legSet.shift;
      } else {
        _setLegsValuesBySide(m_init.defaultHight, 0, m_init.defaultHight, 0);
      }
    }
    break;
  }
  return legsValue;
}

// set walk step value
leg _getWalkStep(unsigned char counter, char speedValue) {
  short shift = 0;
  // leg step values
  leg legStep = {0, 0};
  // forward shift
  switch (speedValue) {
    case -1:
    {
      if (m_robotState.robotStateNow == ROBOT_SWIM) {
        shift =  - mPointWalkSwim[counter];
      } else {
        if (m_robotState.robotStateNow == ROBOT_INO) {
          shift =  - mPointWalkIno[counter];
        } else {
          shift =  - mPointWalk[counter];
        }
      }
    }
    break;
    case 1:
    {
      if (m_robotState.robotStateNow == ROBOT_SWIM) {
        shift = mPointWalkSwim[counter];
      } else {
        if (m_robotState.robotStateNow == ROBOT_INO) {
          shift = mPointWalkIno[counter];
        } else {
          shift = mPointWalk[counter];
        }
      }
    }
    break;
    case 2:
    {
      if (m_robotState.robotStateNow == ROBOT_SWIM) {
         shift = mPointWalkSwim[counter] * 2;
      } else {
        if (m_robotState.robotStateNow == ROBOT_INO) {
          shift = mPointWalkIno[counter] * 2;
        } else {
          shift = mPointWalk[counter] * 2;
        }
      }
    }
    break;
    default:
    break;
  }
  if (m_robotState.robotStateNow == ROBOT_SWIM) {
    legStep.hight = m_robotState.legHightNow - (int)(m_robotState.legLiftNow / mLiftHFlagSwim[counter]);
  } else {
    if (m_robotState.robotStateNow == ROBOT_INO) {
      legStep.hight = m_robotState.legHightNow - (int)(m_robotState.legLiftNow / mLiftHFlagIno[counter]);
    } else {
      legStep.hight = m_robotState.legHightNow - (int)(m_robotState.legLiftNow / mLiftHFlag[counter]);
    }
  }
  legStep.shift = shift * m_robotState.speedMuliplierNow;
  return legStep;
}
