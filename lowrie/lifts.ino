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
#define LEG_LOWERING_SPEED          40
#define LEG_LIFTING_SPEED           20
#define STEP_DOWN_HIGHT             20
#define SIDE_BALLANCE_MAX           20

// leg lifting state structure
typedef struct liftsParameters {
  short legHightNow;
  short legLiftNow;
  bool compensationEnabled;
  bool sideBallanceEnabled;
} liftsParameters;

// robot parameters
liftsParameters liftParam = {
  HIGHT_DEFAULT,            // short legHightNow;
  50,                       // short legLiftNow;
  false,                    // bool compensationEnabled;
  false                     // bool sideBallanceEnabled;
};
/*
// leg hight surface buffers
short surfaceBufferFL = 0;
short surfaceBufferFR = 0;
short surfaceBufferRL = 0;
short surfaceBufferRR = 0;
// leg hight phase buffers
*/
/*
short phaseBufferFL = 0;
short phaseBufferFR = 0;
short phaseBufferRL = 0;
short phaseBufferRR = 0;
*/
/*
// side ballance
short sideBallance = 0;
*/
// flag to delay moving
//bool keepMoving = true;
// leg lifting
short legLiftingFL = 0;
short legLiftingFR = 0;
short legLiftingRL = 0;
short legLiftingRR = 0;

/*
uses
m_legsValue
m_gyroState
*/

/*
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
    case LEG_LIFTED:
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
    case LEG_LIFTED:
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
    case LEG_LIFTED:
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
    case LEG_LIFTED:
    {
      _updateLegsPhaseHight(NEAR_BY_LEG_COMPENSATION, -OPPOSITE_LEG_COMPENSATION, -LIFTING_LEG_COMPENSATION, PAIR_LEG_COMPENSATION);
    }
    break;
    default:
    break;
  }
}
*/

// lowering the leg
short _lowerLeg(short leH, short targetHight, short step) {
  if (leH < targetHight) {
    if (leH < (targetHight - step)) {
      leH += step;
    } else {
      leH = targetHight;
    }
  } else {
    if (leH > (targetHight + step)) {
      leH -= step;
    } else {
      leH = targetHight;
    }
  }
  return leH;
}

// leg lifting
short _setlegLifting(short legLift, unsigned char legState, bool swEnabled, short swValue, short targeth) {
  switch (legState) {
    case LEG_LIFTED:
    case LEG_LIFTING:
    {
      legLift = _lowerLeg(legLift, targeth - liftParam.legLiftNow, LEG_LIFTING_SPEED);
      //legLift = liftParam.legHightNow - liftParam.legLiftNow;
    }
    break;
    case LEG_LOWERING:
    {
      //if ((swValue == 1) || (!swEnabled)) {
      legLift = _lowerLeg(legLift, targeth, LEG_LOWERING_SPEED);
      //  if (legLift != targeth) {
      //    keepMoving = false;
      //  }
      //}
      //legLift = targeth;
    }
    break;
    default:
    break;
  }
  return legLift;
}

// get servo motor steps for speed 3 to - 3
void setLegsLiftCount(quad touchingNow) { //, centers level) {
  //keepMoving = true;
  /*
  if (liftParam.sideBallanceEnabled) {
    //if ((m_legsValue.fl.state == LEG_LINEAR) && (m_legsValue.fr.state == LEG_LINEAR) && (m_legsValue.rl.state == LEG_LINEAR) && (m_legsValue.rr.state == LEG_LINEAR)) {
    if (m_gyroState.aRollNow > 1) {
      if (sideBallance < SIDE_BALLANCE_MAX) {
        sideBallance ++;
      }
    } else if (m_gyroState.aRollNow < -1) {
      if (sideBallance > -SIDE_BALLANCE_MAX) {
        sideBallance --;
      }
    }
    //}
  }
    */

  //if ((m_legsValue.fl.state == LEG_LINEAR) && (m_legsValue.fr.state == LEG_LINEAR) && (m_legsValue.rl.state == LEG_LINEAR) && (m_legsValue.rr.state == LEG_LINEAR)) {
  //  // no lifting
  //} else {
  //  // some lifting
  //}

  // side ballance compensation
  /*
  short sideHightL = 0;
  short sideHightR = 0;
  // side ballance
  if (sideBallance > 0) {
    sideHightL -= sideBallance;
  } else if (sideBallance < 0) {
    sideHightR += sideBallance;
  }
    */
  /*
    // set leg hight phase compensation buffers
  if (liftParam.compensationEnabled) {
    _setLegsPhaseBuffers();
  }
    */
  // leg lifting
  legLiftingFL = _setlegLifting(legLiftingFL, m_legsValue.fl.state, touchingNow.enabledF, touchingNow.fl, m_legsValue.fl.targeth);
  legLiftingFR = _setlegLifting(legLiftingFR, m_legsValue.fr.state, touchingNow.enabledF, touchingNow.fr, m_legsValue.fr.targeth);
  legLiftingRL = _setlegLifting(legLiftingRL, m_legsValue.rl.state, touchingNow.enabledR, touchingNow.rl, m_legsValue.rl.targeth);
  legLiftingRR = _setlegLifting(legLiftingRR, m_legsValue.rr.state, touchingNow.enabledR, touchingNow.rr, m_legsValue.rr.targeth);
  /*
  // recovering front legs
  if (m_legsValue.fl.state == m_legsValue.fr.state) {
    // linear phase
    if (touchingNow.fl == touchingNow.fr) {
      if ((legLiftingFL < 0) && (legLiftingFR < 0)) {
        legLiftingFL ++;
        legLiftingFR ++;
      }
    } else if (touchingNow.fl == 1) {
      if (legLiftingFL < 0) {
        legLiftingFL ++;
      } else if (legLiftingFR > -liftParam.legLiftNow) {
        legLiftingFR --;
      }
    } else if (touchingNow.fr == 1) {
      if (legLiftingFR < 0) {
        legLiftingFR ++;
      } else if (legLiftingFL > -liftParam.legLiftNow) {
        legLiftingFL --;
      }
    }
  }
    */
  /*
  // recovering rear legs
  if (m_legsValue.rl.state == m_legsValue.rr.state) {
    // linear phase
    if (touchingNow.rl == touchingNow.rr) {
      if ((legLiftingRL < 0) && (legLiftingRR < 0)) {
        legLiftingRL ++;
        legLiftingRR ++;
      }
    } else if (touchingNow.rl == 1) {
      if (legLiftingRL < 0) {
        legLiftingRL ++;
      } else if (legLiftingRR > -liftParam.legLiftNow) {
        legLiftingRR --;
      }
    } else if (touchingNow.rr == 1) {
      if (legLiftingRR < 0) {
        legLiftingRR ++;
      } else if (legLiftingRL > -liftParam.legLiftNow) {
        legLiftingRL --;
      }
    }
  }
    */
  /*
  // recover sides
  if ((m_legsValue.fl.state == m_legsValue.fr.state) && (m_legsValue.rl.state == m_legsValue.rr.state)) {
    // left side
    if ((legLiftingFL < legLiftingFR) && (legLiftingRL < legLiftingRR)) {
      legLiftingFL ++;
      legLiftingRL ++;
    }
    // right side
    if ((legLiftingFR < legLiftingFL) && (legLiftingRR < legLiftingRL)) {
      legLiftingFR ++;
      legLiftingRR ++;
    }
  }
    */
  // final
  m_legsValue.fl.hight = legLiftingFL; //  + sideHightL; //  + phaseBufferFL; // + level.front; // + surfaceBufferFL;
  m_legsValue.fr.hight = legLiftingFR; //  + sideHightR; //  + phaseBufferFR; // + level.front; // + surfaceBufferFR;
  m_legsValue.rl.hight = legLiftingRL; //  + sideHightL; //  + phaseBufferRL; // + level.rear; // + surfaceBufferRL;
  m_legsValue.rr.hight = legLiftingRR; //  + sideHightR; //  + phaseBufferRR; // + level.rear; // + surfaceBufferRR;
  //return keepMoving;
}

// init pattern
void initLifts(short legHight, short legLift, bool sideBallance, bool compensation) {
  liftParam.legHightNow = legHight;
  liftParam.legLiftNow = legLift;
  liftParam.compensationEnabled = compensation;
  liftParam.sideBallanceEnabled = sideBallance;
  // set target hight
  m_legsValue.fl.targeth = liftParam.legHightNow;
  m_legsValue.fr.targeth = liftParam.legHightNow;
  m_legsValue.rl.targeth = liftParam.legHightNow;
  m_legsValue.rr.targeth = liftParam.legHightNow;
}
