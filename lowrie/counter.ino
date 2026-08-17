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
timing mainTiming = {64, 32, 16};
// pair shift
char legsPairShift = 16;
// sequence counters

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

// update servo motors values
unsigned char updateCounter(bool walkingModeNow, bool keepCounting) {
  // set direction
  bool walkForward = true;
  if ((m_legsValue.fl.speed < 0) || (m_legsValue.fr.speed < 0)) {
    walkForward = false;
  }
  // update main counter
  if (keepCounting || (m_legsValue.fl.count == 0)) {
    m_legsValue.fl.count ++;
  }
  if (m_legsValue.fl.count >= mainTiming.fullCycle) {
    m_legsValue.fl.count = 0;
  }
  m_legsValue.fr.count = m_legsValue.fl.count + mainTiming.halfCycle;
  if (m_legsValue.fr.count >= mainTiming.fullCycle) {
    m_legsValue.fr.count -= mainTiming.fullCycle;
  }
  // rear pair shift depends on firection of movement
  if (walkForward) {
    m_legsValue.rr.count = m_legsValue.fl.count - legsPairShift;
  } else {
    m_legsValue.rr.count = m_legsValue.fl.count + legsPairShift;
  }
  if (m_legsValue.rr.count >= mainTiming.fullCycle) {
    m_legsValue.rr.count -= mainTiming.fullCycle;
  } else if (m_legsValue.rr.count < 0) {
    m_legsValue.rr.count += mainTiming.fullCycle;
  }
  m_legsValue.rl.count = m_legsValue.rr.count + mainTiming.halfCycle;
  if (m_legsValue.rl.count >= mainTiming.fullCycle) {
    m_legsValue.rl.count -= mainTiming.fullCycle;
  }
  if (walkingModeNow) {
    // set legs state
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
  return (unsigned char)m_legsValue.fl.count;
}

// set main cycle value
void setMainCounter(short mainCycle) {
  mainTiming.fullCycle = mainCycle;
  mainTiming.halfCycle = mainTiming.fullCycle / 2;
  mainTiming.quarterCycle = mainTiming.halfCycle / 2;
  // the same for now
  legsPairShift = mainTiming.quarterCycle;
}
