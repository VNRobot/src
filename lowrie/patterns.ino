/*
Walking Robot TurtleV1
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Robot legs motion patterns
*/

// main counter
unsigned char mainCounter = 0;
// leg values for 4 legs
allLegs legsValue = {125, 0, 125, 0, 125, 0, 125, 0};
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
short mLiftHFlagIno[36]    = {  1,  1,  1,  1, 10, 10,  10,  10,  10,  10,  10,  10,  10,  10,  10,  10,  1000,1000,1000,1000,1000,1000,10,  10, 10, 10, 10, 10, 10, 10, 10,  10,  10,  10,  1,   1};
char mPointWalkIno[36]     = {  0, -5,-10,-15,-14,-13, -12, -11, -10,  -9,  -8,  -7,  -6,  -5,  -4,  -3,  -2,  -1,   0,   1,   2,   3,   4,   5,  6,  7,  8,  9, 10, 11, 12,  13,  14,  15, 10,   5};
// swim
short mLiftHFlagSwim[36]   = {  1,  1,  1,  1,  1,  1,   1,   1,   1,  10,  10,  10,  10,  10,  20,1000,1000,1000,1000,1000,1000,1000,1000,1000, 20, 10, 10, 10,  2,  1,  1,   1,   1,   1,  1,   1};
char mPointWalkSwim[36]    = {  0, -2, -4, -6, -8,-10, -12, -14, -16, -18, -16, -14, -12, -10,  -8,  -6,  -4,  -2,   0,   2,   4,   6,   8,  10, 12, 14, 16, 18, 16, 14, 12,  10,   8,   6,  4,   2};
// recover
short mRecoverDown[36]     = {100, 70, 50, 40, 30, 30,  30,  30,  30,  30,  30,  30,  30,  30,  30,  30,  30,  30,  30,  30,  30,  30,  30,  30, 30, 30, 40, 50, 60, 70, 80,  90, 100, 110,120, 125};
short mRecoverUp[36]       = {100, 80, 80, 80, 80, 80, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150,  80,  80,  80,  80, 80, 80, 80, 80, 80, 80, 80,  90, 100, 110,120, 125};
// legshift temp value for transition
unsigned char legShiftTemp = m_robotState.shiftCycleNow;
// shift between left and right legs
unsigned char halfCycleTemp = m_robotState.halfCycleNow;

// get walking mode
bool getWalkingMode(void) {
  return walkingMode;
}

// get next sequence, mode and speed
void setPattern(void) {
  // set mode
  switch (m_robotState.patternNow) {
    case P_STANDGO:
    {
      speed = 0;
      speedL = speed;
      speedR = speed;
      walkingMode = true;
    }
    break;
    case P_STANDGOLEFT:
    {
      speed = 0;
      speedL = -1; //-1;
      speedR = 1;
      walkingMode = true;
    }
    break;
    case P_STANDGORIGHT:
    {
      speed = 0;
      speedL = 1;
      speedR = -1; //-1;
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
      walkingMode = true;
    }
    break;
    case P_GOLEFT:
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
      speed = -2;
      speedL = speed;
      speedR = speed;
      walkingMode = true;
    }
    break;
    case P_GOBACKLEFT:
    {
      speed = -2;
      speedL = -2;
      speedR = -1;
      walkingMode = true;
    }
    break;
    case P_GOBACKRIGHT:
    {
      speed = -2;
      speedL = -1;
      speedR = -2;
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
}

// update servo motors values
void updateCountPatterns(void) {
  if (mainCounter == 0) {
    // update shift
    if (halfCycleTemp != m_robotState.halfCycleNow) {
      // for swim
      halfCycleTemp = m_robotState.halfCycleNow;
      legShiftTemp = m_robotState.shiftCycleNow;
    } else if (legShiftTemp < m_robotState.shiftCycleNow) {
      legShiftTemp = m_robotState.shiftCycleNow; //legShiftTemp += 4;
    } else if (legShiftTemp > m_robotState.shiftCycleNow) {
      legShiftTemp = m_robotState.shiftCycleNow; //legShiftTemp -= 4;
    }
  }
  // update sequence shift 
  mainCounter ++;
  if (mainCounter >= SERVO_FULL_CYCLE) {
    mainCounter = 0;
  }
  m_sequenceCounter.m = mainCounter;
  // regular walk
  m_sequenceCounter.rr = mainCounter;
  m_sequenceCounter.rl = m_sequenceCounter.rr + halfCycleTemp;
  m_sequenceCounter.fl = mainCounter + legShiftTemp; 
  m_sequenceCounter.fr = m_sequenceCounter.fl + halfCycleTemp;
  //
  if (m_sequenceCounter.fl >= SERVO_FULL_CYCLE) {
    m_sequenceCounter.fl -= SERVO_FULL_CYCLE;
  }
  if (m_sequenceCounter.fr >= SERVO_FULL_CYCLE) {
    m_sequenceCounter.fr -= SERVO_FULL_CYCLE;
  }
  if (m_sequenceCounter.rl >= SERVO_FULL_CYCLE) {
    m_sequenceCounter.rl -= SERVO_FULL_CYCLE;
  }
  if (m_sequenceCounter.rr >= SERVO_FULL_CYCLE) {
    m_sequenceCounter.rr -= SERVO_FULL_CYCLE;
  }
}

// set same values for legs
void _setLegsValuesBySide (short hightL, short shiftL, short hightR, short shiftR) {
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
  switch (m_robotState.patternNow) {
    case P_STANDTOGO:
    {
      _setLegsValuesBySide(m_robotState.legHightNow, 0, m_robotState.legHightNow, 0);
    }
    break;
    case P_GOTOSTAND:
    {
      _setLegsValuesBySide(m_robotState.legHightNow, 0, m_robotState.legHightNow, 0);
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
        legSet = _getWalkStep(m_sequenceCounter.fl, speedL);
        legsValue.fl.hight = legSet.hight;
        legsValue.fl.shift = legSet.shift;
        legSet = _getWalkStep(m_sequenceCounter.fr, speedR);
        legsValue.fr.hight = legSet.hight;
        legsValue.fr.shift = legSet.shift;
        legSet = _getWalkStep(m_sequenceCounter.rl, speedL);
        legsValue.rl.hight = legSet.hight;
        legsValue.rl.shift = legSet.shift;
        legSet = _getWalkStep(m_sequenceCounter.rr, speedR);
        legsValue.rr.hight = legSet.hight;
        legsValue.rr.shift = legSet.shift;
      } else {
        _setLegsValuesBySide(m_robotState.legHightNow, 0, m_robotState.legHightNow, 0);
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
