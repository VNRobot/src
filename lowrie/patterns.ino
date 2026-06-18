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
  unsigned char legPairShiftNow;
} robotState;

// sequence counters
char sequenceCounter = 0;
char sequenceRearCounter = 0;
char sequenceCounterShifted = SERVO_HALF_CYCLE;
char sequenceRearCounterShifted = SERVO_HALF_CYCLE;
// quick shift multiplier
unsigned char quickShiftMultiplier = 3; //(SERVO_HALF_CYCLE - rbState.liftPointNow) / rbState.liftPointNow;
// pair shift
char legsPairShift = 0;
// forward flag
bool goForward  = true;

// robot state
robotState rbState = {
  HIGHT_DEFAULT,           // short legHightNow;
  20,                      // short legLiftNow;
  2,                       // unsigned char liftPointNow
  0                        // unsigned char legPairShiftNow;
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
  sequenceRearCounter = sequenceCounter + legsPairShift;
  if (sequenceRearCounter >= SERVO_FULL_CYCLE) {
    sequenceRearCounter -= SERVO_FULL_CYCLE;
  } else if (sequenceRearCounter < 0) {
    sequenceRearCounter += SERVO_FULL_CYCLE;
  }
  sequenceRearCounterShifted = sequenceRearCounter + SERVO_HALF_CYCLE;
  if (sequenceRearCounterShifted >= SERVO_FULL_CYCLE) {
    sequenceRearCounterShifted -= SERVO_FULL_CYCLE;
  }
  // rear pair shift
  if (goForward) {
    if (legsPairShift < -rbState.legPairShiftNow) {
      legsPairShift ++;
    } else if (legsPairShift > -rbState.legPairShiftNow) {
      legsPairShift --;
    }
  } else {
    if (legsPairShift < rbState.legPairShiftNow) {
      legsPairShift ++;
    } else if (legsPairShift > rbState.legPairShiftNow) {
      legsPairShift --;
    }
  }
  return (unsigned char)sequenceCounter;
}

// get servo motor steps
void setWalkPatternsCount(bool walkingModeNow, char speedLNow, char speedRNow) {
  leg legSet;
  if ((speedLNow + speedRNow) < 0) {
    goForward = false;
  } else {
    goForward = true;
  }
  if (walkingModeNow) {
    // hight
    m_legsValue.fl.hight = rbState.legHightNow;
    m_legsValue.fr.hight = rbState.legHightNow;
    // fl
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
    // fr
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
    if (rbState.legPairShiftNow == 0) {
      // set rear legs values by mirroring front legs
      m_legsValue.rl.shift = m_legsValue.fr.shift;
      m_legsValue.rl.lifted = m_legsValue.fr.lifted;
      m_legsValue.rl.hight = m_legsValue.fr.hight;
      m_legsValue.rr.shift = m_legsValue.fl.shift;
      m_legsValue.rr.lifted = m_legsValue.fl.lifted;
      m_legsValue.rr.hight = m_legsValue.fl.hight;
    } else {
      // ino or crawl
      m_legsValue.rl.hight = rbState.legHightNow;
      m_legsValue.rr.hight = rbState.legHightNow;
      // rr
      if (sequenceRearCounter < rbState.liftPointNow) {
        // start of cycle
        m_legsValue.rr.shift = -sequenceRearCounter * quickShiftMultiplier;
        m_legsValue.rr.lifted = true;
        m_legsValue.rr.hight -= rbState.legLiftNow; // lifted
        m_legsValue.rl.hight += 2; // one leg touch
      } else if (sequenceRearCounter > SERVO_FULL_CYCLE - rbState.liftPointNow) {
        // end of cycle
        m_legsValue.rr.shift = (SERVO_FULL_CYCLE - sequenceRearCounter) * quickShiftMultiplier;
        m_legsValue.rr.lifted = true;
        m_legsValue.rr.hight -= rbState.legLiftNow; // lifted
        m_legsValue.rl.hight += 2; // one leg touch
      } else {
        // linear
        m_legsValue.rr.shift = sequenceRearCounter - SERVO_HALF_CYCLE;
        m_legsValue.rr.lifted = false;
        // extras
        if (sequenceRearCounter == rbState.liftPointNow) {
          m_legsValue.rr.hight -= 6; // almost touch
        } else if (sequenceRearCounter == SERVO_FULL_CYCLE - rbState.liftPointNow) {
          m_legsValue.rr.hight -= rbState.legLiftNow; // start lifting
        }
      }
      // rl
      if (sequenceRearCounterShifted < rbState.liftPointNow) {
        // start of cycle
        m_legsValue.rl.shift = -sequenceRearCounterShifted * quickShiftMultiplier;
        m_legsValue.rl.lifted = true;
        m_legsValue.rl.hight -= rbState.legLiftNow; // lifted
        m_legsValue.rr.hight += 2; // one leg touch
      } else if (sequenceRearCounterShifted > SERVO_FULL_CYCLE - rbState.liftPointNow) {
        // end of cycle
        m_legsValue.rl.shift = (SERVO_FULL_CYCLE - sequenceRearCounterShifted) * quickShiftMultiplier;
        m_legsValue.rl.lifted = true;
        m_legsValue.rl.hight -= rbState.legLiftNow; // lifted
        m_legsValue.rr.hight += 2; // one leg touch
      } else {
        // linear
        m_legsValue.rl.shift = sequenceRearCounterShifted - SERVO_HALF_CYCLE;
        m_legsValue.rl.lifted = false;
        // extras
        if (sequenceRearCounterShifted == rbState.liftPointNow) {
          m_legsValue.rl.hight -= 6; // almost touch
        } else if (sequenceRearCounterShifted == SERVO_FULL_CYCLE - rbState.liftPointNow) {
          m_legsValue.rl.hight -= rbState.legLiftNow; // start lifting
        }
      }
    }
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
      rbState.legPairShiftNow = 0;
    }
    break;
    case ROBOT_INO:
    {
      rbState.legHightNow = HIGHT_DEFAULT;
      rbState.legLiftNow = 50;
      rbState.liftPointNow = 2;
      rbState.legPairShiftNow = 4;
    }
    break;
    case ROBOT_CRAWL:
    {
      rbState.legHightNow = HIGHT_DEFAULT;
      rbState.legLiftNow = 70;
      rbState.liftPointNow = 2;
      rbState.legPairShiftNow = 4;
    }
    break;
    default:
    break;
  }
  quickShiftMultiplier = (SERVO_HALF_CYCLE - rbState.liftPointNow) / rbState.liftPointNow;
}
