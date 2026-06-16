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
  unsigned char liftPointNow;
} robotState;

// sequence counters
unsigned char sequenceCounter = 0;
unsigned char sequenceCounterShifted = SERVO_HALF_CYCLE;
// quick shift multiplier
unsigned char quickShiftMultiplier = 3; //(SERVO_HALF_CYCLE - rbState.liftPointNow) / rbState.liftPointNow;

// robot state
robotState rbState = {
  HIGHT_DEFAULT,           // short legHightNow;
  20,                      // short legLiftNow;
  2                        // unsigned char liftPointNow
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

// update servo motors values
unsigned char updatePatternsCount(void) {
  // update main counter
  sequenceCounter ++;
  if (sequenceCounter >= SERVO_FULL_CYCLE) {
    sequenceCounter = 0;
  }
  sequenceCounterShifted = sequenceCounter + SERVO_HALF_CYCLE;
  if (sequenceCounterShifted >= SERVO_FULL_CYCLE) {
    sequenceCounterShifted -= SERVO_FULL_CYCLE;
  }
  return sequenceCounter;
}

// get servo motor steps
void setWalkPatternsCount(bool walkingModeNow, char speedLNow, char speedRNow) {
  leg legSet;
  if (walkingModeNow) {
    // hight
    m_legsValue.fl.hight = rbState.legHightNow;
    m_legsValue.fr.hight = rbState.legHightNow;
    m_legsValue.rl.hight = rbState.legHightNow;
    m_legsValue.rr.hight = rbState.legHightNow;
    // fl and rr
    if (sequenceCounter < rbState.liftPointNow) {
      // start of cycle
      m_legsValue.fl.shift = -sequenceCounter * quickShiftMultiplier;
      m_legsValue.fl.lifted = true;
      m_legsValue.fl.hight -= rbState.legLiftNow; // lifted
      m_legsValue.fr.hight += 2; // one leg touch
    } else if (sequenceCounter > SERVO_FULL_CYCLE - rbState.liftPointNow) {
      // end of cycle
      m_legsValue.fl.shift = (SERVO_FULL_CYCLE - sequenceCounter) * quickShiftMultiplier;
      m_legsValue.fl.lifted = true;
      m_legsValue.fl.hight -= rbState.legLiftNow; // lifted
      m_legsValue.fr.hight += 2; // one leg touch
    } else {
      // linear
      m_legsValue.fl.shift = sequenceCounter - SERVO_HALF_CYCLE;
      m_legsValue.fl.lifted = false;
      // extras
      if (sequenceCounter == rbState.liftPointNow) {
        m_legsValue.fl.hight -= 6; // almost touch
      } else if (sequenceCounter == SERVO_FULL_CYCLE - rbState.liftPointNow) {
        m_legsValue.fl.hight -= rbState.legLiftNow; // start lifting
      }
    }
    // fr and rl
    if (sequenceCounterShifted < rbState.liftPointNow) {
      // start of cycle
      m_legsValue.fr.shift = -sequenceCounterShifted * quickShiftMultiplier;
      m_legsValue.fr.lifted = true;
      m_legsValue.fr.hight -= rbState.legLiftNow; // lifted
      m_legsValue.fl.hight += 2; // one leg touch
    } else if (sequenceCounterShifted > SERVO_FULL_CYCLE - rbState.liftPointNow) {
      // end of cycle
      m_legsValue.fr.shift = (SERVO_FULL_CYCLE - sequenceCounterShifted) * quickShiftMultiplier;
      m_legsValue.fr.lifted = true;
      m_legsValue.fr.hight -= rbState.legLiftNow; // lifted
      m_legsValue.fl.hight += 2; // one leg touch
    } else {
      // linear
      m_legsValue.fr.shift = sequenceCounterShifted - SERVO_HALF_CYCLE;
      m_legsValue.fr.lifted = false;
      // extras
      if (sequenceCounterShifted == rbState.liftPointNow) {
        m_legsValue.fr.hight -= 6; // almost touch
      } else if (sequenceCounterShifted == SERVO_FULL_CYCLE - rbState.liftPointNow) {
        m_legsValue.fr.hight -= rbState.legLiftNow; // start lifting
      }
    }
    // set rear legs values
    m_legsValue.rl.shift = m_legsValue.fr.shift;
    m_legsValue.rl.lifted = m_legsValue.fr.lifted;
    m_legsValue.rl.hight = m_legsValue.fr.hight;
    m_legsValue.rr.shift = m_legsValue.fl.shift;
    m_legsValue.rr.lifted = m_legsValue.fl.lifted;
    m_legsValue.rr.hight = m_legsValue.fl.hight;
    // set speed
    m_legsValue.fl.shift *= speedLNow;
    m_legsValue.fr.shift *= speedRNow;
    m_legsValue.rl.shift *= speedLNow;
    m_legsValue.rr.shift *= speedRNow;
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
      rbState.liftPointNow = 2;
    }
    break;
    case ROBOT_INO:
    {
      rbState.legHightNow = HIGHT_DEFAULT;
      rbState.legLiftNow = 50;
      rbState.liftPointNow = 2;
    }
    break;
    case ROBOT_CRAWL:
    {
      rbState.legHightNow = HIGHT_DEFAULT;
      rbState.legLiftNow = 70;
      rbState.liftPointNow = 2;
    }
    break;
    default:
    break;
  }
  quickShiftMultiplier = (SERVO_HALF_CYCLE - rbState.liftPointNow) / rbState.liftPointNow;
}
