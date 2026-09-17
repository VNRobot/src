/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Robot legs phase counters
*/

// structure for timing
typedef struct timing {
  short fullCycle;
  short halfCycle;
  short quarterCycle;
} timing;

// main timing 64 32 16
timing mainTiming = {32, 16, 8};
// main counter
unsigned char mainCounter = 0;
// count enable flag
bool keepCounting = false;

/*
uses
m_legsValue
*/

unsigned char _processLegState(unsigned char inState, short shift, short hight, short targeth, short targets) {
  unsigned char outState = inState;
  switch (inState) {
    case LEG_BEFORE_LIFTING:
    {
      outState = LEG_LIFTING;
    }
    break;
    case LEG_LIFTING:
    {
      outState = LEG_LIFTED;
    }
    break;
    case LEG_LIFTED:
    {
      if (shift == targets) {
        outState = LEG_LOWERING;
      }
    }
    break;
    case LEG_LOWERING:
    {
      if (hight == targeth) {
        outState = LEG_AFTER_LOWERING;
      }
    }
    break;
    case LEG_AFTER_LOWERING:
    {
      outState = LEG_LINEAR;
    }
    break;
    default:
    break;
  }
  return outState;
}

// check leg location
unsigned char _getLegLocation(short theShift, short otherShift, short stepSize) {
  unsigned char legLocation = WRONG_LEG;
  if (theShift > otherShift) {
    // rear
    if ((theShift - otherShift) > stepSize) {
      // all good
      legLocation = REAR_LEG;
    }
  } else {
    // front
    if ((otherShift - theShift) > stepSize) {
      // all good
      legLocation = FRONT_LEG;
    }
  }
  return legLocation;
}

// check center mass related lifting flag
bool _getLiftingCandidate(short oppositeOne, short oppositeTwo, short nearBy, short margine) {
  short centerOne = (oppositeOne + nearBy) / 2;
  short centerTwo = (oppositeTwo + nearBy) / 2;
  if ((centerOne > margine) && (centerTwo < -margine)) {
    return true;
  }
  if ((centerTwo > margine) && (centerOne < -margine)) {
    return true;
  }
  return false;
}

// check leg lifting flag
bool _getLegLiftingFlag(short theShift, unsigned char theLocation, char theSpeed) {
  if ((theSpeed >= 0) && (theLocation == REAR_LEG) && (theShift > STEP_SIZE)) {
    return true;
  }
  if ((theSpeed < 0) && (theLocation == FRONT_LEG) && (theShift < -STEP_SIZE)) {
    return true;
  }
  if (theLocation == WRONG_LEG) {
    return true;
  }
  return false;
}

// update servo motors values
unsigned char updateCounter(void) {
  // update main counter
  if (keepCounting) {
    mainCounter ++;
  }
  if (mainCounter >= mainTiming.fullCycle) {
    mainCounter = 0;
  }
  return mainCounter;
}

// update servo motors values
bool updateLegsCounter(void) {
  // side linear
  bool linearLeft = false;
  bool linearRight = false;
  // is left side linear
  if ((m_legsValue.fl.state == LEG_LINEAR) && (m_legsValue.rl.state == LEG_LINEAR)) {
    linearLeft = true;
  } else {
    // left side not linear
    if (m_legsValue.fl.state != LEG_LINEAR) {
      m_legsValue.fl.state = _processLegState(m_legsValue.fl.state, m_legsValue.fl.shift, m_legsValue.fl.hight, m_legsValue.fl.targeth, m_legsValue.fl.targets);
    }
    if (m_legsValue.rl.state != LEG_LINEAR) {
      m_legsValue.rl.state = _processLegState(m_legsValue.rl.state, m_legsValue.rl.shift, m_legsValue.rl.hight, m_legsValue.rl.targeth, m_legsValue.rl.targets);
    }
  }
  // is right side linear
  if ((m_legsValue.fr.state == LEG_LINEAR) && (m_legsValue.rr.state == LEG_LINEAR)) {
    linearRight = true;
  } else {
    // right side not linear
    if (m_legsValue.fr.state != LEG_LINEAR) {
      m_legsValue.fr.state = _processLegState(m_legsValue.fr.state, m_legsValue.fr.shift, m_legsValue.fr.hight, m_legsValue.fr.targeth, m_legsValue.fr.targets);
    }
    if (m_legsValue.rr.state != LEG_LINEAR) {
      m_legsValue.rr.state = _processLegState(m_legsValue.rr.state, m_legsValue.rr.shift, m_legsValue.rr.hight, m_legsValue.rr.targeth, m_legsValue.rr.targets);
    }
  }
  if (linearLeft) {
    m_legsValue.fl.location = _getLegLocation(m_legsValue.fl.shift, m_legsValue.rl.shift, STEP_SIZE);
    m_legsValue.rl.location = _getLegLocation(m_legsValue.rl.shift, m_legsValue.fl.shift, STEP_SIZE);
    if (_getLegLiftingFlag(m_legsValue.fl.shift, m_legsValue.fl.location, m_legsValue.fl.speed)) {
      m_legsValue.fl.state = LEG_BEFORE_LIFTING;
      return true;
    }
    if (_getLegLiftingFlag(m_legsValue.rl.shift, m_legsValue.rl.location, m_legsValue.rl.speed)) {
      m_legsValue.rl.state = LEG_BEFORE_LIFTING;
      return true;
    }
  }
  if (linearRight) {
    m_legsValue.fr.location = _getLegLocation(m_legsValue.fr.shift, m_legsValue.rr.shift, STEP_SIZE);
    m_legsValue.rr.location = _getLegLocation(m_legsValue.rr.shift, m_legsValue.fr.shift, STEP_SIZE);
    if (_getLegLiftingFlag(m_legsValue.fr.shift, m_legsValue.fr.location, m_legsValue.fr.speed)) {
      m_legsValue.fr.state = LEG_BEFORE_LIFTING;
      return true;
    }
    if (_getLegLiftingFlag(m_legsValue.rr.shift, m_legsValue.rr.location, m_legsValue.rr.speed)) {
      m_legsValue.rr.state = LEG_BEFORE_LIFTING;
      return true;
    }
  }
}

// init main time cycle
void initCounter(short mainCycle) {
  mainTiming.fullCycle = mainCycle;
  mainTiming.halfCycle = mainTiming.fullCycle / 2;
  mainTiming.quarterCycle = mainTiming.halfCycle / 2;
  keepCounting = true;
}

// print counter state
void _printCounterState(unsigned char stateCount) {
  switch (stateCount) {
    case LEG_LINEAR:
      Serial.print(F(" LEG_LINEAR "));
    break;
    case LEG_BEFORE_LIFTING:
      Serial.print(F(" LEG_BEFORE_LIFTING "));
    break;
    case LEG_LIFTING:
      Serial.print(F(" LEG_LIFTING "));
    break;
    case LEG_LIFTED:
      Serial.print(F(" LEG_LIFTED "));
    break;
    case LEG_LOWERING:
      Serial.print(F(" LEG_LOWERING "));
    break;
    case LEG_AFTER_LOWERING:
      Serial.print(F(" LEG_AFTER_LOWERING "));
    break;
    default:
      Serial.println(F(" Wrong gyro state "));
  }
}
