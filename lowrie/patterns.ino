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
#define ROCK_FORWARD_COMPENSATION  8
#define SPEED_COMPENSATION         8

// pattern state structure
typedef struct patternParam {
  short legHightNow;
  short legLiftNow;
  unsigned char legPairShiftNow;
  unsigned char legLiftPoint;
  short rockValue;
} patternParam;
// structure for timing
typedef struct timing {
  short fullCycle;
  short halfCycle;
  short quarterCycle;
} timing;

// robot parameters
patternParam patParam = {
  HIGHT_DEFAULT,            // short legHightNow;
  50,                       // short legLiftNow;
  16,                       // unsigned char legPairShiftNow;
  4,                        // unsigned char legLiftPoint;
  ROCK_FORWARD_COMPENSATION // short rockValue;
};

// main timing 64 32 16
timing mainTiming = {64, 32, 16};
// sequence counters
char counterFL = 0;
char counterFR = mainTiming.halfCycle;
char counterRR = mainTiming.fullCycle - patParam.legPairShiftNow;
char counterRL = mainTiming.halfCycle - patParam.legPairShiftNow;
// pair shift
char legsPairShift = 0;
// forward flag
bool goForward = true;

/*
uses
m_legsValue
*/

// set same values for legs
void _setLegsValuesBySide (short hightL, short shiftL, short hightR, short shiftR) {
  m_legsValue.fl.hight = hightL;
  m_legsValue.fl.shift = shiftL;
  m_legsValue.fl.state = LEG_LINEAR;
  m_legsValue.fr.hight = hightR;
  m_legsValue.fr.shift = shiftR;
  m_legsValue.fr.state = LEG_LINEAR;
  m_legsValue.rl.hight = hightL;
  m_legsValue.rl.shift = shiftL;
  m_legsValue.rl.state = LEG_LINEAR;
  m_legsValue.rr.hight = hightR;
  m_legsValue.rr.shift = shiftR;
  m_legsValue.rr.state = LEG_LINEAR;
}

// set legs hight fl fr rl rr
void _setLegsHight(short fl, short fr, short rl, short rr) {
  m_legsValue.fl.hight += fl;
  m_legsValue.fr.hight += fr;
  m_legsValue.rl.hight += rl;
  m_legsValue.rr.hight += rr;
}

// get rockforward value. front negative, rear positive
short getRockForward(char counter) {
  short rockForward = 0;
  if (counter < patParam.legLiftPoint) {
    // start of cycle
    rockForward = patParam.rockValue;
  } else if (counter > (mainTiming.fullCycle - patParam.legLiftPoint)) {
    // end of cycle
    rockForward = patParam.rockValue;
  } else {
    // linear walking shift
    if (counter == patParam.legLiftPoint) {
      rockForward = patParam.rockValue;
    } else if (counter == mainTiming.fullCycle - patParam.legLiftPoint) {
      rockForward = patParam.rockValue;
    } else if (counter == patParam.legLiftPoint + 1) {
      // after leg is down
      rockForward = patParam.rockValue / 2;
    } else if (counter == mainTiming.fullCycle - patParam.legLiftPoint - 1) {
      // before leg is lifted
      rockForward = patParam.rockValue / 2;
    }
  }
  return rockForward;
}

// get leg shift forward
short getLegShiftForward(char counter, unsigned char quickShiftMultiplier) {
  // linear  walking shift
  short shiftForward = counter - mainTiming.halfCycle;
  if (counter < patParam.legLiftPoint) {
    // start of cycle
    shiftForward = -counter * quickShiftMultiplier;
  } else if (counter > (mainTiming.fullCycle - patParam.legLiftPoint)) {
    // end of cycle
    shiftForward = (mainTiming.fullCycle - counter) * quickShiftMultiplier;
  }
  return shiftForward;
}

// update servo motors values
unsigned char updatePatternsCount(void) {
  // update main counter
  counterFL ++;
  if (counterFL >= mainTiming.fullCycle) {
    counterFL = 0;
  }
  counterFR = counterFL + mainTiming.halfCycle;
  if (counterFR >= mainTiming.fullCycle) {
    counterFR -= mainTiming.fullCycle;
  }
  // rear pair shift
  if (goForward) {
    legsPairShift = -patParam.legPairShiftNow;
  } else {
    legsPairShift = patParam.legPairShiftNow;
  }
  counterRR = counterFL + legsPairShift;
  if (counterRR >= mainTiming.fullCycle) {
    counterRR -= mainTiming.fullCycle;
  } else if (counterRR < 0) {
    counterRR += mainTiming.fullCycle;
  }
  counterRL = counterRR + mainTiming.halfCycle;
  if (counterRL >= mainTiming.fullCycle) {
    counterRL -= mainTiming.fullCycle;
  }
  return (unsigned char)counterFL;
}

// get servo motor steps for speed 3 to - 3
void setWalkPatternsCount(bool walkingModeNow, pair speedNow, short ballanceShiftForward, short sideBallance, centers level) {
  short speedShift = 0;
  if ((speedNow.left + speedNow.right) < 0) {
    goForward = false;
    speedShift = -SPEED_COMPENSATION;
  } else {
    goForward = true;
    speedShift = SPEED_COMPENSATION;
  }
  // quick shift forward speed multiplier
  unsigned char quickShiftMultiplier = (mainTiming.halfCycle - patParam.legLiftPoint) / patParam.legLiftPoint;
  if (walkingModeNow) {
    // hight
    m_legsValue.fl.hight = patParam.legHightNow + level.front;
    m_legsValue.fr.hight = patParam.legHightNow + level.front;
    m_legsValue.rl.hight = patParam.legHightNow + level.rear;
    m_legsValue.rr.hight = patParam.legHightNow + level.rear;
    // side ballance
    if (sideBallance > 0) {
      m_legsValue.fl.hight -= sideBallance;
      m_legsValue.rl.hight -= sideBallance;
    } else if (sideBallance < 0) {
      m_legsValue.fr.hight += sideBallance;
      m_legsValue.rr.hight += sideBallance;
    }
    // fl
    if (counterFL < patParam.legLiftPoint) {
      // start of cycle
      m_legsValue.fl.state = LEG_LIFTED;
      _setLegsHight( -patParam.legLiftNow, PAIR_LEG_COMPENSATION, NEAR_BY_LEG_COMPENSATION, -OPPOSITE_LEG_COMPENSATION);
    } else if (counterFL > mainTiming.fullCycle - patParam.legLiftPoint) {
      // end of cycle
      m_legsValue.fl.state = LEG_LIFTED;
      _setLegsHight( -patParam.legLiftNow, PAIR_LEG_COMPENSATION, NEAR_BY_LEG_COMPENSATION, -OPPOSITE_LEG_COMPENSATION);
    } else {
      // linear  walking shift
      m_legsValue.fl.state = LEG_LINEAR;
      // extras
      if (counterFL == patParam.legLiftPoint) {
        m_legsValue.fl.state = LEG_LOWERING;
        _setLegsHight( -TOUCHING_LEG_COMPENSATION, PAIR_LEG_COMPENSATION, NEAR_BY_LEG_COMPENSATION, -OPPOSITE_LEG_COMPENSATION);
      } else if (counterFL == mainTiming.fullCycle - patParam.legLiftPoint) {
        m_legsValue.fl.state = LEG_LIFTING;
        _setLegsHight( -LIFTING_LEG_COMPENSATION, PAIR_LEG_COMPENSATION, NEAR_BY_LEG_COMPENSATION, -OPPOSITE_LEG_COMPENSATION);
      }
    }
    // fr
    if (counterFR < patParam.legLiftPoint) {
      // start of cycle
      m_legsValue.fr.state = LEG_LIFTED;
      _setLegsHight(PAIR_LEG_COMPENSATION, -patParam.legLiftNow, -OPPOSITE_LEG_COMPENSATION, NEAR_BY_LEG_COMPENSATION);
    } else if (counterFR > mainTiming.fullCycle - patParam.legLiftPoint) {
      // end of cycle
      m_legsValue.fr.state = LEG_LIFTED;
      _setLegsHight(PAIR_LEG_COMPENSATION, -patParam.legLiftNow, -OPPOSITE_LEG_COMPENSATION, NEAR_BY_LEG_COMPENSATION);
    } else {
      // linear  walking shift
      m_legsValue.fr.state = LEG_LINEAR;
      // extras
      if (counterFR == patParam.legLiftPoint) {
        m_legsValue.fr.state = LEG_LOWERING;
        _setLegsHight(PAIR_LEG_COMPENSATION, -TOUCHING_LEG_COMPENSATION, -OPPOSITE_LEG_COMPENSATION, NEAR_BY_LEG_COMPENSATION);
      } else if (counterFR == mainTiming.fullCycle - patParam.legLiftPoint) {
        m_legsValue.fr.state = LEG_LIFTING;
        _setLegsHight(PAIR_LEG_COMPENSATION, -LIFTING_LEG_COMPENSATION, -OPPOSITE_LEG_COMPENSATION, NEAR_BY_LEG_COMPENSATION);
      }
    }
    // rr
    if (counterRR < patParam.legLiftPoint) {
      // start of cycle
      m_legsValue.rr.state = LEG_LIFTED;
      _setLegsHight( -OPPOSITE_LEG_COMPENSATION, NEAR_BY_LEG_COMPENSATION, PAIR_LEG_COMPENSATION, -patParam.legLiftNow);
    } else if (counterRR > mainTiming.fullCycle - patParam.legLiftPoint) {
      // end of cycle
      m_legsValue.rr.state = LEG_LIFTED;
      _setLegsHight( -OPPOSITE_LEG_COMPENSATION, NEAR_BY_LEG_COMPENSATION, PAIR_LEG_COMPENSATION, -patParam.legLiftNow);
    } else {
      // linear  walking shift
      m_legsValue.rr.state = LEG_LINEAR;
      // extras
      if (counterRR == patParam.legLiftPoint) {
        m_legsValue.rr.state = LEG_LOWERING;
        _setLegsHight( -OPPOSITE_LEG_COMPENSATION, NEAR_BY_LEG_COMPENSATION, PAIR_LEG_COMPENSATION, -TOUCHING_LEG_COMPENSATION);
      } else if (counterRR == mainTiming.fullCycle - patParam.legLiftPoint) {
        m_legsValue.rr.state = LEG_LIFTING;
        _setLegsHight( -OPPOSITE_LEG_COMPENSATION, NEAR_BY_LEG_COMPENSATION, PAIR_LEG_COMPENSATION, -LIFTING_LEG_COMPENSATION);
      }
    }
    // rl
    if (counterRL < patParam.legLiftPoint) {
      // start of cycle
      m_legsValue.rl.state = LEG_LIFTED;
      _setLegsHight(NEAR_BY_LEG_COMPENSATION, -OPPOSITE_LEG_COMPENSATION, -patParam.legLiftNow, PAIR_LEG_COMPENSATION);
    } else if (counterRL > mainTiming.fullCycle - patParam.legLiftPoint) {
      // end of cycle
      m_legsValue.rl.state = LEG_LIFTED;
      _setLegsHight(NEAR_BY_LEG_COMPENSATION, -OPPOSITE_LEG_COMPENSATION, -patParam.legLiftNow, PAIR_LEG_COMPENSATION);
    } else {
      // linear  walking shift
      m_legsValue.rl.state = LEG_LINEAR;
      // extras
      if (counterRL == patParam.legLiftPoint) {
        m_legsValue.rl.state = LEG_LOWERING;
        _setLegsHight(NEAR_BY_LEG_COMPENSATION, -OPPOSITE_LEG_COMPENSATION, -TOUCHING_LEG_COMPENSATION, PAIR_LEG_COMPENSATION);
      } else if (counterRL == mainTiming.fullCycle - patParam.legLiftPoint) {
        m_legsValue.rl.state = LEG_LIFTING;
        _setLegsHight(NEAR_BY_LEG_COMPENSATION, -OPPOSITE_LEG_COMPENSATION, -LIFTING_LEG_COMPENSATION, PAIR_LEG_COMPENSATION);
      }
    }
    // set speed
    //speedNow.left = 0; // ***
    //speedNow.right = 0; // ***
    // set forward shift
    m_legsValue.fl.shift = getLegShiftForward(counterFL, quickShiftMultiplier);
    m_legsValue.fr.shift = getLegShiftForward(counterFR, quickShiftMultiplier);
    m_legsValue.rl.shift = getLegShiftForward(counterRL, quickShiftMultiplier);
    m_legsValue.rr.shift = getLegShiftForward(counterRR, quickShiftMultiplier);
    // set speed and direction
    m_legsValue.fl.shift *= speedNow.left;
    m_legsValue.fr.shift *= speedNow.right;
    m_legsValue.rl.shift *= speedNow.left;
    m_legsValue.rr.shift *= speedNow.right;
    // compensate speed
    m_legsValue.fl.shift += speedShift;
    m_legsValue.fr.shift += speedShift;
    m_legsValue.rl.shift += speedShift;
    m_legsValue.rr.shift += speedShift;
    // ballance part
    m_legsValue.fl.shift += ballanceShiftForward;
    m_legsValue.fr.shift += ballanceShiftForward;
    m_legsValue.rl.shift += ballanceShiftForward;
    m_legsValue.rr.shift += ballanceShiftForward;
    // center mass movement
    short rockForward = -getRockForward(counterFL) - getRockForward(counterFR) + getRockForward(counterRL) + getRockForward(counterRR);
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
void setPatternParameters(short legHight, short legLift, unsigned char liftPoint) {
  patParam.legHightNow = legHight;
  patParam.legLiftNow = legLift;
  patParam.legLiftPoint = liftPoint;
}

// set main cycle value
void setMainCyclePatterns(short mainCycle) {
  mainTiming.fullCycle = mainCycle;
  mainTiming.halfCycle = mainTiming.fullCycle / 2;
  mainTiming.quarterCycle = mainTiming.halfCycle / 2;
  // the same for now
  patParam.legPairShiftNow = mainTiming.quarterCycle;
}

// get main cycle value
short getMainCyclePatterns(void) {
  return mainTiming.fullCycle;
}

// enable rock forward
void enableRockPatterns( bool rock) {
  if (rock) {
    patParam.rockValue = ROCK_FORWARD_COMPENSATION;
  } else {
    patParam.rockValue = 0;
  }
}