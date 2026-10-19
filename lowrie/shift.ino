/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Robot legs shift forward motion patterns
*/

// walking compensation parameters
#define STATIC_BALLANCE_MAX         20

// pattern state structure
typedef struct shiftParam {
  short shiftForward;
  bool walkEnabled;
  bool ballanceEnabled;
} shiftParam;

// robot parameters
shiftParam shParam = {
  0,                        // short shiftForward;
  false,                    // bool walkEnabled;
  false                     // bool ballanceEnabled;
};

// legs shift buffers
short shiftBufferFL = 0;
short shiftBufferFR = 0;
short shiftBufferRL = 0;
short shiftBufferRR = 0;
// static ballance
short staticForward = 0;
// shift attached flag
bool shiftAttached = false;

/*
uses
m_legsValue
m_gyroState
*/

// get leg shift forward
short _getLegShiftForward(char counter, char state, unsigned char quickShiftMultiplier, short sideSpeed, short shiftForward) {
  // linear  walking shift
  shiftForward += sideSpeed * LINEAR_SPEED;
  switch (state) {
    case LEG_LIFTED_AFTER:
    {
      // quick forward
      shiftForward = -counter * quickShiftMultiplier * sideSpeed * LINEAR_SPEED;
    }
    break;
    case LEG_LIFTED_BEFORE:
    {
      // quick forward
      shiftForward = (mainTiming.fullCycle - counter) * quickShiftMultiplier * sideSpeed * LINEAR_SPEED;
    }
    break;
    case LEG_LOWERING:
    {
      // start point
      shiftForward = (counter - mainTiming.halfCycle) * sideSpeed * LINEAR_SPEED;
    }
    break;
    default:
    break;
  }
  return shiftForward;
}

// attach shift
void attachShift(short shiftForward, bool walk, bool ballance) {
  shiftAttached = true;
  shParam.shiftForward = shiftForward;
  shParam.walkEnabled = walk;
  shParam.ballanceEnabled = ballance;
}

// set forward ballance
char setBallanceShiftCount(bool master, char staticBallance) {
  if (shiftAttached) {
    // ballance
    if (master) {
      if (shParam.ballanceEnabled) {
        short staticForwardTemp = (short)(m_gyroState.aPitchNow * 4); // STATIC_BALLANCE_MULIPLIER
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
    // quick shift lifted leg forward speed multiplier
    unsigned char quickShiftMultiplier = (mainTiming.halfCycle - LIFT_POINT) / LIFT_POINT;
    // disable walking
    if (! shParam.walkEnabled) {
      m_mainData.speedLeft = 0;
      m_mainData.speedRight = 0;
    }
    if (walkingModeNow) {
      // set forward shift
      shiftBufferFL = _getLegShiftForward(m_legsValue.fl.count, m_legsValue.fl.state, quickShiftMultiplier, m_mainData.speedLeft, shiftBufferFL);
      shiftBufferFR = _getLegShiftForward(m_legsValue.fr.count, m_legsValue.fr.state, quickShiftMultiplier, m_mainData.speedRight, shiftBufferFR);
      shiftBufferRL = _getLegShiftForward(m_legsValue.rl.count, m_legsValue.rl.state, quickShiftMultiplier, m_mainData.speedLeft, shiftBufferRL);
      shiftBufferRR = _getLegShiftForward(m_legsValue.rr.count, m_legsValue.rr.state, quickShiftMultiplier, m_mainData.speedRight, shiftBufferRR);
      // final shift
      m_legsValue.fl.shift = staticForward + shiftBufferFL + shParam.shiftForward;
      m_legsValue.fr.shift = staticForward + shiftBufferFR + shParam.shiftForward;
      m_legsValue.rl.shift = staticForward + shiftBufferRL + shParam.shiftForward;
      m_legsValue.rr.shift = staticForward + shiftBufferRR + shParam.shiftForward;
    } else {
      m_legsValue.fl.shift = 0;
      m_legsValue.fr.shift = 0;
      m_legsValue.rl.shift = 0;
      m_legsValue.rr.shift = 0;
    }
  }
}

// init shift 
void initShift(void) {
}
