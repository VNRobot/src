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
  Serial.println(" - ");
  // side linear
  bool linearLeft = false;
  bool linearRight = false;
  // is left side linear
  if ((m_legsValue.fl.state == LEG_LINEAR) && (m_legsValue.rl.state == LEG_LINEAR)) {
    linearLeft = true;
  }
  // is right side linear
  if ((m_legsValue.fr.state == LEG_LINEAR) && (m_legsValue.rr.state == LEG_LINEAR)) {
    linearRight = true;
  }
  // update not linear state
  if ((!linearLeft) || (!linearRight)) {
    // left side not linear
    if (m_legsValue.fl.state != LEG_LINEAR) {
      m_legsValue.fl.state = _processLegState(m_legsValue.fl.state, m_legsValue.fl.shift, m_legsValue.fl.hight, m_legsValue.fl.targeth, m_legsValue.fl.targets);
    } else if (m_legsValue.rl.state != LEG_LINEAR) {
      m_legsValue.rl.state = _processLegState(m_legsValue.rl.state, m_legsValue.rl.shift, m_legsValue.rl.hight, m_legsValue.rl.targeth, m_legsValue.rl.targets);
    }
    // right side not linear
    if (m_legsValue.fr.state != LEG_LINEAR) {
      m_legsValue.fr.state = _processLegState(m_legsValue.fr.state, m_legsValue.fr.shift, m_legsValue.fr.hight, m_legsValue.fr.targeth, m_legsValue.fr.targets);
    } else if (m_legsValue.rr.state != LEG_LINEAR) {
      m_legsValue.rr.state = _processLegState(m_legsValue.rr.state, m_legsValue.rr.shift, m_legsValue.rr.hight, m_legsValue.rr.targeth, m_legsValue.rr.targets);
    }
    return true;
  }
  // left side
  if (linearLeft) {
    // check legs location on the left side
    m_legsValue.fl.location = _getLegLocation(m_legsValue.fl.shift, m_legsValue.rl.shift, STEP_SIZE);
    m_legsValue.rl.location = _getLegLocation(m_legsValue.rl.shift, m_legsValue.fl.shift, STEP_SIZE);
  }
  // right side
  if (linearRight) {
    // check legs location on the right side
    m_legsValue.fr.location = _getLegLocation(m_legsValue.fr.shift, m_legsValue.rr.shift, STEP_SIZE);
    m_legsValue.rr.location = _getLegLocation(m_legsValue.rr.shift, m_legsValue.fr.shift, STEP_SIZE);
  }
  // check legs liftable
  bool canLiftFL = false;
  bool canLiftRL = false;
  bool canLiftFR = false;
  bool canLiftRR = false;
  if (linearLeft && linearRight) {
    // get center mass related lifting flag
    canLiftFL = _getLiftingCandidate(m_legsValue.fr.shift, m_legsValue.rr.shift, m_legsValue.rl.shift, STEP_MARGINE);
    canLiftRL = _getLiftingCandidate(m_legsValue.fr.shift, m_legsValue.rr.shift, m_legsValue.fl.shift, STEP_MARGINE);
    canLiftFR = _getLiftingCandidate(m_legsValue.fl.shift, m_legsValue.rl.shift, m_legsValue.rr.shift, STEP_MARGINE);
    canLiftRR = _getLiftingCandidate(m_legsValue.fl.shift, m_legsValue.rl.shift, m_legsValue.fr.shift, STEP_MARGINE);
  }
  // check left side lift
  if (canLiftFL && ((m_legsValue.fl.speed >= 0) && (m_legsValue.fl.location == REAR_LEG) && (m_legsValue.fl.shift > STEP_SIZE))) {
    m_legsValue.fl.state = LEG_BEFORE_LIFTING;
    return true;
  } else if (canLiftFL && ((m_legsValue.fl.speed < 0) && (m_legsValue.fl.location == FRONT_LEG) && (m_legsValue.fl.shift < -STEP_SIZE))) {
    m_legsValue.fl.state = LEG_BEFORE_LIFTING;
    return true;
  } else if (canLiftRL && ((m_legsValue.rl.speed >= 0) && (m_legsValue.rl.location == REAR_LEG) && (m_legsValue.rl.shift > STEP_SIZE))) {
    m_legsValue.rl.state = LEG_BEFORE_LIFTING;
    return true;
  } else if (canLiftRL && ((m_legsValue.rl.speed < 0) && (m_legsValue.rl.location == FRONT_LEG) && (m_legsValue.rl.shift < -STEP_SIZE))) {
    m_legsValue.rl.state = LEG_BEFORE_LIFTING;
    return true;
  }
  // check right side lift
  if (canLiftFR && ((m_legsValue.fr.speed >= 0) && (m_legsValue.fr.location == REAR_LEG) && (m_legsValue.fr.shift > STEP_SIZE))) {
    m_legsValue.fr.state = LEG_BEFORE_LIFTING;
    return true;
  } else if (canLiftFR && ((m_legsValue.fr.speed < 0) && (m_legsValue.fr.location == FRONT_LEG) && (m_legsValue.fr.shift < -STEP_SIZE))) {
    m_legsValue.fr.state = LEG_BEFORE_LIFTING;
    return true;
  } else if (canLiftRR && ((m_legsValue.rr.speed >= 0) && (m_legsValue.rr.location == REAR_LEG) && (m_legsValue.rr.shift > STEP_SIZE))) {
    m_legsValue.rr.state = LEG_BEFORE_LIFTING;
    return true;
  } else if (canLiftRR && ((m_legsValue.rr.speed < 0) && (m_legsValue.rr.location == FRONT_LEG) && (m_legsValue.rr.shift < -STEP_SIZE))) {
    m_legsValue.rr.state = LEG_BEFORE_LIFTING;
    return true;
  }
  // emergency lift
  if (m_legsValue.fl.location == WRONG_LEG) {
    m_legsValue.fl.state = LEG_BEFORE_LIFTING;
    return true;
  } else if (m_legsValue.fr.location == WRONG_LEG) {
    m_legsValue.fr.state = LEG_BEFORE_LIFTING;
    return true;
  } else if (m_legsValue.rl.location == WRONG_LEG) {
    m_legsValue.rl.state = LEG_BEFORE_LIFTING;
    return true;
  } else if (m_legsValue.rr.location == WRONG_LEG) {
    m_legsValue.rr.state = LEG_BEFORE_LIFTING;
    return true;
  }
  //Serial.println(" ");
  //_printCounterState(m_legsValue.fl.state);
  //_printCounterState(m_legsValue.fr.state);
  //_printCounterState(m_legsValue.rl.state);
  //_printCounterState(m_legsValue.rr.state);
  return false;
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
