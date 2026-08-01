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
char legsPairShift = 5;
// sequence counters
char counterFL = 0;
char counterFR = mainTiming.halfCycle;
char counterRR = mainTiming.fullCycle - legsPairShift;
char counterRL = mainTiming.halfCycle - legsPairShift;

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
unsigned char updateCounter(bool keepCounting, bool walkForward) {
  // update main counter
  if (keepCounting || (counterFL == 0)) {
    counterFL ++;
  }
  if (counterFL >= mainTiming.fullCycle) {
    counterFL = 0;
  }
  counterFR = counterFL + mainTiming.halfCycle;
  if (counterFR >= mainTiming.fullCycle) {
    counterFR -= mainTiming.fullCycle;
  }
  // rear pair shift depends on firection of movement
  if (walkForward) {
    counterRR = counterFL - legsPairShift;
  } else {
    counterRR = counterFL + legsPairShift;
  }
  if (counterRR >= mainTiming.fullCycle) {
    counterRR -= mainTiming.fullCycle;
  } else if (counterRR < 0) {
    counterRR += mainTiming.fullCycle;
  }
  counterRL = counterRR + mainTiming.halfCycle;
  if (counterRL >= mainTiming.fullCycle) {
    counterRL -= mainTiming.fullCycle;
  }
  return (unsigned char)counterFL;
}

// set legs state
void setLegsStateCounter(bool walkingModeNow, unsigned char liftPoint) {
  if (walkingModeNow) {
    // set legs state
    m_legsValue.fl.state = _getLegState(counterFL, liftPoint);
    m_legsValue.fr.state = _getLegState(counterFR, liftPoint);
    m_legsValue.rl.state = _getLegState(counterRL, liftPoint);
    m_legsValue.rr.state = _getLegState(counterRR, liftPoint);
  } else {
    m_legsValue.fl.state = LEG_LINEAR;
    m_legsValue.fr.state = LEG_LINEAR;
    m_legsValue.rl.state = LEG_LINEAR;
    m_legsValue.rr.state = LEG_LINEAR;
  }
}

// set main cycle value
void setMainCounter(short mainCycle) {
  mainTiming.fullCycle = mainCycle;
  mainTiming.halfCycle = mainTiming.fullCycle / 2;
  mainTiming.quarterCycle = mainTiming.halfCycle / 2;
  // the same for now
  legsPairShift = mainTiming.quarterCycle;
}

// get main cycle value
short getSizeCounter(void) {
  return mainTiming.fullCycle;
}
