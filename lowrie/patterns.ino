/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Robot legs motion patterns
*/

// ballance correction max value
#define STATIC_BALLANCE_MAX     60

// robot state structure
typedef struct robotState {
  short legHightNow;
  short legLiftNow;
  unsigned char legPairShiftNow;
  unsigned char ballanceMultiplierNow;
} robotState;

// sequence counters
char sequenceCounter = 0;
char sequenceRearCounter = 0;
char sequenceCounterShifted = SERVO_HALF_CYCLE;
char sequenceRearCounterShifted = SERVO_HALF_CYCLE;
// pair shift
char legsPairShift = 0;
// forward flag
bool goForward  = true;
// ballance enabled flag
bool ballanceEnabled = false;
// static forward ballance value
char staticForward = 0;
char staticForwardTemp = 0;

// robot state
robotState rbState = {
  HIGHT_DEFAULT,           // short legHightNow;
  50,                      // short legLiftNow;
  SERVO_QUARTER_CYCLE,     // unsigned char legPairShiftNow;
  4                        // unsigned char ballanceMultiplierNow;
};

/*
uses
m_legsValue
m_gyroState
m_swState
m_swCount
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
  // rear pair shift
  if (goForward) {
    legsPairShift = -rbState.legPairShiftNow;
  } else {
    legsPairShift = rbState.legPairShiftNow;
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
  return (unsigned char)sequenceCounter;
}

// get servo motor steps for speed 6 to - 6
void setWalkPatternsCount(bool walkingModeNow, char speedLNow, char speedRNow) {
  // compensation parameters
  short pairLeg     = 4;    // + 2
  short nearByLeg   = 4;    // + 2
  short oppositeLeg = 6;    // - 4
  short touchLeg    = 6;    // - 6
  short liftLeg    = 20;    // - 50
  short rockForwardValue = 4;
  short rockForward = 0;
  // quick shift forward speed multiplier
  unsigned char quickShiftMultiplier = 3;
  char absoluteSpeed = (speedLNow + speedRNow) / 2; // 0 - 6
  if (absoluteSpeed < 0) {
    goForward = false;
    absoluteSpeed = -absoluteSpeed;
  } else {
    goForward = true;
  }
  // leg lift time point
  unsigned char liftLegPoint = absoluteSpeed / 4 + 1;
  quickShiftMultiplier = (SERVO_HALF_CYCLE - liftLegPoint) / liftLegPoint;
  if (walkingModeNow) {
    // hight
    m_legsValue.fl.hight = rbState.legHightNow;
    m_legsValue.fr.hight = rbState.legHightNow;
    // fl
    if (sequenceCounter < liftLegPoint) {
      // start of cycle
      m_legsValue.fl.shift = -sequenceCounter * quickShiftMultiplier;
      m_legsValue.fl.lifted = true;
      m_legsValue.fl.hight -= rbState.legLiftNow; // lifted
      m_legsValue.fr.hight += pairLeg; // one leg touch
      m_legsValue.rl.hight += nearByLeg; // one leg touch
      m_legsValue.rr.hight -= oppositeLeg; // opposite leg in ino mode
      rockForward = -rockForwardValue;
    } else if (sequenceCounter > SERVO_FULL_CYCLE - liftLegPoint) {
      // end of cycle
      m_legsValue.fl.shift = (SERVO_FULL_CYCLE - sequenceCounter) * quickShiftMultiplier;
      m_legsValue.fl.lifted = true;
      m_legsValue.fl.hight -= rbState.legLiftNow; // lifted
      m_legsValue.fr.hight += pairLeg; // one leg touch
      m_legsValue.rl.hight += nearByLeg; // one leg touch
      m_legsValue.rr.hight -= oppositeLeg; // opposite leg in ino mode
      rockForward = -rockForwardValue;
    } else {
      // linear
      m_legsValue.fl.shift = sequenceCounter - SERVO_HALF_CYCLE;
      m_legsValue.fl.lifted = false;
      // extras
      if (sequenceCounter == liftLegPoint) {
        m_legsValue.fl.hight -= touchLeg; // almost touch
        m_legsValue.fl.lifted = true;
      } else if (sequenceCounter == SERVO_FULL_CYCLE - liftLegPoint) {
        m_legsValue.fl.hight -= liftLeg; // start lifting
        m_legsValue.fr.hight += pairLeg; // one leg touch
        m_legsValue.rl.hight += nearByLeg; // one leg touch
        m_legsValue.rr.hight -= oppositeLeg; // opposite leg in ino mode
        rockForward = -rockForwardValue;
      }
    }
    // fr
    if (sequenceCounterShifted < liftLegPoint) {
      // start of cycle
      m_legsValue.fr.shift = -sequenceCounterShifted * quickShiftMultiplier;
      m_legsValue.fr.lifted = true;
      m_legsValue.fr.hight -= rbState.legLiftNow; // lifted
      m_legsValue.fl.hight += pairLeg; // one leg touch
      m_legsValue.rr.hight += nearByLeg; // one leg touch
      m_legsValue.rl.hight -= oppositeLeg; // opposite leg in ino mode
      rockForward = -rockForwardValue;
    } else if (sequenceCounterShifted > SERVO_FULL_CYCLE - liftLegPoint) {
      // end of cycle
      m_legsValue.fr.shift = (SERVO_FULL_CYCLE - sequenceCounterShifted) * quickShiftMultiplier;
      m_legsValue.fr.lifted = true;
      m_legsValue.fr.hight -= rbState.legLiftNow; // lifted
      m_legsValue.fl.hight += pairLeg; // one leg touch
      m_legsValue.rr.hight += nearByLeg; // one leg touch
      m_legsValue.rl.hight -= oppositeLeg; // opposite leg in ino mode
      rockForward = -rockForwardValue;
    } else {
      // linear
      m_legsValue.fr.shift = sequenceCounterShifted - SERVO_HALF_CYCLE;
      m_legsValue.fr.lifted = false;
      // extras
      if (sequenceCounterShifted == liftLegPoint) {
        m_legsValue.fr.hight -= touchLeg; // almost touch
        m_legsValue.fr.lifted = true;
      } else if (sequenceCounterShifted == SERVO_FULL_CYCLE - liftLegPoint) {
        m_legsValue.fr.hight -= liftLeg; // start lifting
        m_legsValue.fl.hight += pairLeg; // one leg touch
        m_legsValue.rr.hight += nearByLeg; // one leg touch
        m_legsValue.rl.hight -= oppositeLeg; // opposite leg in ino mode
        rockForward = -rockForwardValue;
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
      if (sequenceRearCounter < liftLegPoint) {
        // start of cycle
        m_legsValue.rr.shift = -sequenceRearCounter * quickShiftMultiplier;
        m_legsValue.rr.lifted = true;
        m_legsValue.rr.hight -= rbState.legLiftNow; // lifted
        m_legsValue.rl.hight += pairLeg; // one leg touch
        m_legsValue.fr.hight += nearByLeg; // one leg touch
        m_legsValue.fl.hight -= oppositeLeg; // opposite leg in ino mode
        rockForward = rockForwardValue;
      } else if (sequenceRearCounter > SERVO_FULL_CYCLE - liftLegPoint) {
        // end of cycle
        m_legsValue.rr.shift = (SERVO_FULL_CYCLE - sequenceRearCounter) * quickShiftMultiplier;
        m_legsValue.rr.lifted = true;
        m_legsValue.rr.hight -= rbState.legLiftNow; // lifted
        m_legsValue.rl.hight += pairLeg; // one leg touch
        m_legsValue.fr.hight += nearByLeg; // one leg touch
        m_legsValue.fl.hight -= oppositeLeg; // opposite leg in ino mode
        rockForward = rockForwardValue;
      } else {
        // linear
        m_legsValue.rr.shift = sequenceRearCounter - SERVO_HALF_CYCLE;
        m_legsValue.rr.lifted = false;
        // extras
        if (sequenceRearCounter == liftLegPoint) {
          m_legsValue.rr.hight -= touchLeg; // almost touch
          m_legsValue.rr.lifted = true;
        } else if (sequenceRearCounter == SERVO_FULL_CYCLE - liftLegPoint) {
          m_legsValue.rr.hight -= liftLeg; // start lifting
          m_legsValue.rl.hight += pairLeg; // one leg touch
          m_legsValue.fr.hight += nearByLeg; // one leg touch
          m_legsValue.fl.hight -= oppositeLeg; // opposite leg in ino mode
          rockForward = rockForwardValue;
        }
      }
      // rl
      if (sequenceRearCounterShifted < liftLegPoint) {
        // start of cycle
        m_legsValue.rl.shift = -sequenceRearCounterShifted * quickShiftMultiplier;
        m_legsValue.rl.lifted = true;
        m_legsValue.rl.hight -= rbState.legLiftNow; // lifted
        m_legsValue.rr.hight += pairLeg; // one leg touch
        m_legsValue.fl.hight += nearByLeg; // one leg touch
        m_legsValue.fr.hight -= oppositeLeg; // opposite leg in ino mode
        rockForward = rockForwardValue;
      } else if (sequenceRearCounterShifted > SERVO_FULL_CYCLE - liftLegPoint) {
        // end of cycle
        m_legsValue.rl.shift = (SERVO_FULL_CYCLE - sequenceRearCounterShifted) * quickShiftMultiplier;
        m_legsValue.rl.lifted = true;
        m_legsValue.rl.hight -= rbState.legLiftNow; // lifted
        m_legsValue.rr.hight += pairLeg; // one leg touch
        m_legsValue.fl.hight += nearByLeg; // one leg touch
        m_legsValue.fr.hight -= oppositeLeg; // opposite leg in ino mode
        rockForward = rockForwardValue;
      } else {
        // linear
        m_legsValue.rl.shift = sequenceRearCounterShifted - SERVO_HALF_CYCLE;
        m_legsValue.rl.lifted = false;
        // extras
        if (sequenceRearCounterShifted == liftLegPoint) {
          m_legsValue.rl.hight -= touchLeg; // almost touch
          m_legsValue.rl.lifted = true;
        } else if (sequenceRearCounterShifted == SERVO_FULL_CYCLE - liftLegPoint) {
          m_legsValue.rl.hight -= liftLeg; // start lifting
          m_legsValue.rr.hight += pairLeg; // one leg touch
          m_legsValue.fl.hight += nearByLeg; // one leg touch
          m_legsValue.fr.hight -= oppositeLeg; // opposite leg in ino mode
          rockForward = rockForwardValue;
        }
      }
    }
    // set speed
    //speedLNow = 0; // ***
    //speedRNow = 0; // ***
    m_legsValue.fl.shift *= speedLNow;
    m_legsValue.fr.shift *= speedRNow;
    m_legsValue.rl.shift *= speedLNow;
    m_legsValue.rr.shift *= speedRNow;
    // ballance part
    if (ballanceEnabled) {
      // add forward shift
      staticForwardTemp = (short)(m_gyroState.aPitchNow * rbState.ballanceMultiplierNow); // 4 or 2 has to be tuned
      // 
      if ((staticForward > staticForwardTemp) && (staticForward > -STATIC_BALLANCE_MAX)) {
        staticForward --;
      } else if ((staticForward < staticForwardTemp) && (staticForward < STATIC_BALLANCE_MAX)) {
        staticForward ++;
      }
      staticForward += rockForward;
      m_legsValue.fl.shift += staticForward;
      m_legsValue.fr.shift += staticForward;
      m_legsValue.rl.shift += staticForward;
      m_legsValue.rr.shift += staticForward;
      // compensate hight
      if (staticForward < 0) {
        staticForward = -staticForward;
      }
      staticForward /= 8;
      m_legsValue.fl.hight -= staticForward;
      m_legsValue.fr.hight -= staticForward;
      m_legsValue.rl.hight -= staticForward;
      m_legsValue.rr.hight -= staticForward;
    }
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
      rbState.legLiftNow = 50;
      rbState.legPairShiftNow = SERVO_QUARTER_CYCLE;
      rbState.ballanceMultiplierNow = 4;
    }
    break;
    case ROBOT_INO:
    {
      rbState.legHightNow = HIGHT_DEFAULT;
      rbState.legLiftNow = 50;
      rbState.legPairShiftNow = SERVO_QUARTER_CYCLE;
      rbState.ballanceMultiplierNow = 4;
    }
    break;
    default:
    break;
  }
}

// enable ballance
void enableBallancePatterns(bool ballance) {
  ballanceEnabled = ballance;
}
