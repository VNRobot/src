/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Robot legs hight motion patterns
*/

// lift value mm
#define LIFT_DEFAULT                50
// walking compensation parameters
#define LEG_LIFTING_SPEED           10
#define LEG_LOWERING_SPEED          20
#define SIDE_BALLANCE_MAX           20

// pattern state structure
typedef struct patternParam {
  short legHightNow;
  short legLiftNow;
  bool sideBallanceEnabled;
} patternParam;

// robot parameters
patternParam patParam = {
  120,                      // short legHightNow;
  LIFT_DEFAULT,             // short legLiftNow;
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
// lifts attached flag
bool liftsAttached = false;

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
short _setlegLifting(short legLift, char legState) {
  switch (legState) {
    case LEG_LIFTED_BEFORE:
    case LEG_LIFTED_AFTER:
    case LEG_LIFTING:
    {
      legLift = _lowerLeg(legLift, -patParam.legLiftNow, LEG_LIFTING_SPEED);
    }
    break;
    case LEG_LINEAR:
    case LEG_LOWERING:
    case LEG_AFTER_LOWERING:
    {
      legLift = _lowerLeg(legLift, 0, LEG_LOWERING_SPEED);
    }
    break;
    default:
    break;
  }
  return legLift;
}

// attach lifts
void attachLifts(short legHight, bool sideBallance) {
  liftsAttached = true;
  patParam.legHightNow = legHight;
  patParam.sideBallanceEnabled = sideBallance;
}

// init lifts
void initLifts(void) {
}

// get servo motor steps for speed 3 to - 3
void setWalkLiftsCount(bool walkingModeNow, short angleCenter) {
  if (liftsAttached) {
    angleCenter -= angleCenter / 3;
    if (patParam.sideBallanceEnabled) {
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
        sideHightL = -sideBallance;
      } else if (sideBallance < 0) {
        sideHightR = sideBallance;
      }
      // leg lifting
      legLiftingFL = _setlegLifting(legLiftingFL, m_legsValue.fl.state);
      legLiftingFR = _setlegLifting(legLiftingFR, m_legsValue.fr.state);
      legLiftingRL = _setlegLifting(legLiftingRL, m_legsValue.rl.state);
      legLiftingRR = _setlegLifting(legLiftingRR, m_legsValue.rr.state);
      // final
      m_legsValue.fl.hight = patParam.legHightNow + legLiftingFL + sideHightL - angleCenter;
      m_legsValue.fr.hight = patParam.legHightNow + legLiftingFR + sideHightR - angleCenter;
      m_legsValue.rl.hight = patParam.legHightNow + legLiftingRL + sideHightL + angleCenter;
      m_legsValue.rr.hight = patParam.legHightNow + legLiftingRR + sideHightR + angleCenter;
    } else {
      m_legsValue.fl.hight = patParam.legHightNow - angleCenter;
      m_legsValue.fr.hight = patParam.legHightNow - angleCenter;
      m_legsValue.rl.hight = patParam.legHightNow + angleCenter;
      m_legsValue.rr.hight = patParam.legHightNow + angleCenter;
    }
  }
}
