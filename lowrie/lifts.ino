/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Robot legs hight motion patterns
*/

// walking compensation parameters
#define LEG_LIFTING_SPEED           10
#define SIDE_BALLANCE_MAX           20

// pattern state structure
typedef struct patternParam {
  short legHightNow;
  short legLiftNow;
  bool sideBallanceEnabled;
} patternParam;

// robot parameters
patternParam patParam = {
  HIGHT_DEFAULT,            // short legHightNow;
  50,                       // short legLiftNow;
  false                     // bool sideBallanceEnabled;
};

// leg hight surface buffers
short surfaceBufferFL = 0;
short surfaceBufferFR = 0;
short surfaceBufferRL = 0;
short surfaceBufferRR = 0;
// side ballance
short sideBallance = 0;
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
short _setlegLifting(short legLift, unsigned char legState) {
  switch (legState) {
    case LEG_LIFTED_BEFORE:
    case LEG_LIFTED_AFTER:
    case LEG_LIFTING:
    {
      legLift = _lowerLeg(legLift, -patParam.legLiftNow, LEG_LIFTING_SPEED);
    }
    break;
    case LEG_LOWERING:
    {
      legLift = 0;
    }
    break;
    default:
    break;
  }
  return legLift;
}

// get servo motor steps for speed 3 to - 3
void setWalkLiftsCount(bool walkingModeNow) {
  if (patParam.sideBallanceEnabled) {
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
  if (walkingModeNow) {
    // side ballance compensation
    short sideHightL = 0;
    short sideHightR = 0;
    // side ballance
    if (sideBallance > 0) {
      sideHightL -= sideBallance;
    } else if (sideBallance < 0) {
      sideHightR += sideBallance;
    }
    // leg lifting
    legLiftingFL = _setlegLifting(legLiftingFL, m_legsValue.fl.state);
    legLiftingFR = _setlegLifting(legLiftingFR, m_legsValue.fr.state);
    legLiftingRL = _setlegLifting(legLiftingRL, m_legsValue.rl.state);
    legLiftingRR = _setlegLifting(legLiftingRR, m_legsValue.rr.state);
    // final
    m_legsValue.fl.hight = patParam.legHightNow + legLiftingFL + sideHightL;
    m_legsValue.fr.hight = patParam.legHightNow + legLiftingFR + sideHightR;
    m_legsValue.rl.hight = patParam.legHightNow + legLiftingRL + sideHightL;
    m_legsValue.rr.hight = patParam.legHightNow + legLiftingRR + sideHightR;
  } else {
    m_legsValue.fl.hight = patParam.legHightNow;
    m_legsValue.fr.hight = patParam.legHightNow;
    m_legsValue.rl.hight = patParam.legHightNow;
    m_legsValue.rr.hight = patParam.legHightNow;
  }
}

// init pattern
void initLifts(short legHight, short legLift, bool sideBallance) {
  patParam.legHightNow = legHight;
  patParam.legLiftNow = legLift;
  patParam.sideBallanceEnabled = sideBallance;
}
