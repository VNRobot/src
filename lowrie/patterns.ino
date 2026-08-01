/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Robot legs hight motion patterns
*/

// walking compensation parameters
#define PAIR_LEG_COMPENSATION       2
#define NEAR_BY_LEG_COMPENSATION    2
#define OPPOSITE_LEG_COMPENSATION   4
#define LIFTING_LEG_COMPENSATION    10
#define LEG_LOWERING_SPEED          5
#define STEP_DOWN_HIGHT             20
#define SIDE_BALLANCE_MAX           20

// pattern state structure
typedef struct patternParam {
  short legHightNow;
  short legLiftNow;
  unsigned char legLiftPoint;
  bool switchEnabled;
  bool compensationEnabled;
  bool sideBallanceEnabled;
} patternParam;

// robot parameters
patternParam patParam = {
  HIGHT_DEFAULT,            // short legHightNow;
  50,                       // short legLiftNow;
  5,                        // unsigned char legLiftPoint;
  false,                    // bool switchEnabled;
  false,                    // bool compensationEnabled;
  false                     // bool sideBallanceEnabled;
};

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
// side ballance
short sideBallance = 0;

/*
uses
m_legsValue
m_gyroState
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
    case LEG_LIFTING:
    case LEG_LOWERING:
    {
      _updateLegsPhaseHight(LIFTING_LEG_COMPENSATION, PAIR_LEG_COMPENSATION, NEAR_BY_LEG_COMPENSATION, -OPPOSITE_LEG_COMPENSATION);
    }
    break;
    case LEG_LIFTED_BEFORE:
    case LEG_LIFTED_AFTER:
    {
      _updateLegsPhaseHight(-LIFTING_LEG_COMPENSATION, PAIR_LEG_COMPENSATION, NEAR_BY_LEG_COMPENSATION, -OPPOSITE_LEG_COMPENSATION);
    }
    break;
    default:
    break;
  }
  // fr
  switch (m_legsValue.fr.state) {
    case LEG_LIFTING:
    case LEG_LOWERING:
    {
      _updateLegsPhaseHight(PAIR_LEG_COMPENSATION, LIFTING_LEG_COMPENSATION, -OPPOSITE_LEG_COMPENSATION, NEAR_BY_LEG_COMPENSATION);
    }
    break;
    case LEG_LIFTED_BEFORE:
    case LEG_LIFTED_AFTER:
    {
      _updateLegsPhaseHight(PAIR_LEG_COMPENSATION, -LIFTING_LEG_COMPENSATION, -OPPOSITE_LEG_COMPENSATION, NEAR_BY_LEG_COMPENSATION);
    }
    break;
    default:
    break;
  }
  // rr
  switch (m_legsValue.rr.state) {
    case LEG_LIFTING:
    case LEG_LOWERING:
    {
      _updateLegsPhaseHight(-OPPOSITE_LEG_COMPENSATION, NEAR_BY_LEG_COMPENSATION, PAIR_LEG_COMPENSATION, LIFTING_LEG_COMPENSATION);
    }
    break;
    case LEG_LIFTED_BEFORE:
    case LEG_LIFTED_AFTER:
    {
      _updateLegsPhaseHight(-OPPOSITE_LEG_COMPENSATION, NEAR_BY_LEG_COMPENSATION, PAIR_LEG_COMPENSATION, -LIFTING_LEG_COMPENSATION);
    }
    break;
    default:
    break;
  }
  // rl
  switch (m_legsValue.rl.state) {
    case LEG_LIFTING:
    case LEG_LOWERING:
    {
      _updateLegsPhaseHight(NEAR_BY_LEG_COMPENSATION, -OPPOSITE_LEG_COMPENSATION, LIFTING_LEG_COMPENSATION, PAIR_LEG_COMPENSATION);
    }
    break;
    case LEG_LIFTED_BEFORE:
    case LEG_LIFTED_AFTER:
    {
      _updateLegsPhaseHight(NEAR_BY_LEG_COMPENSATION, -OPPOSITE_LEG_COMPENSATION, -LIFTING_LEG_COMPENSATION, PAIR_LEG_COMPENSATION);
    }
    break;
    default:
    break;
  }
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

// get servo motor steps for speed 3 to - 3
bool setWalkPatternsLiftCount(bool walkingModeNow, quad touchingNow) {
  // flag to delay counting
  bool keepCounting = true;
  if (patParam.sideBallanceEnabled) {
    if ((m_legsValue.fl.state == LEG_LINEAR) && (m_legsValue.fr.state == LEG_LINEAR) && (m_legsValue.rl.state == LEG_LINEAR) && (m_legsValue.rr.state == LEG_LINEAR)) {
      if (m_gyroState.aRollNow > 1) {
        if (sideBallance < SIDE_BALLANCE_MAX) {
          sideBallance ++;
        }
      } else if (m_gyroState.aRollNow < -1) {
        if (sideBallance > -SIDE_BALLANCE_MAX) {
          sideBallance --;
        }
      }
    }
  }
  if (walkingModeNow) {
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
        surfaceBufferFL = -patParam.legLiftNow;
      }
      break;
      case LEG_LIFTED_BEFORE:
      case LEG_LIFTED_AFTER:
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
        //Serial.print(F(" Wrong pattern state "));
      break;
    }
    // fr
    switch (m_legsValue.fr.state) {
      case LEG_LINEAR:
      break;
      case LEG_LIFTING:
      {
        surfaceBufferFR = -patParam.legLiftNow;
      }
      break;
      case LEG_LIFTED_BEFORE:
      case LEG_LIFTED_AFTER:
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
        //Serial.print(F(" Wrong pattern state "));
      break;
    }
    // rr
    switch (m_legsValue.rr.state) {
      case LEG_LINEAR:
      break;
      case LEG_LIFTING:
      {
        surfaceBufferRR = -patParam.legLiftNow;
      }
      break;
      case LEG_LIFTED_BEFORE:
      case LEG_LIFTED_AFTER:
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
        //Serial.print(F(" Wrong pattern state "));
      break;
    }
    // rl
    switch (m_legsValue.rl.state) {
      case LEG_LINEAR:
      break;
      case LEG_LIFTING:
      {
        surfaceBufferRL = -patParam.legLiftNow;
      }
      break;
      case LEG_LIFTED_BEFORE:
      case LEG_LIFTED_AFTER:
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
        //Serial.print(F(" Wrong pattern state "));
      break;
    }
    m_legsValue.fl.hight = patParam.legHightNow + sideHightFL + phaseBufferFL + surfaceBufferFL;
    m_legsValue.fr.hight = patParam.legHightNow + sideHightFR + phaseBufferFR + surfaceBufferFR;
    m_legsValue.rl.hight = patParam.legHightNow + sideHightRL + phaseBufferRL + surfaceBufferRL;
    m_legsValue.rr.hight = patParam.legHightNow + sideHightRR + phaseBufferRR + surfaceBufferRR;
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

// enable switch
void enableSwtchPatterns(bool sw) {
  patParam.switchEnabled = sw;
}

// enable compensation
void enableCompensationPatterns(bool comp) {
  patParam.compensationEnabled = comp;
}

void enableSideBallancePatterns(bool side) {
  patParam.sideBallanceEnabled = side;
}
