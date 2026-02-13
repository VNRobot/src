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

// get next sequence, mode and speed
void setPattern(void) {
  // set speed
  switch (m_robotState.patternNow) {
    case P_STANDGO:
    case P_STANDGOLEFT:
    case P_STANDGORIGHT:
    {
      speed = 0;
      walkingMode = true;
    }
    break;
    case P_GOFORWARD:
    case P_GOLEFT:
    case P_GORIGHT:
    {
      if (speed < 1) {
        speed = 1;
      } else {
        speed = 2;
      }
      walkingMode = true;
    }
    break;
    case P_GOBACK:
    case P_GOBACKLEFT:
    case P_GOBACKRIGHT:
    {
      if (speed > -1) {
        speed = -1;
      } else {
        speed = -2;
      }
      walkingMode = true;
    }
    break;
    default:
    {
      speed = 0;
      walkingMode = false;
    }
    break;
  }
  // set mode
  switch (m_robotState.patternNow) {
    case P_STANDGOLEFT:
    {
      speedL = -speed - 1;
      speedR = speed + 1;
    }
    break;
    case P_STANDGORIGHT:
    {
      speedL = speed + 1;
      speedR = -speed - 1;
    }
    break;
    case P_GOLEFT:
    {
      speedL = speed - 1;
      speedR = speed;
    }
    break;
    case P_GORIGHT:
    {
      speedL = speed;
      speedR = speed - 1;
    }
    break;
    case P_GOBACKLEFT:
    {
      speedL = speed;
      speedR = speed + 1;
    }
    break;
    case P_GOBACKRIGHT:
    {
      speedL = speed + 1;
      speedR = speed;
    }
    break;
    default:
    {
      speedL = speed;
      speedR = speed;
    }
    break;
  }
  m_robotState.speedNow = speed;
}

// update servo motors values
void updateCountPatterns(void) {
  // update sequence shift 
  mainCounter ++;
  if (mainCounter >= SERVO_FULL_CYCLE) {
    mainCounter = 0;
  }
  m_sequenceCounter.m = mainCounter;
  // regular walk
  m_sequenceCounter.rr = mainCounter;
  m_sequenceCounter.rl = m_sequenceCounter.rr + SERVO_HALF_CYCLE;
  m_sequenceCounter.fl = mainCounter; 
  m_sequenceCounter.fr = m_sequenceCounter.fl + SERVO_HALF_CYCLE;
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
  return legsValue;
}

// set walk step value
leg _getWalkStep(unsigned char counter, char speedValue) {
  short shift = 0;
  // leg step values
  leg legStep = {0, 0};
  // forward shift
  legStep.hight = m_robotState.legHightNow - (short)(m_robotState.legLiftNow / mLiftHFlag[counter]);
  legStep.shift = (short)((mPointWalk[counter] * speedValue * m_robotState.speedMuliplierNow + 5) / 10);
  return legStep;
}
