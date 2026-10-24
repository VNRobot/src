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
} timing;

// main timing 44 22
timing mainTiming = {MAIN_COUNTER_END, MAIN_COUNTER_END / 2, LIFT_TIME_POINT};

/*
uses
m_legsValue
*/

// get leg state
char _getLegState(char counter) {
  char state = LEG_LINEAR;
  if (counter < mainTiming.liftPoint) {
    // start of cycle
    state = LEG_LIFTED_AFTER;
  } else if (counter > mainTiming.fullCycle - mainTiming.liftPoint) {
    // end of cycle
    state = LEG_LIFTED_BEFORE;
  } else if (counter == mainTiming.liftPoint) {
    state = LEG_LOWERING;
  } else if (counter == mainTiming.fullCycle - mainTiming.liftPoint) {
    state = LEG_LIFTING;
  } else if (counter == mainTiming.liftPoint + 1) {
    state = LEG_AFTER_LOWERING;
  } else if (counter == mainTiming.fullCycle - mainTiming.liftPoint - 1) {
    state = LEG_BEFORE_LIFTING;
  }
  return state;
}

// set legs state
void _setlegsState(bool walkingModeNow) {
  if (walkingModeNow) {
    // set legs in walking state
    m_legsValue.fl.state = _getLegState(m_legsValue.fl.count);
    m_legsValue.fr.state = _getLegState(m_legsValue.fr.count);
    m_legsValue.rl.state = _getLegState(m_legsValue.rl.count);
    m_legsValue.rr.state = _getLegState(m_legsValue.rr.count);
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
  m_legsValue.rr.count = m_legsValue.fl.count;
  m_legsValue.rl.count = m_legsValue.fr.count;
  // set legs state
  _setlegsState(walkingModeNow);
  if (masterCounter) {
    return (char)m_legsValue.fl.count;
  } else {
    return (char)m_legsValue.fr.count;
  }
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
