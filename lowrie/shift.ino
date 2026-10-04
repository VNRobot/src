/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Robot legs shift forward motion patterns
*/

// walking compensation parameters
#define STATIC_BALLANCE_MULIPLIER   4
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

// set forward ballance
char setBallanceShiftCount(bool master, char staticBallance) {
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
  return staticForward;
}

// get servo motor steps for speed 2 to - 2
void setWalkShiftCount(bool walkingModeNow) {
  // quick shift lifted leg forward speed multiplier
  unsigned char quickShiftMultiplierL = (mainTiming.halfCycle - m_legsValue.fl.liftPoint) / m_legsValue.fl.liftPoint;
  unsigned char quickShiftMultiplierR = (mainTiming.halfCycle - m_legsValue.fr.liftPoint) / m_legsValue.fr.liftPoint;
  // disable walking
  if (! shParam.walkEnabled) {
    m_legsValue.fl.speed = 0;
    m_legsValue.fr.speed = 0;
    m_legsValue.rl.speed = 0;
    m_legsValue.rr.speed = 0;
  }
  if (walkingModeNow) {
    // set forward shift
    shiftBufferFL = _getLegShiftForward(m_legsValue.fl.count, m_legsValue.fl.state, quickShiftMultiplierL, m_legsValue.fl.speed, shiftBufferFL);
    shiftBufferFR = _getLegShiftForward(m_legsValue.fr.count, m_legsValue.fr.state, quickShiftMultiplierR, m_legsValue.fr.speed, shiftBufferFR);
    shiftBufferRL = _getLegShiftForward(m_legsValue.rl.count, m_legsValue.rl.state, quickShiftMultiplierL, m_legsValue.rl.speed, shiftBufferRL);
    shiftBufferRR = _getLegShiftForward(m_legsValue.rr.count, m_legsValue.rr.state, quickShiftMultiplierR, m_legsValue.rr.speed, shiftBufferRR);
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

// init shift 
void initShift(short shiftForward, bool walk, bool ballance) {
  shParam.shiftForward = shiftForward;
  shParam.walkEnabled = walk;
  shParam.ballanceEnabled = ballance;
}
