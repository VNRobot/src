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
short shiftValueFL = 0;
short shiftValueFR = 0;
short shiftValueRL = 0;
short shiftValueRR = 0;
// static ballance
// short staticForward = 0;

/*
uses
m_legsValue
m_gyroState
*/

/*
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
    case LEG_LIFTED:
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
*/

// get leg shift forward
short _getLegShiftForward(unsigned char state, unsigned char maxSpeed, short sideSpeed, short shiftForward, short targets) {
  switch (state) {
    case LEG_LIFTED:
    {
      // quick forward
      if (sideSpeed < 0) {
        shiftForward += maxSpeed;
        if (shiftForward > targets) {
          shiftForward = targets;
        }
      } else {
        shiftForward -= maxSpeed;
        if (shiftForward < targets) {
          shiftForward = targets;
        }
      }
    }
    break;
    case LEG_LINEAR:
    case LEG_BEFORE_LIFTING:
    case LEG_LIFTING:
    case LEG_LOWERING:
    case LEG_AFTER_LOWERING:
    {
      // linear  walking shift
      shiftForward += sideSpeed * LINEAR_LEG_SPEED;
      if (shiftForward > STEP_SIZE * SPEED_MAX) {
        shiftForward = STEP_SIZE * SPEED_MAX;
      }
    }
    break;
    default:
    break;
  }
  return shiftForward;
}

// set step limit
short _setStepLimit(short step) {
  if (step > STEP_SIZE * SPEED_MAX) {
    step = STEP_SIZE * SPEED_MAX;
  } else if (step < -STEP_SIZE * SPEED_MAX) {
    step = -STEP_SIZE * SPEED_MAX;
  }
  return step;
}

// get servo motor steps for speed 2 to - 2
void setWalkPatternsShiftCount(void) {
  // disable walking
  if (! shParam.walkEnabled) {
    m_legsValue.fl.speed = 0;
    m_legsValue.fr.speed = 0;
    m_legsValue.rl.speed = 0;
    m_legsValue.rr.speed = 0;
  }
  // maximal speed for lifted leg
  short speedMaxL = LIFTED_LEG_SPEED;
  short speedMaxR = LIFTED_LEG_SPEED;
  // set direction
  //bool goForward = true;
  //if ((m_legsValue.fl.speed < 0) || (m_legsValue.fr.speed < 0)) {
  //  goForward = false;
  //}
  if (m_legsValue.fl.speed < 0) {
    m_legsValue.fl.targets = STEP_SIZE * SPEED_MAX;
    m_legsValue.rl.targets = STEP_SIZE * SPEED_MAX;
    speedMaxL = -LIFTED_LEG_SPEED;
  } else {
    m_legsValue.fl.targets = -STEP_SIZE * SPEED_MAX;
    m_legsValue.rl.targets = -STEP_SIZE * SPEED_MAX;
  }
  if (m_legsValue.fr.speed < 0) {
    m_legsValue.fr.targets = STEP_SIZE * SPEED_MAX;
    m_legsValue.rr.targets = STEP_SIZE * SPEED_MAX;
    speedMaxR = -LIFTED_LEG_SPEED;
  } else {
    m_legsValue.fr.targets = -STEP_SIZE * SPEED_MAX;
    m_legsValue.rr.targets = -STEP_SIZE * SPEED_MAX;
  }
  // find maximal and absolute speed
  /*
  // shift forward to direction of movement
  short speedShift = 0;
  if (goForward) {
    speedShift = SPEED_COMPENSATION;
  } else {
    speedShift = -SPEED_COMPENSATION;
  }
    */
  /*
  // ballance
  if (shParam.ballanceEnabled) {
    short staticForwardTemp = (short)(m_gyroState.aPitchNow * STATIC_BALLANCE_MULIPLIER);
    if ((staticForward > staticForwardTemp) && (staticForward > -STATIC_BALLANCE_MAX)) {
      staticForward --;
    } else if ((staticForward < staticForwardTemp) && (staticForward < STATIC_BALLANCE_MAX)) {
      staticForward ++;
    }
  }
    */
  // final
  // set forward shift
  shiftValueFL = _getLegShiftForward(m_legsValue.fl.state, speedMaxL, m_legsValue.fl.speed, shiftValueFL, m_legsValue.fl.targets);
  shiftValueFR = _getLegShiftForward(m_legsValue.fr.state, speedMaxR, m_legsValue.fr.speed, shiftValueFR, m_legsValue.fr.targets);
  shiftValueRL = _getLegShiftForward(m_legsValue.rl.state, speedMaxL, m_legsValue.rl.speed, shiftValueRL, m_legsValue.rl.targets);
  shiftValueRR = _getLegShiftForward(m_legsValue.rr.state, speedMaxR, m_legsValue.rr.speed, shiftValueRR, m_legsValue.rr.targets);
  /*
  // center mass movement
  short rockForward = 0;
  if (shParam.rockEnabled) {
    rockForward = _getRockForward(m_legsValue.rl.state) + _getRockForward(m_legsValue.rr.state) -_getRockForward(m_legsValue.fl.state) - _getRockForward(m_legsValue.fr.state);
  }
    */
  // final shift
  m_legsValue.fl.shift = _setStepLimit(shiftValueFL); //  + shParam.shiftForward); // staticForward + rockForward + speedShift
  m_legsValue.fr.shift = _setStepLimit(shiftValueFR); //  + shParam.shiftForward); // staticForward + rockForward + speedShift
  m_legsValue.rl.shift = _setStepLimit(shiftValueRL); //  + shParam.shiftForward); // staticForward + rockForward + speedShift
  m_legsValue.rr.shift = _setStepLimit(shiftValueRR); //  + shParam.shiftForward); // staticForward + rockForward + speedShift
}

// init shift 
void initShift(short shiftForward, bool walk, bool ballance, bool rock) {
  shParam.shiftForward = shiftForward;
  shParam.walkEnabled = walk;
  shParam.ballanceEnabled = ballance;
  shParam.rockEnabled = rock;
}
