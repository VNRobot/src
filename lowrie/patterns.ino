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
  unsigned char pairShiftNow;
} robotState;

// leg timing phase. main m
struct phase {
  unsigned char m;
  unsigned char fl;
  unsigned char fr;
  unsigned char rl;
  unsigned char rr;
};

// walk counters
unsigned char walkCounter = 0;
unsigned char walkForwardCounter = 0;
// leg pair shift
unsigned char pairShift = 0;
// speed dependent pattern
bool speedFlexMode = true;
// sequence counters
phase sequenceCounter = {0, 0, 0, 0, 0};

// robot state
robotState rbState = {
  ROBOT_NORM,              // unsigned char robotStateNow;
  HIGHT_DEFAULT,           // short legHightNow;
  20,                      // short legLiftNow;
  2,                       // char speedMuliplierNow; 1 or 2
  0                        // unsigned char pairShiftNow
};

/*
uses
m_legsValue
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
unsigned char updatePatternsCount(bool forwardNow) {
  // update main counter
  sequenceCounter.m ++;
  walkForwardCounter += rbState.speedMuliplierNow;
  if (sequenceCounter.m >= MAIN_FULL_CYCLE) {
    sequenceCounter.m = 0;
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
  // update pair shift
  if (pairShift < rbState.pairShiftNow) {
    pairShift ++;
  } else if (pairShift > rbState.pairShiftNow) {
    pairShift --;
  }
  // set counters
  sequenceCounter.fl = walkCounter + pairShift;
  sequenceCounter.fr = walkCounter + SERVO_HALF_CYCLE + pairShift;
  sequenceCounter.rl = walkCounter + SERVO_HALF_CYCLE;
  sequenceCounter.rr = walkCounter;
  //
  if (sequenceCounter.fl >= SERVO_FULL_CYCLE) {
    sequenceCounter.fl -= SERVO_FULL_CYCLE;
  }
  if (sequenceCounter.fr >= SERVO_FULL_CYCLE) {
    sequenceCounter.fr -= SERVO_FULL_CYCLE;
  }
  if (sequenceCounter.rl >= SERVO_FULL_CYCLE) {
    sequenceCounter.rl -= SERVO_FULL_CYCLE;
  }
  if (sequenceCounter.rr >= SERVO_FULL_CYCLE) {
    sequenceCounter.rr -= SERVO_FULL_CYCLE;
  }
  return sequenceCounter.m;
}

// get servo motor steps
void setWalkPatternsCount(bool walkingModeNow, char speedLNow, char speedRNow) {
  leg legSet;
  if (walkingModeNow) {
    legSet = _getWalkStep(sequenceCounter.fl, speedLNow);
    m_legsValue.fl.hight = legSet.hight;
    m_legsValue.fl.shift = legSet.shift;
    m_legsValue.fl.lifted = legSet.lifted;
    legSet = _getWalkStep(sequenceCounter.fr, speedRNow);
    m_legsValue.fr.hight = legSet.hight;
    m_legsValue.fr.shift = legSet.shift;
    m_legsValue.fr.lifted = legSet.lifted;
    legSet = _getWalkStep(sequenceCounter.rl, speedLNow);
    m_legsValue.rl.hight = legSet.hight;
    m_legsValue.rl.shift = legSet.shift;
    m_legsValue.rl.lifted = legSet.lifted;
    legSet = _getWalkStep(sequenceCounter.rr, speedRNow);
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
      rbState.legLiftNow = 20;
      rbState.speedMuliplierNow = 2;
      rbState.pairShiftNow = 0;
      speedFlexMode = true;
    }
    break;
    case ROBOT_INO:
    {
      rbState.robotStateNow = ROBOT_INO;
      rbState.legHightNow = HIGHT_DEFAULT;
      rbState.legLiftNow = 40;
      rbState.speedMuliplierNow = 1;
      rbState.pairShiftNow = SERVO_PAIR_SHIFT;
      speedFlexMode = false;
    }
    break;
    case ROBOT_CRAWL:
    {
      rbState.robotStateNow = ROBOT_CRAWL;
      rbState.legHightNow = HIGHT_DEFAULT;
      rbState.legLiftNow = 80;
      rbState.speedMuliplierNow = 1;
      rbState.pairShiftNow = SERVO_PAIR_SHIFT;
      speedFlexMode = false;
    }
    break;
    default:
    break;
  }
}

