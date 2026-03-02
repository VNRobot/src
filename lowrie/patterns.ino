/*
Walking Robot TurtleV1
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Robot legs motion patterns
*/

// main counter
unsigned char mainCounter = 0;
unsigned char walkCounter = 0;
unsigned char walkForwardCounter = 0;
// walking mode
bool walkingMode = false;
// speed relative value from -1 to 2
char speed = 0;
char speedL = 0;
char speedR = 0;
// walking direction
bool walkFrward = true;
// leg pair shift
unsigned char pairShift = 0;

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
}

// update servo motors values
void updateCountPatterns(void) {
  // update main counter
  mainCounter ++;
  walkForwardCounter += m_robotState.speedMuliplierNow;
  if (mainCounter >= MAIN_FULL_CYCLE) {
    mainCounter = 0;
    if (walkForwardCounter >= SERVO_FULL_CYCLE) {
      walkForwardCounter = 0;
    }
  }
  // set servo counter
  if (walkFrward) {
      walkCounter = walkForwardCounter;
  } else {
    walkCounter = SERVO_FULL_CYCLE - walkForwardCounter - 1;
  }
  // remember main counter
  m_sequenceCounter.m = mainCounter;
  // update pair shift
  if (m_robotState.robotStateNow == ROBOT_INO) {
    if (pairShift < SERVO_PAIR_SHIFT) {
      pairShift ++;
    }
  } else {
    if (pairShift > 0) {
      pairShift --;
    }
  }
  // set counters
  m_sequenceCounter.fl = walkCounter + pairShift;
  m_sequenceCounter.fr = walkCounter + SERVO_HALF_CYCLE + pairShift;
  m_sequenceCounter.rl = walkCounter + SERVO_HALF_CYCLE;
  m_sequenceCounter.rr = walkCounter;
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
  m_legsValue.fl.hight = hightL;
  m_legsValue.fl.shift = shiftL;
  m_legsValue.fl.lifted = false;
  m_legsValue.fr.hight = hightR;
  m_legsValue.fr.shift = shiftR;
  m_legsValue.fr.lifted = false;
  m_legsValue.rl.hight = hightL;
  m_legsValue.rl.shift = shiftL;
  m_legsValue.rl.lifted = false;
  m_legsValue.rr.hight = hightR;
  m_legsValue.rr.shift = shiftR;
  m_legsValue.rr.lifted = false;
}

// get servo motor steps
void getWalkPatterns(void) {
  leg legSet;
  if (walkingMode) {
    legSet = _getWalkStep(m_sequenceCounter.fl, speedL);
    m_legsValue.fl.hight = legSet.hight;
    m_legsValue.fl.shift = legSet.shift;
    m_legsValue.fl.lifted = legSet.lifted;
    legSet = _getWalkStep(m_sequenceCounter.fr, speedR);
    m_legsValue.fr.hight = legSet.hight;
    m_legsValue.fr.shift = legSet.shift;
    m_legsValue.fr.lifted = legSet.lifted;
    legSet = _getWalkStep(m_sequenceCounter.rl, speedL);
    m_legsValue.rl.hight = legSet.hight;
    m_legsValue.rl.shift = legSet.shift;
    m_legsValue.rl.lifted = legSet.lifted;
    legSet = _getWalkStep(m_sequenceCounter.rr, speedR);
    m_legsValue.rr.hight = legSet.hight;
    m_legsValue.rr.shift = legSet.shift;
    m_legsValue.rr.lifted = legSet.lifted;
  } else {
    _setLegsValuesBySide(m_robotState.legHightNow, 0, m_robotState.legHightNow, 0);
  }
}

// set walk step value
leg _getWalkStep(unsigned char counter, char speedValue) {
  // leg step values
  leg legStep = {0, 0, false};
  // lift timing point
  unsigned char liftPoint = (2 + speedValue) * m_robotState.speedMuliplierNow;
  // quick shift multiplier
  unsigned char quickShiftMultiplier = (SERVO_HALF_CYCLE - (liftPoint - 1)) / (liftPoint - 1);
  //
  if (counter < liftPoint) {
    legStep.shift = -counter * quickShiftMultiplier;
    legStep.hight = -m_robotState.legLiftNow; // lifted
    legStep.lifted = true;
  } else if (counter > SERVO_FULL_CYCLE - liftPoint) {
    legStep.shift = (SERVO_FULL_CYCLE - counter) * quickShiftMultiplier;
    legStep.hight = -m_robotState.legLiftNow; // lifted
    legStep.lifted = true;
  } else {
    legStep.lifted = false;
    legStep.shift = counter - SERVO_HALF_CYCLE;
    if ((counter > SERVO_HALF_CYCLE - liftPoint) && (counter < SERVO_HALF_CYCLE + liftPoint)) {
      legStep.hight = 0; // one leg touch
    } else {
      if (counter == liftPoint) {
        legStep.hight = -6; // touch the ground
      } else {
        legStep.hight = -2; // compensate legs flexibility
      }
    }
  }
  // hight
  legStep.hight += m_robotState.legHightNow;
  // forward shift
  legStep.shift = legStep.shift * speedValue;
  return legStep;
}
