/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Robot legs shift forward motion patterns
*/

// shift forward mm
#define SHIFT_FORWARD                 16
// linear leg speed mm per count
#define SHIFT_LINEAR_SPEED            2
// walking compensation parameters
#define STATIC_BALLANCE_MAX           20
// static ballance multiplier
#define STATIC_BALLANCE_MULIPLIER     4

// pattern state structure
typedef struct shiftParam {
  short shiftForward;
  short linearSpeed;
  short quickShiftMultiplier;
  bool walkEnabled;
  bool ballanceEnabled;
} shiftParam;

// robot parameters
shiftParam shParam = {
  SHIFT_FORWARD,            // short shiftForward;
  SHIFT_LINEAR_SPEED,       // short linearSpeed;
  5,                        // short quickShiftMultiplier;
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
short _getLegShiftForward(char counter, char state, short sideSpeed, short shiftForward) {
  // linear  walking shift
  shiftForward += sideSpeed * shParam.linearSpeed;
  switch (state) {
    case LEG_LIFTED_AFTER:
    {
      // quick forward
      shiftForward = -counter * shParam.quickShiftMultiplier * sideSpeed * shParam.linearSpeed;
    }
    break;
    case LEG_LIFTED_BEFORE:
    {
      // quick forward
      shiftForward = (mainTiming.fullCycle - counter) * shParam.quickShiftMultiplier * sideSpeed * shParam.linearSpeed;
    }
    break;
    case LEG_LOWERING:
    {
      // start point
      shiftForward = (counter - mainTiming.halfCycle) * sideSpeed * shParam.linearSpeed;
    }
    break;
    default:
    break;
  }
  return shiftForward;
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
      shiftBufferFL = _getLegShiftForward(m_legsValue.fl.count, m_legsValue.fl.state, m_mainData.speedLeft, shiftBufferFL);
      shiftBufferFR = _getLegShiftForward(m_legsValue.fr.count, m_legsValue.fr.state, m_mainData.speedRight, shiftBufferFR);
      shiftBufferRL = _getLegShiftForward(m_legsValue.rl.count, m_legsValue.rl.state, m_mainData.speedLeft, shiftBufferRL);
      shiftBufferRR = _getLegShiftForward(m_legsValue.rr.count, m_legsValue.rr.state, m_mainData.speedRight, shiftBufferRR);
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

// set shift forward
void setShiftForward(short shiftForwrd) {
  shParam.shiftForward = SHIFT_FORWARD + shiftForwrd;
}
