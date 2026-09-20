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
} timing;

// main timing 44 22
timing mainTiming = {44, 22};

/*
uses
m_legsValue
*/

// get leg state
unsigned char _getLegState(char counter, unsigned char liftPoint) {
  unsigned char state = LEG_LINEAR;
  if (counter < liftPoint) {
    // start of cycle
    state = LEG_LIFTED_AFTER;
  } else if (counter > mainTiming.fullCycle - liftPoint) {
    // end of cycle
    state = LEG_LIFTED_BEFORE;
  } else if (counter == liftPoint) {
    state = LEG_LOWERING;
  } else if (counter == mainTiming.fullCycle - liftPoint) {
    state = LEG_LIFTING;
  } else if (counter == liftPoint + 1) {
    state = LEG_AFTER_LOWERING;
  } else if (counter == mainTiming.fullCycle - liftPoint - 1) {
    state = LEG_BEFORE_LIFTING;
  }
  return state;
}

// set legs state
void _setlegsState(bool walkingModeNow) {
  if (walkingModeNow) {
    // set legs in walking state
    m_legsValue.fl.state = _getLegState(m_legsValue.fl.count, m_legsValue.fl.liftPoint);
    m_legsValue.fr.state = _getLegState(m_legsValue.fr.count, m_legsValue.fr.liftPoint);
    m_legsValue.rl.state = _getLegState(m_legsValue.rl.count, m_legsValue.rl.liftPoint);
    m_legsValue.rr.state = _getLegState(m_legsValue.rr.count, m_legsValue.rr.liftPoint);
  } else {
    // not walking always linear
    m_legsValue.fl.state = LEG_LINEAR;
    m_legsValue.fr.state = LEG_LINEAR;
    m_legsValue.rl.state = LEG_LINEAR;
    m_legsValue.rr.state = LEG_LINEAR;
  }
}

// update servo motors values
unsigned char updateCounter(bool walkingModeNow, bool masterCounter) {
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
    return (unsigned char)m_legsValue.fl.count;
  } else {
    return (unsigned char)m_legsValue.fr.count;
  }
}

// init main time cycle
void initCounter(short mainCycle) {
  mainTiming.fullCycle = mainCycle;
  mainTiming.halfCycle = mainTiming.fullCycle / 2;
}

void setHalfCounter (bool walkingModeNow) {
  m_legsValue.fl.count = mainTiming.halfCycle;
  m_legsValue.fr.count = 0;
  m_legsValue.rr.count = m_legsValue.fl.count;
  m_legsValue.rl.count = m_legsValue.fr.count;
  // set legs state
  _setlegsState(walkingModeNow);
}