/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Robot legs shift forward motion patterns
*/

// walking compensation parameters
#define ROCK_FORWARD_COMPENSATION   8
#define SPEED_COMPENSATION          0
#define STATIC_BALLANCE_MULIPLIER   4
#define STATIC_BALLANCE_MAX         20

// pattern state structure
typedef struct shiftParam {
  short shiftForward;
  bool walkEnabled;
  bool rockEnabled;
  bool ballanceEnabled;
} shiftParam;

// robot parameters
shiftParam shParam = {
  0,                        // short shiftForward;
  false,                    // bool walkEnabled;
  false,                    // bool rockEnabled;
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

// get rockforward value. front negative, rear positive
short _getRockForward(unsigned char legState) {
  short rockForward = 0;
  switch (legState) {
    case LEG_BEFORE_LIFTING:
    case LEG_AFTER_LOWERING:
    {
      rockForward = ROCK_FORWARD_COMPENSATION / 2;
    }
    break;
    case LEG_LIFTING:
    case LEG_LIFTED_BEFORE:
    case LEG_LIFTED_AFTER:
    case LEG_LOWERING:
    {
      rockForward = ROCK_FORWARD_COMPENSATION;
    }
    break;
    default:
    break;
  }
  return rockForward;
}

// get leg shift forward
short _getLegShiftForward(char counter, unsigned char state, unsigned char quickShiftMultiplier, short sideSpeed, short shiftForward) {
  // linear  walking shift
  shiftForward += sideSpeed;
  switch (state) {
    case LEG_LIFTED_AFTER:
    {
      // quick forwaed
      shiftForward = -counter * quickShiftMultiplier * sideSpeed;
    }
    break;
    case LEG_LIFTED_BEFORE:
    {
      // quick forward
      shiftForward = (mainTiming.fullCycle - counter) * quickShiftMultiplier * sideSpeed;
    }
    break;
    case LEG_LOWERING:
    {
      // start point
      shiftForward = (counter - mainTiming.halfCycle) * sideSpeed;
    }
    break;
    default:
    break;
  }
  return shiftForward;
}

// get servo motor steps for speed 3 to - 3
void setWalkPatternsShiftCount(bool walkingModeNow) {
  // shift forward to direction of movement
  short speedShift = 0;
  // maximal speed for both sides
  short speedMax = 0;
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
  // set direction
  bool goForward = true;
  if ((m_legsValue.fl.speed < 0) || (m_legsValue.fr.speed < 0)) {
    goForward = false;
  }
  // find maximal and absolute speed
  if (goForward) {
    speedShift = SPEED_COMPENSATION;
    if (m_legsValue.fl.speed > m_legsValue.fr.speed) {
      speedMax = m_legsValue.fl.speed;
    } else {
      speedMax = m_legsValue.fr.speed;
    }
  } else {
    speedShift = -SPEED_COMPENSATION;
    if (m_legsValue.fl.speed < m_legsValue.fr.speed) {
      speedMax = m_legsValue.fl.speed;
    } else {
      speedMax = m_legsValue.fr.speed;
    }
  }
  // equal side speed when all legs touching ground
  if ((m_legsValue.fl.state == LEG_LINEAR) && (m_legsValue.fr.state == LEG_LINEAR) && (m_legsValue.rl.state == LEG_LINEAR) && (m_legsValue.rr.state == LEG_LINEAR)) {
    // speed
    m_legsValue.fl.speed = speedMax;
    m_legsValue.fr.speed = speedMax;
    m_legsValue.rl.speed = speedMax;
    m_legsValue.rr.speed = speedMax;
  }
  // ballance
  if (shParam.ballanceEnabled) {
    short staticForwardTemp = (short)(m_gyroState.aPitchNow * STATIC_BALLANCE_MULIPLIER);
    if ((staticForward > staticForwardTemp) && (staticForward > -STATIC_BALLANCE_MAX)) {
      staticForward --;
    } else if ((staticForward < staticForwardTemp) && (staticForward < STATIC_BALLANCE_MAX)) {
      staticForward ++;
    }
  }
  if (walkingModeNow) {
    // set forward shift
    shiftBufferFL = _getLegShiftForward(m_legsValue.fl.count, m_legsValue.fl.state, quickShiftMultiplierL, m_legsValue.fl.speed, shiftBufferFL);
    shiftBufferFR = _getLegShiftForward(m_legsValue.fr.count, m_legsValue.fr.state, quickShiftMultiplierR, m_legsValue.fr.speed, shiftBufferFR);
    shiftBufferRL = _getLegShiftForward(m_legsValue.rl.count, m_legsValue.rl.state, quickShiftMultiplierL, m_legsValue.rl.speed, shiftBufferRL);
    shiftBufferRR = _getLegShiftForward(m_legsValue.rr.count, m_legsValue.rr.state, quickShiftMultiplierR, m_legsValue.rr.speed, shiftBufferRR);
    // center mass movement
    short rockForward = 0;
    if (shParam.rockEnabled) {
      rockForward = _getRockForward(m_legsValue.rl.state) + _getRockForward(m_legsValue.rr.state) -_getRockForward(m_legsValue.fl.state) - _getRockForward(m_legsValue.fr.state);
    }
    // final shift
    m_legsValue.fl.shift = staticForward + rockForward + shiftBufferFL + speedShift + shParam.shiftForward;
    m_legsValue.fr.shift = staticForward + rockForward + shiftBufferFR + speedShift + shParam.shiftForward;
    m_legsValue.rl.shift = staticForward + rockForward + shiftBufferRL + speedShift + shParam.shiftForward;
    m_legsValue.rr.shift = staticForward + rockForward + shiftBufferRR + speedShift + shParam.shiftForward;
  } else {
    m_legsValue.fl.shift = 0;
    m_legsValue.fr.shift = 0;
    m_legsValue.rl.shift = 0;
    m_legsValue.rr.shift = 0;
  }
}

// init shift 
void initShift(short shiftForward, bool walk, bool ballance, bool rock) {
  shParam.shiftForward = shiftForward;
  shParam.walkEnabled = walk;
  shParam.ballanceEnabled = ballance;
  shParam.rockEnabled = rock;
}
