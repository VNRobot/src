/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Robot legs shift forward motion patterns
*/

// walking compensation parameters
#define ROCK_FORWARD_COMPENSATION   8
#define SPEED_COMPENSATION          8
#define STATIC_BALLANCE_MULIPLIER   4
#define STATIC_BALLANCE_MAX         20

// pattern state structure
typedef struct shiftParam {
  unsigned char legLiftPoint;
  short shiftForward;
  bool walkEnabled;
  bool rockEnabled;
  bool ballanceEnabled;
} shiftParam;

// robot parameters
shiftParam shParam = {
  5,                        // unsigned char legLiftPoint;
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
short _getLegShiftForward(char counter, unsigned char quickShiftMultiplier, short sideSpeed, short shiftForward) {
  // linear  walking shift
  shiftForward += sideSpeed;
  switch (m_legsValue.fl.state) {
    case LEG_LIFTED_BEFORE:
    {
      // quick forwaed
      shiftForward = -counter * quickShiftMultiplier * sideSpeed;
    }
    break;
    case LEG_LIFTED_AFTER:
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
void setWalkPatternsShiftCount(bool walkingModeNow, pair speedNow, bool goForward) {
  // shift forward to direction of movement
  short speedShift = 0;
  // maximal speed for both sides
  short speedMax = 0;
  // quick shift lifted leg forward speed multiplier
  unsigned char quickShiftMultiplier = (mainTiming.halfCycle - shParam.legLiftPoint) / shParam.legLiftPoint;
  // disable walking
  if (! shParam.walkEnabled) {
    speedNow.left = 0;
    speedNow.right = 0;
  }
  // find maximal and absolute speed
  if (goForward) {
    speedShift = SPEED_COMPENSATION;
    if (speedNow.left > speedNow.right) {
      speedMax = speedNow.left;
    } else {
      speedMax = speedNow.right;
    }
  } else {
    speedShift = -SPEED_COMPENSATION;
    if (speedNow.left < speedNow.right) {
      speedMax = speedNow.left;
    } else {
      speedMax = speedNow.right;
    }
  }
  // equal side speed when all legs touching ground
  if ((m_legsValue.fl.state == LEG_LINEAR) && (m_legsValue.fr.state == LEG_LINEAR) && (m_legsValue.rl.state == LEG_LINEAR) && (m_legsValue.rr.state == LEG_LINEAR)) {
    // speed
    speedNow.left = speedMax;
    speedNow.right = speedMax;
    // ballance
    if (shParam.ballanceEnabled) {
      short staticForwardTemp = (short)(m_gyroState.aPitchNow * STATIC_BALLANCE_MULIPLIER);
      if ((staticForward > staticForwardTemp) && (staticForward > -STATIC_BALLANCE_MAX)) {
        staticForward --;
      } else if ((staticForward < staticForwardTemp) && (staticForward < STATIC_BALLANCE_MAX)) {
        staticForward ++;
      }
    }
  }
  if (walkingModeNow) {
    // set forward shift
    shiftBufferFL = _getLegShiftForward(counterFL, quickShiftMultiplier, speedNow.left, shiftBufferFL);
    shiftBufferFR = _getLegShiftForward(counterFR, quickShiftMultiplier, speedNow.right, shiftBufferFR);
    shiftBufferRL = _getLegShiftForward(counterRL, quickShiftMultiplier, speedNow.left, shiftBufferRL);
    shiftBufferRR = _getLegShiftForward(counterRR, quickShiftMultiplier, speedNow.right, shiftBufferRR);
    // center mass movement
    short rockForward = 0;
    if (shParam.rockEnabled) {
      rockForward = _getRockForward(m_legsValue.rl.state) + _getRockForward(m_legsValue.rr.state) -_getRockForward(m_legsValue.fl.state) - _getRockForward(m_legsValue.fr.state);
    }
    // final shift
    m_legsValue.fl.shift = shiftBufferFL + speedShift + shParam.shiftForward + rockForward + staticForward;
    m_legsValue.fr.shift = shiftBufferFR + speedShift + shParam.shiftForward + rockForward + staticForward;
    m_legsValue.rl.shift = shiftBufferRL + speedShift + shParam.shiftForward + rockForward + staticForward;
    m_legsValue.rr.shift = shiftBufferRR + speedShift + shParam.shiftForward + rockForward + staticForward;
  } else {
    m_legsValue.fl.shift = 0;
    m_legsValue.fr.shift = 0;
    m_legsValue.rl.shift = 0;
    m_legsValue.rr.shift = 0;
  }
}

// set lift point
void setLiftPointShift(unsigned char liftPoint) {
  shParam.legLiftPoint = liftPoint;
}

// set forward ballance 
void setForwardShift(short shiftForward) {
  shParam.shiftForward = shiftForward;
}

// enable rock forward
void enableRockShift(bool rock) {
  shParam.rockEnabled = rock;
}

// enable walk forward
void enableWalkShift(bool walk) {
  shParam.walkEnabled = walk;
}

// enable static ballance
void enableBallanceShift(bool ballance) {
  shParam.ballanceEnabled = ballance;
}
