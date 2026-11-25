/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Robot legs phase counters
*/

// main half time delay in ms. bigger the number slower the robot
#define MAIN_TIME_DELAY               12
// main counter
#define MAIN_COUNTER_END              44
// leg lift time point
#define LIFT_TIME_POINT               5

// structure for timing
typedef struct timing {
  short fullCycle;
  short halfCycle;
  short liftPoint;
  short rearShift;
  short rearShiftNow;
} timing;

// main timing 44 22
timing mainTiming = {
  MAIN_COUNTER_END * 2,     // short fullCycle;
  MAIN_COUNTER_END,         // short halfCycle;
  LIFT_TIME_POINT,          // short liftPoint;
  MAIN_COUNTER_END / 2,     // short rearShift;
  0                         // short rearShiftNow;
};

/*
uses
m_legsValue
*/

// get leg state
char _getLegState(leg legData) {
  if (legData.count == mainTiming.fullCycle - mainTiming.liftPoint - 1) {
    return LEG_BEFORE_LIFTING;
  }
  if (legData.count == mainTiming.liftPoint) {
    return LEG_LOWERING;
  }
  if (legData.state == LEG_BEFORE_LIFTING) {
    return LEG_LIFTING;
  }
  if (legData.state == LEG_LIFTING) {
    return LEG_LIFTED;
  }
  if (legData.state == LEG_LOWERING) {
    return LEG_AFTER_LOWERING;
  }
  if (legData.state == LEG_AFTER_LOWERING) {
    return LEG_LINEAR;
  }
  return legData.state;
}

// set legs state
void _setlegsState(bool walkingModeNow) {
  if (walkingModeNow) {
    // set legs in walking state
    m_legsValue.fl.state = _getLegState(m_legsValue.fl);
    m_legsValue.fr.state = _getLegState(m_legsValue.fr);
    m_legsValue.rl.state = _getLegState(m_legsValue.rl);
    m_legsValue.rr.state = _getLegState(m_legsValue.rr);
  } else {
    // not walking always linear
    m_legsValue.fl.state = LEG_LINEAR;
    m_legsValue.fr.state = LEG_LINEAR;
    m_legsValue.rl.state = LEG_LINEAR;
    m_legsValue.rr.state = LEG_LINEAR;
  }
}

// attach main time cycle
void attachCounter(void) {
}

// init main time cycle
void initCounter(void) {
}

// update servo motors values
char updateCounter(bool walkingModeNow, bool masterCounter) {
  // update main counter
  m_legsValue.fl.count ++;
  if (m_legsValue.fl.count >= mainTiming.fullCycle) {
    m_legsValue.fl.count = 0;
  }
  m_legsValue.fr.count = m_legsValue.fl.count + mainTiming.halfCycle;
  if (m_legsValue.fr.count >= mainTiming.fullCycle) {
    m_legsValue.fr.count -= mainTiming.fullCycle;
  }
  // rear time shift
  if (mainTiming.rearShiftNow > mainTiming.rearShift) {
    mainTiming.rearShiftNow --;
  } else if (mainTiming.rearShiftNow < mainTiming.rearShift) {
    mainTiming.rearShiftNow ++;
  }
  m_legsValue.rr.count = m_legsValue.fl.count - mainTiming.rearShiftNow;
  if (m_legsValue.rr.count < 0) {
    m_legsValue.rr.count += mainTiming.fullCycle;
  }
  m_legsValue.rl.count = m_legsValue.fr.count - mainTiming.rearShiftNow;
  if (m_legsValue.rl.count < 0) {
    m_legsValue.rl.count += mainTiming.fullCycle;
  }
  // set legs state using count value
  _setlegsState(walkingModeNow);
  return m_legsValue.fl.count;
}

// sync from master
void setHalfCounter (bool walkingModeNow) {
  m_legsValue.fl.count = mainTiming.halfCycle - 1;
  m_legsValue.fr.count = mainTiming.fullCycle - 1;
  m_legsValue.rr.count = m_legsValue.fl.count;
  m_legsValue.rl.count = m_legsValue.fr.count;
  // set legs state
  _setlegsState(walkingModeNow);
}

// delay
void doSleepHalfTime(void) {
  delay(MAIN_TIME_DELAY);
}

// get leg lifted speed
short getQuickShiftMuliplierCounter(void) {
  return (mainTiming.halfCycle - mainTiming.liftPoint) / mainTiming.liftPoint;
}

// set mode sync or async
void setSyncModeCounter(bool syncMode) {
  if (syncMode) {
    mainTiming.fullCycle = MAIN_COUNTER_END;
    mainTiming.halfCycle = MAIN_COUNTER_END / 2;
    mainTiming.rearShift = 0;
  } else {
    mainTiming.fullCycle = MAIN_COUNTER_END * 2;
    mainTiming.halfCycle = MAIN_COUNTER_END;
    mainTiming.rearShift = MAIN_COUNTER_END / 2;
  }
}
