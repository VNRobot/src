/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Robot legs motion patterns
*/

// robot state structure
typedef struct robotState {
  short legHightNow;
  short legLiftNow;
} robotState;

// leg timing phase. main m
struct phase {
  unsigned char m;
  unsigned char fl;
  unsigned char fr;
  unsigned char rl;
  unsigned char rr;
};

// walk counter
unsigned char walkCounter = 0;
// sequence counters
phase sequenceCounter = {0, 0, 0, 0, 0};

// robot state
robotState rbState = {
  HIGHT_DEFAULT,           // short legHightNow;
  20                       // short legLiftNow;
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
  unsigned char liftPoint = 2;
  if ((speedValue < 4) && (speedValue > -4)) {
    liftPoint = 1;
  }
  // quick shift multiplier
  unsigned char quickShiftMultiplier = (SERVO_HALF_CYCLE - liftPoint) / liftPoint;
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
      legStep.hight = 2; // one leg touch
    } else {
      if (counter == liftPoint) {
        legStep.hight = -6; // touch the ground
      } else {
        legStep.hight = 0; // compensate legs flexibility
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
unsigned char updatePatternsCount(void) {
  // update main counter
  sequenceCounter.m ++;
  if (sequenceCounter.m >= SERVO_FULL_CYCLE) {
    sequenceCounter.m = 0;
  }
  // set counters
  sequenceCounter.fl = sequenceCounter.m;
  sequenceCounter.fr = sequenceCounter.m + SERVO_HALF_CYCLE;
  sequenceCounter.rl = sequenceCounter.m + SERVO_HALF_CYCLE;
  sequenceCounter.rr = sequenceCounter.m;
  //
  //if (sequenceCounter.fl >= SERVO_FULL_CYCLE) {
  //  sequenceCounter.fl -= SERVO_FULL_CYCLE;
  //}
  if (sequenceCounter.fr >= SERVO_FULL_CYCLE) {
    sequenceCounter.fr -= SERVO_FULL_CYCLE;
  }
  if (sequenceCounter.rl >= SERVO_FULL_CYCLE) {
    sequenceCounter.rl -= SERVO_FULL_CYCLE;
  }
  //if (sequenceCounter.rr >= SERVO_FULL_CYCLE) {
  //  sequenceCounter.rr -= SERVO_FULL_CYCLE;
  //}
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
      rbState.legHightNow = HIGHT_DEFAULT;
      rbState.legLiftNow = 30;
    }
    break;
    case ROBOT_INO:
    {
      rbState.legHightNow = HIGHT_DEFAULT;
      rbState.legLiftNow = 50;
    }
    break;
    case ROBOT_CRAWL:
    {
      rbState.legHightNow = HIGHT_DEFAULT;
      rbState.legLiftNow = 70;
    }
    break;
    default:
    break;
  }
}
