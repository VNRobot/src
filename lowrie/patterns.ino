/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Robot legs motion patterns
*/

// walking compensation parameters
#define PAIR_LEG_COMPENSATION       2
#define NEAR_BY_LEG_COMPENSATION    2
#define OPPOSITE_LEG_COMPENSATION   4
#define LIFTING_LEG_COMPENSATION    10
#define ROCK_FORWARD_COMPENSATION   8
#define SPEED_COMPENSATION          8
#define LEG_LOWERING_SPEED          5
#define STEP_DOWN_HIGHT             20

// pattern state structure
typedef struct patternParam {
  short legHightNow;
  short legLiftNow;
  unsigned char legPairShiftNow;
  unsigned char legLiftPoint;
  bool walkEnabled;
  bool rockEnabled;
  bool switchEnabled;
  bool compensationEnabled;
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
  false,                    // bool walkEnabled;
  false,                    // bool rockEnabled;
  false,                    // bool switchEnabled;
  false                     // bool compensationEnabled;
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
// absolute non zero speed for leg lifting
short speedAbsoluteL = 1;
short speedAbsoluteR = 1;
// legs shift buffers
short shiftBufferFL = 0;
short shiftBufferFR = 0;
short shiftBufferRL = 0;
short shiftBufferRR = 0;
// leg hight surface buffers
short surfaceBufferFL = 0;
short surfaceBufferFR = 0;
short surfaceBufferRL = 0;
short surfaceBufferRR = 0;
// leg hight phase buffers
short phaseBufferFL = 0;
short phaseBufferFR = 0;
short phaseBufferRL = 0;
short phaseBufferRR = 0;

/*
uses
m_legsValue
*/

// update legs phase
void _updateLegsPhaseHight(short fl, short fr, short rl, short rr) {
  phaseBufferFL += fl;
  phaseBufferFR += fr;
  phaseBufferRL += rl;
  phaseBufferRR += rr;
}

// set leg hight compensation
void _setLegsPhaseBuffers(void) {
  // reset leg hight phase buffers
  phaseBufferFL = 0;
  phaseBufferFR = 0;
  phaseBufferRL = 0;
  phaseBufferRR = 0;
  // fl
  switch (m_legsValue.fl.state) {
    case LEG_LINEAR:
    break;
    case LEG_LIFTING:
    {
      _updateLegsPhaseHight(LIFTING_LEG_COMPENSATION, PAIR_LEG_COMPENSATION, NEAR_BY_LEG_COMPENSATION, -OPPOSITE_LEG_COMPENSATION);
    }
    break;
    case LEG_LIFTED:
    case LEG_LOWERING:
    {
      _updateLegsPhaseHight(-LIFTING_LEG_COMPENSATION, PAIR_LEG_COMPENSATION, NEAR_BY_LEG_COMPENSATION, -OPPOSITE_LEG_COMPENSATION);
    }
    break;
    default:
    break;
  }
  // fr
  switch (m_legsValue.fr.state) {
    case LEG_LINEAR:
    break;
    case LEG_LIFTING:
    {
      _updateLegsPhaseHight(PAIR_LEG_COMPENSATION, LIFTING_LEG_COMPENSATION, -OPPOSITE_LEG_COMPENSATION, NEAR_BY_LEG_COMPENSATION);
    }
    break;
    case LEG_LIFTED:
    case LEG_LOWERING:
    {
      _updateLegsPhaseHight(PAIR_LEG_COMPENSATION, -LIFTING_LEG_COMPENSATION, -OPPOSITE_LEG_COMPENSATION, NEAR_BY_LEG_COMPENSATION);
    }
    break;
    default:
    break;
  }
  // rr
  switch (m_legsValue.rr.state) {
    case LEG_LINEAR:
    break;
    case LEG_LIFTING:
    {
      _updateLegsPhaseHight(-OPPOSITE_LEG_COMPENSATION, NEAR_BY_LEG_COMPENSATION, PAIR_LEG_COMPENSATION, LIFTING_LEG_COMPENSATION);
    }
    break;
    case LEG_LIFTED:
    case LEG_LOWERING:
    {
      _updateLegsPhaseHight(-OPPOSITE_LEG_COMPENSATION, NEAR_BY_LEG_COMPENSATION, PAIR_LEG_COMPENSATION, -LIFTING_LEG_COMPENSATION);
    }
    break;
    default:
    break;
  }
  // rl
  switch (m_legsValue.rl.state) {
    case LEG_LINEAR:
    break;
    case LEG_LIFTING:
    {
      _updateLegsPhaseHight(NEAR_BY_LEG_COMPENSATION, -OPPOSITE_LEG_COMPENSATION, LIFTING_LEG_COMPENSATION, PAIR_LEG_COMPENSATION);
    }
    break;
    case LEG_LIFTED:
    case LEG_LOWERING:
    {
      _updateLegsPhaseHight(NEAR_BY_LEG_COMPENSATION, -OPPOSITE_LEG_COMPENSATION, -LIFTING_LEG_COMPENSATION, PAIR_LEG_COMPENSATION);
    }
    break;
    default:
    break;
  }
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
    rockForward = ROCK_FORWARD_COMPENSATION;
  } else if (counter > (mainTiming.fullCycle - patParam.legLiftPoint)) {
    // end of cycle
    rockForward = ROCK_FORWARD_COMPENSATION;
  } else {
    // linear walking shift
    if (counter == patParam.legLiftPoint) {
      rockForward = ROCK_FORWARD_COMPENSATION;
    } else if (counter == mainTiming.fullCycle - patParam.legLiftPoint) {
      rockForward = ROCK_FORWARD_COMPENSATION;
    } else if (counter == patParam.legLiftPoint + 1) {
      // after leg is down
      rockForward = ROCK_FORWARD_COMPENSATION / 2;
    } else if (counter == mainTiming.fullCycle - patParam.legLiftPoint - 1) {
      // before leg is lifted
      rockForward = ROCK_FORWARD_COMPENSATION / 2;
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

// lowering the leg
short _lowerLeg(short leH, short targetHight) {
  if (leH < targetHight) {
    if (leH < (targetHight - LEG_LOWERING_SPEED)) {
      leH += LEG_LOWERING_SPEED;
    } else {
      leH = targetHight;
    }
  } else {
    if (leH > (targetHight + LEG_LOWERING_SPEED)) {
      leH -= LEG_LOWERING_SPEED;
    } else {
      leH = targetHight;
    }
  }
  return leH;
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

// set legs state
void setWalkPatternsStateCount(bool walkingModeNow) {
  if (walkingModeNow) {
    // set legs state
    m_legsValue.fl.state = _getLegState(counterFL);
    m_legsValue.fr.state = _getLegState(counterFR);
    m_legsValue.rl.state = _getLegState(counterRL);
    m_legsValue.rr.state = _getLegState(counterRR);
  } else {
    m_legsValue.fl.state = LEG_LINEAR;
    m_legsValue.fr.state = LEG_LINEAR;
    m_legsValue.rl.state = LEG_LINEAR;
    m_legsValue.rr.state = LEG_LINEAR;
  }
}

// get servo motor steps for speed 3 to - 3
void setWalkPatternsShiftCount(bool walkingModeNow, pair speedNow, short ballanceShiftForward) {
  // shift forward to direction of movement
  short speedShift = 0;
  // maximal speed for both sides
  short speedMax = 0;
  // quick shift lifted leg forward speed multiplier
  unsigned char quickShiftMultiplier = (mainTiming.halfCycle - patParam.legLiftPoint) / patParam.legLiftPoint;
  // disable walking
  if (! patParam.walkEnabled) {
    speedNow.left = 0;
    speedNow.right = 0;
  }
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
  if (walkingModeNow) {
    // set speed
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
    short rockForward = 0;
    if (patParam.rockEnabled) {
      rockForward = _getRockForward(counterRL) + _getRockForward(counterRR) -_getRockForward(counterFL) - _getRockForward(counterFR);
    }
    // final shift
    m_legsValue.fl.shift = shiftBufferFL + speedShift + ballanceShiftForward + rockForward;
    m_legsValue.fr.shift = shiftBufferFR + speedShift + ballanceShiftForward + rockForward;
    m_legsValue.rl.shift = shiftBufferRL + speedShift + ballanceShiftForward + rockForward;
    m_legsValue.rr.shift = shiftBufferRR + speedShift + ballanceShiftForward + rockForward;
  } else {
    m_legsValue.fl.shift = 0;
    m_legsValue.fr.shift = 0;
    m_legsValue.rl.shift = 0;
    m_legsValue.rr.shift = 0;
  }
}

// get servo motor steps for speed 3 to - 3
bool setWalkPatternsLiftCount(bool walkingModeNow, short ballanceShiftForward, short sideBallance, quad touchingNow) { //, centers level) {
  // flag to delay counting
  bool keepCounting = true;
  if (walkingModeNow) {
    // compensation hight for shift forward
    char hightCompensation = ballanceShiftForward / 4;
    if (hightCompensation < 0) {
      hightCompensation = -hightCompensation;
    }
    // side ballance compensation
    short sideHightFL = 0;
    short sideHightFR = 0;
    short sideHightRL = 0;
    short sideHightRR = 0;
    // side ballance
    if (sideBallance > 0) {
      sideHightFL -= sideBallance;
      sideHightFR -= sideBallance;
    } else if (sideBallance < 0) {
      sideHightRL += sideBallance;
      sideHightRR += sideBallance;
    }
    // set leg hight phase compensation buffers
    if (patParam.compensationEnabled) {
      _setLegsPhaseBuffers();
    }
    // fl
    switch (m_legsValue.fl.state) {
      case LEG_LINEAR:
      break;
      case LEG_LIFTING:
      {
        surfaceBufferFL = -patParam.legLiftNow * speedAbsoluteL;
      }
      break;
      case LEG_LIFTED:
      {
        surfaceBufferFR = _lowerLeg(surfaceBufferFR, 0);
        //surfaceBufferRL = _lowerLeg(surfaceBufferRL, 0);
        //surfaceBufferRR = _lowerLeg(surfaceBufferRR, 0);
      }
      break;
      case LEG_LOWERING:
      {
        if (patParam.switchEnabled) {
          if (touchingNow.fl == 1) {
            surfaceBufferFL = _lowerLeg(surfaceBufferFL, 0);
            if (surfaceBufferFL != 0) {
              keepCounting = false;
            }
            /*
            } else {
              // lower pair leg
              surfaceBufferFR = _lowerLeg(surfaceBufferFR, -STEP_DOWN_HIGHT);
              if (surfaceBufferFR != -STEP_DOWN_HIGHT) {
                keepCounting = false;
              }
            }
              */
          } else {
            surfaceBufferFL -= LIFTING_LEG_COMPENSATION;
          }
        } else {
          surfaceBufferFL = 0;
        }
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
        surfaceBufferFR = -patParam.legLiftNow * speedAbsoluteR;
      }
      break;
      case LEG_LIFTED:
      {
        surfaceBufferFL = _lowerLeg(surfaceBufferFL, 0);
        //surfaceBufferRL = _lowerLeg(surfaceBufferRL, 0);
        //surfaceBufferRR = _lowerLeg(surfaceBufferRR, 0);
      }
      break;
      case LEG_LOWERING:
      {
        if (patParam.switchEnabled) {
          if (touchingNow.fr == 1) {
            surfaceBufferFR = _lowerLeg(surfaceBufferFR, 0);
            if (surfaceBufferFR != 0) {
              keepCounting = false;
            }
            /*
            } else {
              // lower pair leg
              surfaceBufferFL = _lowerLeg(surfaceBufferFL, -STEP_DOWN_HIGHT);
              if (surfaceBufferFL != -STEP_DOWN_HIGHT) {
                keepCounting = false;
              }
            }
              */
          } else {
            surfaceBufferFR -= LIFTING_LEG_COMPENSATION;
          }
        } else {
          surfaceBufferFR = 0;
        }
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
        surfaceBufferRR = -patParam.legLiftNow * speedAbsoluteR;
      }
      break;
      case LEG_LIFTED:
      {
        surfaceBufferRL = _lowerLeg(surfaceBufferRL, 0);
        //surfaceBufferFL = _lowerLeg(surfaceBufferFL, 0);
        //surfaceBufferFR = _lowerLeg(surfaceBufferFR, 0);
      }
      break;
      case LEG_LOWERING:
      {
        if (patParam.switchEnabled) {
          if (touchingNow.rr == 1) {
            surfaceBufferRR = _lowerLeg(surfaceBufferRR, 0);
            if (surfaceBufferRR != 0) {
              keepCounting = false;
            }
            /*
            } else {
              // lower pair leg
              surfaceBufferRL = _lowerLeg(surfaceBufferRL, -STEP_DOWN_HIGHT);
              if (surfaceBufferRL != -STEP_DOWN_HIGHT) {
                keepCounting = false;
              }
            }
              */
          } else {
            surfaceBufferRR -= LIFTING_LEG_COMPENSATION;
          }
        } else {
          surfaceBufferRR = 0;
        }
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
        surfaceBufferRL = -patParam.legLiftNow * speedAbsoluteL;
      }
      break;
      case LEG_LIFTED:
      {
        surfaceBufferRR = _lowerLeg(surfaceBufferRR, 0);
        //surfaceBufferFL = _lowerLeg(surfaceBufferFL, 0);
        //surfaceBufferFR = _lowerLeg(surfaceBufferFR, 0);
      }
      break;
      case LEG_LOWERING:
      {
        if (patParam.switchEnabled) {
          if (touchingNow.rl == 1) {
            surfaceBufferRL = _lowerLeg(surfaceBufferRL, 0);
            if (surfaceBufferRL != 0) {
              keepCounting = false;
            }
            /*
            } else {
              // lower pair leg
              surfaceBufferRR = _lowerLeg(surfaceBufferRR, -STEP_DOWN_HIGHT);
              if (surfaceBufferRR != -STEP_DOWN_HIGHT) {
                keepCounting = false;
              }
            }
              */
          } else {
            surfaceBufferRL -= LIFTING_LEG_COMPENSATION;
          }
        } else {
          surfaceBufferRL = 0;
        }
      }
      break;
      default:
        Serial.print(F(" Wrong pattern state "));
      break;
    }
    m_legsValue.fl.hight = patParam.legHightNow - hightCompensation + sideHightFL + phaseBufferFL + surfaceBufferFL; // + level.front;
    m_legsValue.fr.hight = patParam.legHightNow - hightCompensation + sideHightFR + phaseBufferFR + surfaceBufferFR; // + level.front;
    m_legsValue.rl.hight = patParam.legHightNow - hightCompensation + sideHightRL + phaseBufferRL + surfaceBufferRL; // + level.rear;
    m_legsValue.rr.hight = patParam.legHightNow - hightCompensation + sideHightRR + phaseBufferRR + surfaceBufferRR; // + level.rear;
  } else {
    m_legsValue.fl.hight = patParam.legHightNow;
    m_legsValue.fr.hight = patParam.legHightNow;
    m_legsValue.rl.hight = patParam.legHightNow;
    m_legsValue.rr.hight = patParam.legHightNow;
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
void enableRockPatterns(bool rock) {
  patParam.rockEnabled = rock;
}

// enable walk forward
void enableWalkPatterns(bool walk) {
  patParam.walkEnabled = walk;
}

// enable switch
void enableSwtchPatterns(bool sw) {
  patParam.switchEnabled = sw;
}

// enable compensation
void enableCompensationPatterns(bool comp) {
  patParam.compensationEnabled = comp;
}
