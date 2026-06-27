/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Robot legs motion patterns
*/

// walking compensation parameters
#define PAIR_LEG_COMPENSATION     4
#define NEAR_BY_LEG_COMPENSATION  4
#define OPPOSITE_LEG_COMPENSATION 6
#define TOUCHING_LEG_COMPENSATION 6
#define LIFTING_LEG_COMPENSATION  10
#define ROCK_FORWARD_COMPENSATION  4

// pattern state structure
typedef struct patternParam {
  short legHightNow;
  short legLiftNow;
  unsigned char legPairShiftNow;
} patternParam;

// robot parameters
patternParam patParam = {
  HIGHT_DEFAULT,           // short legHightNow;
  20,                      // short legLiftNow;
  0                        // unsigned char legPairShiftNow;
};

// sequence counters
char sequenceCounter = 0;
char sequenceCounterShifted = m_mainTiming.halfCycle;
char sequenceRearCounter = 0;
char sequenceRearCounterShifted = m_mainTiming.halfCycle;
// pair shift
char legsPairShift = 0;
// forward flag
bool goForward  = true;

/*
uses
m_legsValue
m_mainTiming
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

// set legs hight fl fr rl rr
void _setLegsHight(short fl, short fr, short rl, short rr) {
  m_legsValue.fl.hight += fl;
  m_legsValue.fr.hight += fr;
  m_legsValue.rl.hight += rl;
  m_legsValue.rr.hight += rr;
}

// update servo motors values
unsigned char updatePatternsCount(void) {
  // update main counter
  sequenceCounter ++;
  if (sequenceCounter >= m_mainTiming.fullCycle) {
    sequenceCounter = 0;
  }
  sequenceCounterShifted = sequenceCounter + m_mainTiming.halfCycle;
  if (sequenceCounterShifted >= m_mainTiming.fullCycle) {
    sequenceCounterShifted -= m_mainTiming.fullCycle;
  }
  // rear pair shift
  if (goForward) {
    legsPairShift = -patParam.legPairShiftNow;
  } else {
    legsPairShift = patParam.legPairShiftNow;
  }
  sequenceRearCounter = sequenceCounter + legsPairShift;
  if (sequenceRearCounter >= m_mainTiming.fullCycle) {
    sequenceRearCounter -= m_mainTiming.fullCycle;
  } else if (sequenceRearCounter < 0) {
    sequenceRearCounter += m_mainTiming.fullCycle;
  }
  sequenceRearCounterShifted = sequenceRearCounter + m_mainTiming.halfCycle;
  if (sequenceRearCounterShifted >= m_mainTiming.fullCycle) {
    sequenceRearCounterShifted -= m_mainTiming.fullCycle;
  }
  return (unsigned char)sequenceCounter;
}

// get servo motor steps for speed 3 to - 3
void setWalkPatternsCount(bool walkingModeNow, char speedLNow, char speedRNow, short ballanceShiftForward) {
  // center mass movement
  short rockForward = 0;
  // quick shift forward speed multiplier
  unsigned char quickShiftMultiplier = 3;
  char absoluteSpeed = (speedLNow + speedRNow) / 2; // 0 - 3
  if (absoluteSpeed < 0) {
    goForward = false;
    absoluteSpeed = -absoluteSpeed;
  } else {
    goForward = true;
  }
  // leg lift time point
  unsigned char liftLegPoint = ((absoluteSpeed + 1) / 3) * 2 + 2; // 4
  quickShiftMultiplier = (m_mainTiming.halfCycle - liftLegPoint) / liftLegPoint;
  if (walkingModeNow) {
    // hight
    m_legsValue.fl.hight = patParam.legHightNow;
    m_legsValue.fr.hight = patParam.legHightNow;
    // fl
    if (sequenceCounter < liftLegPoint) {
      // start of cycle
      m_legsValue.fl.shift = -sequenceCounter * quickShiftMultiplier;
      m_legsValue.fl.lifted = true;
      _setLegsHight( -patParam.legLiftNow, PAIR_LEG_COMPENSATION, NEAR_BY_LEG_COMPENSATION, -OPPOSITE_LEG_COMPENSATION);
      rockForward = -ROCK_FORWARD_COMPENSATION;
    } else if (sequenceCounter > m_mainTiming.fullCycle - liftLegPoint) {
      // end of cycle
      m_legsValue.fl.shift = (m_mainTiming.fullCycle - sequenceCounter) * quickShiftMultiplier;
      m_legsValue.fl.lifted = true;
      _setLegsHight( -patParam.legLiftNow, PAIR_LEG_COMPENSATION, NEAR_BY_LEG_COMPENSATION, -OPPOSITE_LEG_COMPENSATION);
      rockForward = -ROCK_FORWARD_COMPENSATION;
    } else {
      // linear  walking shift
      m_legsValue.fl.shift = sequenceCounter - m_mainTiming.halfCycle;
      m_legsValue.fl.lifted = false;
      // extras
      if (sequenceCounter == liftLegPoint) {
        //m_legsValue.fl.lifted = false;
        _setLegsHight( -TOUCHING_LEG_COMPENSATION, PAIR_LEG_COMPENSATION, NEAR_BY_LEG_COMPENSATION, -OPPOSITE_LEG_COMPENSATION);
        rockForward = -ROCK_FORWARD_COMPENSATION;
      } else if (sequenceCounter == m_mainTiming.fullCycle - liftLegPoint) {
        _setLegsHight( -LIFTING_LEG_COMPENSATION, PAIR_LEG_COMPENSATION, NEAR_BY_LEG_COMPENSATION, -OPPOSITE_LEG_COMPENSATION);
        rockForward = -ROCK_FORWARD_COMPENSATION;
      } else if (sequenceCounter == liftLegPoint + 1) {
        // after leg is down
        rockForward = -ROCK_FORWARD_COMPENSATION / 2;
      } else if (sequenceCounter == m_mainTiming.fullCycle - liftLegPoint - 1) {
        // before leg is lifted
        rockForward = -ROCK_FORWARD_COMPENSATION / 2;
      }
    }
    // fr
    if (sequenceCounterShifted < liftLegPoint) {
      // start of cycle
      m_legsValue.fr.shift = -sequenceCounterShifted * quickShiftMultiplier;
      m_legsValue.fr.lifted = true;
      _setLegsHight(PAIR_LEG_COMPENSATION, -patParam.legLiftNow, -OPPOSITE_LEG_COMPENSATION, NEAR_BY_LEG_COMPENSATION);
      rockForward = -ROCK_FORWARD_COMPENSATION;
    } else if (sequenceCounterShifted > m_mainTiming.fullCycle - liftLegPoint) {
      // end of cycle
      m_legsValue.fr.shift = (m_mainTiming.fullCycle - sequenceCounterShifted) * quickShiftMultiplier;
      m_legsValue.fr.lifted = true;
      _setLegsHight(PAIR_LEG_COMPENSATION, -patParam.legLiftNow, -OPPOSITE_LEG_COMPENSATION, NEAR_BY_LEG_COMPENSATION);
      rockForward = -ROCK_FORWARD_COMPENSATION;
    } else {
      // linear  walking shift
      m_legsValue.fr.shift = sequenceCounterShifted - m_mainTiming.halfCycle;
      m_legsValue.fr.lifted = false;
      // extras
      if (sequenceCounterShifted == liftLegPoint) {
        //m_legsValue.fr.lifted = false;
        _setLegsHight(PAIR_LEG_COMPENSATION, -TOUCHING_LEG_COMPENSATION, -OPPOSITE_LEG_COMPENSATION, NEAR_BY_LEG_COMPENSATION);
        rockForward = -ROCK_FORWARD_COMPENSATION;
      } else if (sequenceCounterShifted == m_mainTiming.fullCycle - liftLegPoint) {
        _setLegsHight(PAIR_LEG_COMPENSATION, -LIFTING_LEG_COMPENSATION, -OPPOSITE_LEG_COMPENSATION, NEAR_BY_LEG_COMPENSATION);
        rockForward = -ROCK_FORWARD_COMPENSATION;
      } else if (sequenceCounterShifted == liftLegPoint + 1) {
        // after leg is down
        rockForward = -ROCK_FORWARD_COMPENSATION / 2;
      } else if (sequenceCounterShifted == m_mainTiming.fullCycle - liftLegPoint - 1) {
        // before leg is lifted
        rockForward = -ROCK_FORWARD_COMPENSATION / 2;
      }
    }
    // ino
    m_legsValue.rl.hight = patParam.legHightNow;
    m_legsValue.rr.hight = patParam.legHightNow;
    // rr
    if (sequenceRearCounter < liftLegPoint) {
      // start of cycle
      m_legsValue.rr.shift = -sequenceRearCounter * quickShiftMultiplier;
      m_legsValue.rr.lifted = true;
      _setLegsHight( -OPPOSITE_LEG_COMPENSATION, NEAR_BY_LEG_COMPENSATION, PAIR_LEG_COMPENSATION, -patParam.legLiftNow);
      rockForward = ROCK_FORWARD_COMPENSATION;
    } else if (sequenceRearCounter > m_mainTiming.fullCycle - liftLegPoint) {
      // end of cycle
      m_legsValue.rr.shift = (m_mainTiming.fullCycle - sequenceRearCounter) * quickShiftMultiplier;
      m_legsValue.rr.lifted = true;
      _setLegsHight( -OPPOSITE_LEG_COMPENSATION, NEAR_BY_LEG_COMPENSATION, PAIR_LEG_COMPENSATION, -patParam.legLiftNow);
      rockForward = ROCK_FORWARD_COMPENSATION;
    } else {
      // linear  walking shift
      m_legsValue.rr.shift = sequenceRearCounter - m_mainTiming.halfCycle;
      m_legsValue.rr.lifted = false;
      // extras
      if (sequenceRearCounter == liftLegPoint) {
        //m_legsValue.rr.lifted = false;
        _setLegsHight( -OPPOSITE_LEG_COMPENSATION, NEAR_BY_LEG_COMPENSATION, PAIR_LEG_COMPENSATION, -TOUCHING_LEG_COMPENSATION);
        rockForward = ROCK_FORWARD_COMPENSATION;
      } else if (sequenceRearCounter == m_mainTiming.fullCycle - liftLegPoint) {
        _setLegsHight( -OPPOSITE_LEG_COMPENSATION, NEAR_BY_LEG_COMPENSATION, PAIR_LEG_COMPENSATION, -LIFTING_LEG_COMPENSATION);
        rockForward = ROCK_FORWARD_COMPENSATION;
      } else if (sequenceRearCounter == liftLegPoint + 1) {
        // after leg is down
        rockForward = ROCK_FORWARD_COMPENSATION / 2;
      } else if (sequenceRearCounter == m_mainTiming.fullCycle - liftLegPoint - 1) {
        // before leg is lifted
        rockForward = ROCK_FORWARD_COMPENSATION / 2;
      }
    }
    // rl
    if (sequenceRearCounterShifted < liftLegPoint) {
      // start of cycle
      m_legsValue.rl.shift = -sequenceRearCounterShifted * quickShiftMultiplier;
      m_legsValue.rl.lifted = true;
      _setLegsHight(NEAR_BY_LEG_COMPENSATION, -OPPOSITE_LEG_COMPENSATION, -patParam.legLiftNow, PAIR_LEG_COMPENSATION);
      rockForward = ROCK_FORWARD_COMPENSATION;
    } else if (sequenceRearCounterShifted > m_mainTiming.fullCycle - liftLegPoint) {
      // end of cycle
      m_legsValue.rl.shift = (m_mainTiming.fullCycle - sequenceRearCounterShifted) * quickShiftMultiplier;
      m_legsValue.rl.lifted = true;
      _setLegsHight(NEAR_BY_LEG_COMPENSATION, -OPPOSITE_LEG_COMPENSATION, -patParam.legLiftNow, PAIR_LEG_COMPENSATION);
      rockForward = ROCK_FORWARD_COMPENSATION;
    } else {
      // linear  walking shift
      m_legsValue.rl.shift = sequenceRearCounterShifted - m_mainTiming.halfCycle;
      m_legsValue.rl.lifted = false;
      // extras
      if (sequenceRearCounterShifted == liftLegPoint) {
        //m_legsValue.rl.lifted = false;
        _setLegsHight(NEAR_BY_LEG_COMPENSATION, -OPPOSITE_LEG_COMPENSATION, -TOUCHING_LEG_COMPENSATION, PAIR_LEG_COMPENSATION);
        rockForward = ROCK_FORWARD_COMPENSATION;
      } else if (sequenceRearCounterShifted == m_mainTiming.fullCycle - liftLegPoint) {
        _setLegsHight(NEAR_BY_LEG_COMPENSATION, -OPPOSITE_LEG_COMPENSATION, -LIFTING_LEG_COMPENSATION, PAIR_LEG_COMPENSATION);
        rockForward = ROCK_FORWARD_COMPENSATION;
      } else if (sequenceRearCounterShifted == liftLegPoint + 1) {
        // after leg is down
        rockForward = ROCK_FORWARD_COMPENSATION / 2;
      } else if (sequenceRearCounterShifted == m_mainTiming.fullCycle - liftLegPoint - 1) {
        // before leg is lifted
        rockForward = ROCK_FORWARD_COMPENSATION / 2;
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
    m_legsValue.fl.shift += ballanceShiftForward;
    m_legsValue.fr.shift += ballanceShiftForward;
    m_legsValue.rl.shift += ballanceShiftForward;
    m_legsValue.rr.shift += ballanceShiftForward;
    m_legsValue.fl.shift += rockForward;
    m_legsValue.fr.shift += rockForward;
    m_legsValue.rl.shift += rockForward;
    m_legsValue.rr.shift += rockForward;
    // compensate hight
    char hightCompensation = ballanceShiftForward / 4;
    if (hightCompensation < 0) {
      hightCompensation = -hightCompensation;
    }
    m_legsValue.fl.hight -= hightCompensation;
    m_legsValue.fr.hight -= hightCompensation;
    m_legsValue.rl.hight -= hightCompensation;
    m_legsValue.rr.hight -= hightCompensation;
  } else {
    _setLegsValuesBySide(patParam.legHightNow, 0, patParam.legHightNow, 0);
  }
}

// set pattern parameters
void setPatternParameters(short legHight, short legLift, unsigned char legPairShift) {
  patParam.legHightNow = legHight;
  patParam.legLiftNow = legLift;
  patParam.legPairShiftNow = legPairShift;
}
