/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Robot legs motion patterns
*/

// robot state structure
typedef struct robotState {
  unsigned char robotStateNow;
  short legHightNow;
  short legLiftNow;
  char speedMuliplierNow;
} robotState;

// main counter
unsigned char mainCounter = 0;
unsigned char walkCounter = 0;
unsigned char walkForwardCounter = 0;
// leg pair shift
unsigned char pairShift = 0;
// speed dependent pattern
bool speedFlexMode = true;

// robot state
robotState rbState = {
  ROBOT_NORM,              // unsigned char robotStateNow;
  HIGHT_DEFAULT,           // short legHightNow;
  LEG_LIFT,                // short legLiftNow;
  2                        // char speedMuliplierNow; 1 or 2
};

/*
uses
m_legsValue
m_sequenceCounter
*/

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

// set walk step value
leg _getWalkStep(unsigned char counter, char speedValue) {
  // leg step values
  leg legStep = {0, 0, false};
  // lift timing point
  char speedBuffer = 2;
  if (speedFlexMode) {
    speedBuffer = speedValue;
  }
  unsigned char liftPoint = (2 + speedBuffer) * rbState.speedMuliplierNow;
  // quick shift multiplier
  unsigned char quickShiftMultiplier = (SERVO_HALF_CYCLE - (liftPoint - 1)) / (liftPoint - 1);
  //
  if (counter < liftPoint) {
    legStep.shift = -counter * quickShiftMultiplier;
    legStep.hight = -rbState.legLiftNow; // lifted
    legStep.lifted = true;
  } else if (counter > SERVO_FULL_CYCLE - liftPoint) {
    legStep.shift = (SERVO_FULL_CYCLE - counter) * quickShiftMultiplier;
    legStep.hight = -rbState.legLiftNow; // lifted
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
  legStep.hight += rbState.legHightNow;
  // forward shift
  legStep.shift = legStep.shift * speedValue;
  return legStep;
}

// update servo motors values
void updatePatternsCount(bool forwardNow) {
  // update main counter
  mainCounter ++;
  walkForwardCounter += rbState.speedMuliplierNow;
  if (mainCounter >= MAIN_FULL_CYCLE) {
    mainCounter = 0;
    if (walkForwardCounter >= SERVO_FULL_CYCLE) {
      walkForwardCounter = 0;
    }
  }
  // set servo counter
  if (forwardNow) {
      walkCounter = walkForwardCounter;
  } else {
    walkCounter = SERVO_FULL_CYCLE - walkForwardCounter - 1;
  }
  // remember main counter
  m_sequenceCounter.m = mainCounter;
  // update pair shift
  if (rbState.robotStateNow == ROBOT_INO) {
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

// get servo motor steps
void setWalkPatternsCount(bool walkingModeNow, char speedLNow, char speedRNow) {
  leg legSet;
  if (walkingModeNow) {
    legSet = _getWalkStep(m_sequenceCounter.fl, speedLNow);
    m_legsValue.fl.hight = legSet.hight;
    m_legsValue.fl.shift = legSet.shift;
    m_legsValue.fl.lifted = legSet.lifted;
    legSet = _getWalkStep(m_sequenceCounter.fr, speedRNow);
    m_legsValue.fr.hight = legSet.hight;
    m_legsValue.fr.shift = legSet.shift;
    m_legsValue.fr.lifted = legSet.lifted;
    legSet = _getWalkStep(m_sequenceCounter.rl, speedLNow);
    m_legsValue.rl.hight = legSet.hight;
    m_legsValue.rl.shift = legSet.shift;
    m_legsValue.rl.lifted = legSet.lifted;
    legSet = _getWalkStep(m_sequenceCounter.rr, speedRNow);
    m_legsValue.rr.hight = legSet.hight;
    m_legsValue.rr.shift = legSet.shift;
    m_legsValue.rr.lifted = legSet.lifted;
  } else {
    _setLegsValuesBySide(rbState.legHightNow, 0, rbState.legHightNow, 0);
  }
}

// set robot state
void setStatePattern(unsigned char newState) {
  switch (newState) {
    case ROBOT_NORM:
    {
      rbState.robotStateNow = ROBOT_NORM;
      rbState.legHightNow = HIGHT_DEFAULT;
      rbState.legLiftNow = LEG_LIFT;
      rbState.speedMuliplierNow = 2;
    }
    break;
    case ROBOT_INO:
    {
      rbState.robotStateNow = ROBOT_INO;
      rbState.legHightNow = HIGHT_DEFAULT;
      rbState.legLiftNow = LEG_LIFT * 2;
      rbState.speedMuliplierNow = 1;
    }
    break;
    case ROBOT_CRAWL:
    {
      rbState.robotStateNow = ROBOT_CRAWL;
      rbState.legHightNow = HIGHT_DEFAULT;
      rbState.legLiftNow = LEG_LIFT * 2;
      rbState.speedMuliplierNow = 1;
    }
    break;
    default:
    break;
  }
}

// get speed multilpier
char getspeedMuliplierPattern(void) {
  return rbState.speedMuliplierNow;
}

// get robot state
char getRobotStatePattern(void) {
  return rbState.robotStateNow;
}
