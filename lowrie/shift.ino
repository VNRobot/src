/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Robot legs shift forward motion patterns
*/

// shift forward mm
#define SHIFT_FORWARD                 16
// walking compensation parameters
#define STATIC_BALLANCE_MAX           20
// static ballance multiplier
#define STATIC_BALLANCE_MULIPLIER     4
// lifted leg speed
#define LIFTED_SPEED_MAX              16
// maximal shift
#define WALK_SHIFT_MAX                90

// pattern state structure
typedef struct shiftParam {
  short shiftForward;
  short quickShiftMultiplier;
  bool walkEnabled;
  bool ballanceEnabled;
} shiftParam;

// robot parameters
shiftParam shParam = {
  SHIFT_FORWARD,            // short shiftForward;
  5,                        // short quickShiftMultiplier;
  false,                    // bool walkEnabled;
  false                     // bool ballanceEnabled;
};

// static ballance
short staticForward = 0;
// shift attached flag
bool shiftAttached = false;

/*
uses
m_legsValue
m_gyroState
*/

// shift the leg
short _shiftLeg(short legS, short target, short step) {
  if (legS < target) {
    if (legS < (target - step)) {
      legS += step;
    } else {
      legS = target;
    }
  } else {
    if (legS > (target + step)) {
      legS -= step;
    } else {
      legS = target;
    }
  }
  return legS;
}

// get leg shift forward
short _getLegShiftForward(leg legData, short sideSpeed) {
  switch (legData.state) {
    case LEG_LINEAR:
    case LEG_BEFORE_LIFTING:
    case LEG_LIFTING:
    case LEG_AFTER_LOWERING:
    {
      // linear  walking shift
      legData.shifBuffer += sideSpeed;
      if (legData.shifBuffer > WALK_SHIFT_MAX) {
        legData.shifBuffer = WALK_SHIFT_MAX;
      } else if (legData.shifBuffer < - WALK_SHIFT_MAX) {
        legData.shifBuffer = - WALK_SHIFT_MAX;
      }
    }
    break;
    case LEG_LIFTED:
    {
      // quick forward
      //if (legData.count < mainTiming.halfCycle) {
      //  legData.shifBuffer = -legData.count * shParam.quickShiftMultiplier * sideSpeed;
      //} else {
      //  legData.shifBuffer = (mainTiming.fullCycle - legData.count) * shParam.quickShiftMultiplier * sideSpeed;
      //}
      legData.shifBuffer = _shiftLeg(legData.shifBuffer, legData.shifTarget, LIFTED_SPEED_MAX);
    }
    break;
    case LEG_LOWERING:
    {
      // start point
      //legData.shifBuffer = (legData.count - mainTiming.halfCycle) * sideSpeed;
      legData.shifBuffer = _shiftLeg(legData.shifBuffer, legData.shifTarget, LIFTED_SPEED_MAX);
    }
    break;
    default:
    break;
  }
  return legData.shifBuffer;
}


// attach shift
void attachShift(short quickShiftMultiplier, bool walk, bool ballance) {
  shiftAttached = true;
  shParam.quickShiftMultiplier = quickShiftMultiplier;
  shParam.walkEnabled = walk;
  shParam.ballanceEnabled = ballance;
}

// init shift 
void initShift(void) {
}

// set forward ballance
char setBallanceShiftCount(bool master, char staticBallance) {
  if (shiftAttached) {
    // ballance
    if (master) {
      if (shParam.ballanceEnabled) {
        short staticForwardTemp = (short)(m_gyroState.aPitchNow * STATIC_BALLANCE_MULIPLIER);
        if ((staticForward > staticForwardTemp) && (staticForward > -STATIC_BALLANCE_MAX)) {
          staticForward --;
        } else if ((staticForward < staticForwardTemp) && (staticForward < STATIC_BALLANCE_MAX)) {
          staticForward ++;
        }
      }
    } else {
      staticForward = staticBallance;
    }
  }
  return staticForward;
}

// get servo motor steps for speed 2 to - 2
void setWalkShiftCount(bool walkingModeNow) {
  if (shiftAttached) {
    // disable walking
    if (! shParam.walkEnabled) {
      m_mainData.speedLeft = 0;
      m_mainData.speedRight = 0;
    }
    if (walkingModeNow) {
      // set forward shift
      m_legsValue.fl.shifBuffer = _getLegShiftForward(m_legsValue.fl, m_mainData.speedLeft);
      m_legsValue.fr.shifBuffer = _getLegShiftForward(m_legsValue.fr, m_mainData.speedRight);
      m_legsValue.rl.shifBuffer = _getLegShiftForward(m_legsValue.rl, m_mainData.speedLeft);
      m_legsValue.rr.shifBuffer = _getLegShiftForward(m_legsValue.rr, m_mainData.speedRight);
      // final shift
      m_legsValue.fl.shift = staticForward + m_legsValue.fl.shifBuffer + shParam.shiftForward;
      m_legsValue.fr.shift = staticForward + m_legsValue.fr.shifBuffer + shParam.shiftForward;
      m_legsValue.rl.shift = staticForward + m_legsValue.rl.shifBuffer + shParam.shiftForward;
      m_legsValue.rr.shift = staticForward + m_legsValue.rr.shifBuffer + shParam.shiftForward;
    } else {
      m_legsValue.fl.shift = 0;
      m_legsValue.fr.shift = 0;
      m_legsValue.rl.shift = 0;
      m_legsValue.rr.shift = 0;
    }
  }
}

// set shift forward
void setShiftForward(short shiftForwrd) {
  shParam.shiftForward = SHIFT_FORWARD + shiftForwrd;
}

// quick shift
void setQuickShift(short quickShiftMultiplier) {
  shParam.quickShiftMultiplier = quickShiftMultiplier;
}
