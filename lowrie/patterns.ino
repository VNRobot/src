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
// quick shift multiplier
unsigned char quickShiftMultiplier = 5;


// get next sequence, mode and speed
void setPattern(void) {
  // set speed
  switch (m_robotState.patternNow) {
    case P_STANDGO:
    {
      speed = 0;
      walkingMode = true;
    }
    break;
    case P_STANDGOLEFT:
    case P_STANDGORIGHT:
    {
      speed = -2;
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
      speedL = speed;
      speedR = 0;
    }
    break;
    case P_STANDGORIGHT:
    {
      speedL = 0;
      speedR = speed;
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
  // set quick shift
  quickShiftMultiplier = (m_robotState.halfCycleNow - 3) / 3;
}

// update servo motors values
void updateCountPatterns(void) {
  // update sequence shift 
  mainCounter ++;
  if (mainCounter >= m_robotState.fullCycleNow) {
    mainCounter = 0;
  }
  m_sequenceCounter.m = mainCounter;
  // set counters
  if (m_robotState.robotStateNow == ROBOT_NORM) {
    // norm
    m_sequenceCounter.fl = mainCounter; 
    m_sequenceCounter.rl = mainCounter + m_robotState.halfCycleNow;
    m_sequenceCounter.rr = mainCounter;
    m_sequenceCounter.fr = mainCounter + m_robotState.halfCycleNow;
  } else if (m_robotState.robotStateNow == ROBOT_INO) {
    // ino
    if (m_robotState.speedNow >= 0) {
      // forward
      m_sequenceCounter.fl = mainCounter + m_robotState.halfCycleNow;
      m_sequenceCounter.rl = mainCounter + m_robotState.halfCycleNow + m_robotState.pairCycleNow;
      m_sequenceCounter.rr = mainCounter                             + m_robotState.pairCycleNow;
      m_sequenceCounter.fr = mainCounter;
    } else { 
      // back
      m_sequenceCounter.fl = mainCounter + m_robotState.halfCycleNow + m_robotState.pairCycleNow;
      m_sequenceCounter.rl = mainCounter + m_robotState.halfCycleNow;
      m_sequenceCounter.rr = mainCounter;
      m_sequenceCounter.fr = mainCounter                             + m_robotState.pairCycleNow; 
    }
  }
  //
  if (m_sequenceCounter.fl >= m_robotState.fullCycleNow) {
    m_sequenceCounter.fl -= m_robotState.fullCycleNow;
  }
  if (m_sequenceCounter.fr >= m_robotState.fullCycleNow) {
    m_sequenceCounter.fr -= m_robotState.fullCycleNow;
  }
  if (m_sequenceCounter.rl >= m_robotState.fullCycleNow) {
    m_sequenceCounter.rl -= m_robotState.fullCycleNow;
  }
  if (m_sequenceCounter.rr >= m_robotState.fullCycleNow) {
    m_sequenceCounter.rr -= m_robotState.fullCycleNow;
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
  char fowardPoint = 0;
  short liftPoint = 1;
  //
  if (counter < 4) {
    fowardPoint = -counter * quickShiftMultiplier;
    liftPoint = 1;
  } else if (counter > m_robotState.fullCycleNow - 4) {
    fowardPoint = (m_robotState.fullCycleNow - counter) * quickShiftMultiplier;
    liftPoint = 1;
  } else {
    fowardPoint = counter - m_robotState.halfCycleNow;
    if ((counter > m_robotState.halfCycleNow - 4) && (counter < m_robotState.halfCycleNow + 4)) {
      liftPoint = 1000;
    } else {
      liftPoint = 10;
    }
  }
  // leg step values
  leg legStep = {0, 0};
  // forward shift
  legStep.hight = m_robotState.legHightNow - (short)(m_robotState.legLiftNow / liftPoint);
  legStep.shift = (short)(fowardPoint * speedValue * m_robotState.speedMuliplierNow);
  return legStep;
}
