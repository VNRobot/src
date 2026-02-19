/*
Walking Robot TurtleV1
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Robot legs motion patterns
*/

// main counter
unsigned char mainCounter = 0;
// leg values for 4 legs
allLegs legsValue = {125, 0, false, 125, 0, false, 125, 0, false, 125, 0, false};
// walking mode
bool walkingMode = false;
// speed relative value from -1 to 2
char speed = 0;
char speedL = 0;
char speedR = 0;
// quick shift multiplier
unsigned char quickShiftMultiplier = 5;
// walking direction
bool walkFrward = true;

// get next sequence, mode and speed
void setPattern(void) {
  // set speed
  switch (m_robotState.patternNow) {
    case P_STANDGOLEFT:
    case P_STANDGORIGHT:
    case P_GOFORWARD:
    case P_GOLEFT:
    case P_GORIGHT:
    case P_GOBACK:
    case P_GOBACKLEFT:
    case P_GOBACKRIGHT:
    {
      if (speed < 1) {
        speed = 1;
      } else {
        speed = 2;
      }
    }
    break;
    case P_STANDGO:
    {
      speed = 0;
    }
    break;
    default:
    {
      speed = 0;
    }
    break;
  }
  // set mode and direction
  switch (m_robotState.patternNow) {
    case P_STANDGO:
    case P_GOFORWARD:
    case P_GOLEFT:
    case P_GORIGHT:
    {
      walkingMode = true;
      walkFrward = true;
    }
    break;
    case P_GOBACK:
    case P_GOBACKLEFT:
    case P_GOBACKRIGHT:
    case P_STANDGOLEFT:
    case P_STANDGORIGHT:
    {
      walkingMode = true;
      walkFrward = false;
    }
    break;
    default:
    {
      walkingMode = false;
      walkFrward = true;
    }
    break;
  }
  // set speed by side
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
    case P_GOBACKRIGHT:
    {
      speedL = speed - 1;
      speedR = speed;
    }
    break;
    case P_GORIGHT:
    case P_GOBACKLEFT:
    {
      speedL = speed;
      speedR = speed - 1;
    }
    break;
    default:
    {
      speedL = speed;
      speedR = speed;
    }
    break;
  }
  if (walkFrward) {
    m_robotState.speedNow = speed;
  } else {
    m_robotState.speedNow = -speed;
  }
  // set quick shift
  quickShiftMultiplier = (m_robotState.halfCycleNow - 3) / 3;
}

// update servo motors values
void updateCountPatterns(void) {
  // update sequence shift 
  if (walkFrward) {
    mainCounter ++;
    if (mainCounter == m_robotState.fullCycleNow) {
      mainCounter = 0;
    }
  } else {
    if (mainCounter == 0) {
      mainCounter = m_robotState.fullCycleNow - 1;
    } else {
      mainCounter --;
    }
  }
  m_sequenceCounter.m = mainCounter;
  // set counters
  m_sequenceCounter.fl = mainCounter;
  m_sequenceCounter.fr = mainCounter + m_robotState.halfCycleNow;
  if (m_robotState.robotStateNow == ROBOT_NORM) {
    // norm
    m_sequenceCounter.rl = mainCounter + m_robotState.halfCycleNow;
    m_sequenceCounter.rr = mainCounter;
  } else if (m_robotState.robotStateNow == ROBOT_INO) {
    // ino
    m_sequenceCounter.rl = mainCounter + m_robotState.pairCycleNow;
    m_sequenceCounter.rr = mainCounter + m_robotState.halfCycleNow + m_robotState.pairCycleNow;
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
  legsValue.fl.lifted = false;
  legsValue.fr.hight = hightR;
  legsValue.fr.shift = shiftR;
  legsValue.fr.lifted = false;
  legsValue.rl.hight = hightL;
  legsValue.rl.shift = shiftL;
  legsValue.rl.lifted = false;
  legsValue.rr.hight = hightR;
  legsValue.rr.shift = shiftR;
  legsValue.rr.lifted = false;
}

// get servo motor steps
allLegs getWalkPatterns(void) {
  leg legSet;
  if (walkingMode) {
    legSet = _getWalkStep(m_sequenceCounter.fl, speedL);
    legsValue.fl.hight = legSet.hight;
    legsValue.fl.shift = legSet.shift;
    legsValue.fl.lifted = legSet.lifted;
    legSet = _getWalkStep(m_sequenceCounter.fr, speedR);
    legsValue.fr.hight = legSet.hight;
    legsValue.fr.shift = legSet.shift;
    legsValue.fr.lifted = legSet.lifted;
    legSet = _getWalkStep(m_sequenceCounter.rl, speedL);
    legsValue.rl.hight = legSet.hight;
    legsValue.rl.shift = legSet.shift;
    legsValue.rl.lifted = legSet.lifted;
    legSet = _getWalkStep(m_sequenceCounter.rr, speedR);
    legsValue.rr.hight = legSet.hight;
    legsValue.rr.shift = legSet.shift;
    legsValue.rr.lifted = legSet.lifted;
  } else {
    _setLegsValuesBySide(m_robotState.legHightNow, 0, m_robotState.legHightNow, 0);
  }
  return legsValue;
}

// set walk step value
leg _getWalkStep(unsigned char counter, char speedValue) {
  // leg step values
  leg legStep = {0, 0, false};
  //
  if (counter < 4) {
    legStep.shift = -counter * quickShiftMultiplier;
    if (speedValue == 2) {
      legStep.hight = -m_robotState.legLiftNow; // lifted
    } else {
      legStep.hight = -m_robotState.legLiftNow / 2; // lifted
    }
    legStep.lifted = true;
  } else if (counter > m_robotState.fullCycleNow - 4) {
    legStep.shift = (m_robotState.fullCycleNow - counter) * quickShiftMultiplier;
    if (speedValue == 2) {
      legStep.hight = -m_robotState.legLiftNow; // lifted
    } else {
      legStep.hight = -m_robotState.legLiftNow / 2; // lifted
    }
    legStep.lifted = true;
  } else {
    legStep.lifted = false;
    legStep.shift = counter - m_robotState.halfCycleNow;
    if ((counter > m_robotState.halfCycleNow - 4) && (counter < m_robotState.halfCycleNow + 4)) {
      legStep.hight = 0; // one leg touch
    } else {
      if (counter == 4) {
        legStep.hight = -6; // touch the ground
      } else {
        legStep.hight = -2; // compensate legs flexibility
      }
    }
  }
  // hight
  legStep.hight += m_robotState.legHightNow;
  // forward shift
  legStep.shift = legStep.shift * speedValue * m_robotState.speedMuliplierNow;
  return legStep;
}
