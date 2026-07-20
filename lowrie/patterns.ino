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
#define LEG_LOWERING_SPEED        10

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
  16,                       // short legLiftNow;
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
// legs shift buffers
short shiftBufferFL = 0;
short shiftBufferFR = 0;
short shiftBufferRL = 0;
short shiftBufferRR = 0;

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

// update legs hight fl fr rl rr
bool _updateLegsHight(short vfl, short vfr, short vrl, short vrr, short fl, short fr, short rl, short rr) {
  short newfl = vfl + fl;
  short newfr = vfr + fr;
  short newrl = vrl + rl;
  short newrr = vrr + rr;
  bool keepCounting = true;
  if (m_legsValue.fl.state == LEG_LOWERING) {
    if (newfl - m_legsValue.fl.hight > LEG_LOWERING_SPEED) {
      m_legsValue.fl.hight += LEG_LOWERING_SPEED;
      keepCounting = false;
    } else {
      m_legsValue.fl.hight = newfl;
    }
  } else {
    m_legsValue.fl.hight = newfl;
  }
  if (m_legsValue.fr.state == LEG_LOWERING) {
    if (newfr - m_legsValue.fr.hight > LEG_LOWERING_SPEED) {
      m_legsValue.fr.hight += LEG_LOWERING_SPEED;
      keepCounting = false;
    } else {
      m_legsValue.fr.hight = newfr;
    }
  } else {
    m_legsValue.fr.hight = newfr;
  }
  if (m_legsValue.rl.state == LEG_LOWERING) {
    if (newrl - m_legsValue.rl.hight > LEG_LOWERING_SPEED) {
      m_legsValue.rl.hight += LEG_LOWERING_SPEED;
      keepCounting = false;
    } else {
      m_legsValue.rl.hight = newrl;
    }
  } else {
    m_legsValue.rl.hight = newrl;
  }
  if (m_legsValue.rr.state == LEG_LOWERING) {
    if (newrr - m_legsValue.rr.hight > LEG_LOWERING_SPEED) {
      m_legsValue.rr.hight += LEG_LOWERING_SPEED;
      keepCounting = false;
    } else {
      m_legsValue.rr.hight = newrr;
    }
  } else {
    m_legsValue.rr.hight = newrr;
  }
  return keepCounting;
}

// get leg state
unsigned char _getLegState(char counter) {
  unsigned char state = LEG_LINEAR;
  if (counter < patParam.legLiftPoint) {
    // start of cycle
    state = LEG_LIFTED;
  } else if (counter > mainTiming.fullCycle - patParam.legLiftPoint) {
    // end of cycle
    state = LEG_LIFTED;
  } else if (counter == patParam.legLiftPoint) {
    state = LEG_LOWERING;
  } else if (counter == mainTiming.fullCycle - patParam.legLiftPoint) {
    state = LEG_LIFTING;
  }
  return state;
}

// get rockforward value. front negative, rear positive
short _getRockForward(char counter) {
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
short _getLegShiftForward(char counter, unsigned char quickShiftMultiplier, short sideSpeed, short shiftForward) {
  // linear  walking shift
  shiftForward += sideSpeed;
  //shiftForward = (counter - mainTiming.halfCycle) * sideSpeed;
  if (counter < patParam.legLiftPoint) {
    // start of cycle
    shiftForward = -counter * quickShiftMultiplier * sideSpeed;
  } else if (counter > (mainTiming.fullCycle - patParam.legLiftPoint)) {
    // end of cycle
    shiftForward = (mainTiming.fullCycle - counter) * quickShiftMultiplier * sideSpeed;
  } else if (counter == patParam.legLiftPoint) {
    // start of linear
    shiftForward = (counter - mainTiming.halfCycle) * sideSpeed;
  }
  return shiftForward;
}

// update servo motors values
unsigned char updatePatternsCount(bool keepCounting) {
  // update main counter
  if (keepCounting || (counterFL == 0)) {
    counterFL ++;
  }
  if (counterFL >= mainTiming.fullCycle) {
    counterFL = 0;
  }
  counterFR = counterFL + mainTiming.halfCycle;
  if (counterFR >= mainTiming.fullCycle) {
    counterFR -= mainTiming.fullCycle;
  }
  // rear pair shift depends on firection of movement
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
bool setWalkPatternsCount(bool walkingModeNow, pair speedNow, short ballanceShiftForward, short sideBallance) { //, centers level) {
  // flag to delay counting
  bool keepCounting = true;
  // shift forward to direction of movement
  short speedShift = 0;
  // maximal speed for both sides
  short speedMax = 0;
  // absolute non zero speed for leg lifting
  short speedAbsoluteL = 1;
  short speedAbsoluteR = 1;
  // find maximal and absolute speed
  if ((speedNow.left + speedNow.right) < 0) {
    goForward = false;
    speedShift = -SPEED_COMPENSATION;
    if (speedNow.left < speedNow.right) {
      speedMax = speedNow.left;
    } else {
      speedMax = speedNow.right;
    }
    speedAbsoluteL = -speedNow.left + 1;
    speedAbsoluteR = -speedNow.right + 1;
  } else {
    goForward = true;
    speedShift = SPEED_COMPENSATION;
    if (speedNow.left > speedNow.right) {
      speedMax = speedNow.left;
    } else {
      speedMax = speedNow.right;
    }
    speedAbsoluteL = speedNow.left + 1;
    speedAbsoluteR = speedNow.right + 1;
  }
  // quick shift lifted leg forward speed multiplier
  unsigned char quickShiftMultiplier = (mainTiming.halfCycle - patParam.legLiftPoint) / patParam.legLiftPoint;
  if (walkingModeNow) {
    // hight value
    short normalHightFL = patParam.legHightNow; // + level.front;
    short normalHightFR = patParam.legHightNow; // + level.front;
    short normalHightRL = patParam.legHightNow; // + level.rear;
    short normalHightRR = patParam.legHightNow; // + level.rear;
    // side ballance
    if (sideBallance > 0) {
      normalHightFL -= sideBallance;
      normalHightFR -= sideBallance;
    } else if (sideBallance < 0) {
      normalHightRL += sideBallance;
      normalHightRR += sideBallance;
    }
    // compensate hight
    char hightCompensation = ballanceShiftForward / 4;
    if (hightCompensation < 0) {
      hightCompensation = -hightCompensation;
    }
    normalHightFL -= hightCompensation;
    normalHightFR -= hightCompensation;
    normalHightRL -= hightCompensation;
    normalHightRR -= hightCompensation;
    // set legs state
    m_legsValue.fl.state = _getLegState(counterFL);
    m_legsValue.fr.state = _getLegState(counterFR);
    m_legsValue.rl.state = _getLegState(counterRL);
    m_legsValue.rr.state = _getLegState(counterRR);
    // fl
    switch (m_legsValue.fl.state) {
      case LEG_LINEAR:
      break;
      case LEG_LIFTING:
      {
        _updateLegsHight(normalHightFL, normalHightFR, normalHightRL, normalHightRR, -LIFTING_LEG_COMPENSATION, PAIR_LEG_COMPENSATION, NEAR_BY_LEG_COMPENSATION, -OPPOSITE_LEG_COMPENSATION);
      }
      break;
      case LEG_LIFTED:
      {
        _updateLegsHight(normalHightFL, normalHightFR, normalHightRL, normalHightRR, -patParam.legLiftNow * speedAbsoluteL, PAIR_LEG_COMPENSATION, NEAR_BY_LEG_COMPENSATION, -OPPOSITE_LEG_COMPENSATION);
      }
      break;
      case LEG_LOWERING:
      {
        keepCounting = _updateLegsHight(normalHightFL, normalHightFR, normalHightRL, normalHightRR, -TOUCHING_LEG_COMPENSATION, PAIR_LEG_COMPENSATION, NEAR_BY_LEG_COMPENSATION, -OPPOSITE_LEG_COMPENSATION);
      }
      break;
      default:
        Serial.print(F(" Wrong pattern state "));
      break;
    }
    // fr
    switch (m_legsValue.fr.state) {
      case LEG_LINEAR:
      break;
      case LEG_LIFTING:
      {
        _updateLegsHight(normalHightFL, normalHightFR, normalHightRL, normalHightRR, PAIR_LEG_COMPENSATION, -LIFTING_LEG_COMPENSATION, -OPPOSITE_LEG_COMPENSATION, NEAR_BY_LEG_COMPENSATION);
      }
      break;
      case LEG_LIFTED:
      {
        _updateLegsHight(normalHightFL, normalHightFR, normalHightRL, normalHightRR, PAIR_LEG_COMPENSATION, -patParam.legLiftNow * speedAbsoluteR, -OPPOSITE_LEG_COMPENSATION, NEAR_BY_LEG_COMPENSATION);
      }
      break;
      case LEG_LOWERING:
      {
        keepCounting = _updateLegsHight(normalHightFL, normalHightFR, normalHightRL, normalHightRR, PAIR_LEG_COMPENSATION, -TOUCHING_LEG_COMPENSATION, -OPPOSITE_LEG_COMPENSATION, NEAR_BY_LEG_COMPENSATION);
      }
      break;
      default:
        Serial.print(F(" Wrong pattern state "));
      break;
    }
    // rr
    switch (m_legsValue.rr.state) {
      case LEG_LINEAR:
      break;
      case LEG_LIFTING:
      {
        _updateLegsHight(normalHightFL, normalHightFR, normalHightRL, normalHightRR, -OPPOSITE_LEG_COMPENSATION, NEAR_BY_LEG_COMPENSATION, PAIR_LEG_COMPENSATION, -LIFTING_LEG_COMPENSATION);
      }
      break;
      case LEG_LIFTED:
      {
        _updateLegsHight(normalHightFL, normalHightFR, normalHightRL, normalHightRR, -OPPOSITE_LEG_COMPENSATION, NEAR_BY_LEG_COMPENSATION, PAIR_LEG_COMPENSATION, -patParam.legLiftNow * speedAbsoluteR);
      }
      break;
      case LEG_LOWERING:
      {
        keepCounting = _updateLegsHight(normalHightFL, normalHightFR, normalHightRL, normalHightRR, -OPPOSITE_LEG_COMPENSATION, NEAR_BY_LEG_COMPENSATION, PAIR_LEG_COMPENSATION, -TOUCHING_LEG_COMPENSATION);
      }
      break;
      default:
        Serial.print(F(" Wrong pattern state "));
      break;
    }
    // rl
    switch (m_legsValue.rl.state) {
      case LEG_LINEAR:
      break;
      case LEG_LIFTING:
      {
        _updateLegsHight(normalHightFL, normalHightFR, normalHightRL, normalHightRR, NEAR_BY_LEG_COMPENSATION, -OPPOSITE_LEG_COMPENSATION, -LIFTING_LEG_COMPENSATION, PAIR_LEG_COMPENSATION);
      }
      break;
      case LEG_LIFTED:
      {
        _updateLegsHight(normalHightFL, normalHightFR, normalHightRL, normalHightRR, NEAR_BY_LEG_COMPENSATION, -OPPOSITE_LEG_COMPENSATION, -patParam.legLiftNow * speedAbsoluteL, PAIR_LEG_COMPENSATION);
      }
      break;
      case LEG_LOWERING:
      {
        keepCounting = _updateLegsHight(normalHightFL, normalHightFR, normalHightRL, normalHightRR, NEAR_BY_LEG_COMPENSATION, -OPPOSITE_LEG_COMPENSATION, -TOUCHING_LEG_COMPENSATION, PAIR_LEG_COMPENSATION);
      }
      break;
      default:
        Serial.print(F(" Wrong pattern state "));
      break;
    }
    // set speed
    //speedNow.left = 0; // ***
    //speedNow.right = 0; // ***
    // equal side speed when all legs touching ground
    if ((m_legsValue.fl.state != LEG_LIFTED) && (m_legsValue.fr.state != LEG_LIFTED) && (m_legsValue.rl.state != LEG_LIFTED) && (m_legsValue.rr.state != LEG_LIFTED)) {
      speedNow.left = speedMax;
      speedNow.right = speedMax;
    }
    // set forward shift
    shiftBufferFL = _getLegShiftForward(counterFL, quickShiftMultiplier, speedNow.left, shiftBufferFL);
    shiftBufferFR = _getLegShiftForward(counterFR, quickShiftMultiplier, speedNow.right, shiftBufferFR);
    shiftBufferRL = _getLegShiftForward(counterRL, quickShiftMultiplier, speedNow.left, shiftBufferRL);
    shiftBufferRR = _getLegShiftForward(counterRR, quickShiftMultiplier, speedNow.right, shiftBufferRR);
    // center mass movement
    short rockForward = -_getRockForward(counterFL) - _getRockForward(counterFR) + _getRockForward(counterRL) + _getRockForward(counterRR);
    // final shift
    m_legsValue.fl.shift = shiftBufferFL + speedShift + ballanceShiftForward + rockForward;
    m_legsValue.fr.shift = shiftBufferFR + speedShift + ballanceShiftForward + rockForward;
    m_legsValue.rl.shift = shiftBufferRL + speedShift + ballanceShiftForward + rockForward;
    m_legsValue.rr.shift = shiftBufferRR + speedShift + ballanceShiftForward + rockForward;
  } else {
    _setLegsValuesBySide(patParam.legHightNow, 0, patParam.legHightNow, 0);
  }
  return keepCounting;
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