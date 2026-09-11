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
void updateLegsCounter(void) {
  // check center mass
  short massFLFR = (m_legsValue.fl.shift + m_legsValue.fr.shift) / 2;
  short massFLRR = (m_legsValue.fl.shift + m_legsValue.rr.shift) / 2;
  short massFRRL = (m_legsValue.fr.shift + m_legsValue.rl.shift) / 2;
  short massRLRR = (m_legsValue.rl.shift + m_legsValue.rr.shift) / 2;
  // leg shift
  short shiftFL = m_legsValue.fl.shift;
  short shiftFR = m_legsValue.fr.shift;
  short shiftRL = m_legsValue.rl.shift;
  short shiftRR = m_legsValue.rr.shift;
  // check and change legs state
  // all linear
  if ((m_legsValue.fl.state == LEG_LINEAR) && (m_legsValue.fr.state == LEG_LINEAR) && (m_legsValue.rl.state == LEG_LINEAR) && (m_legsValue.rr.state == LEG_LINEAR)) {
    // make decision whitch leg to lift first
    if ((massFRRL < 0) && (massRLRR < 0)) {
      // can't lift fl
      shiftFL = 0;
    }
    if ((massRLRR < 0) && (massFLRR < 0)) {
      // can't lift fr
      shiftFR = 0;
    }
    if ((massFLFR < 0) && (massFLRR < 0)) {
      // can't lift rl
      shiftRL = 0;
    }
    if ((massFLFR < 0) && (massFRRL < 0)) {
      // can't lift rr
      shiftRR = 0;
    }

    // print values
    Serial.print(" shiftFL ");
    Serial.print((int)shiftFL);
    Serial.print(" shiftFR ");
    Serial.print((int)shiftFR);
    Serial.print(" shiftRL ");
    Serial.print((int)shiftRL);
    Serial.print(" shiftRR ");
    Serial.println((int)shiftRR);

    // max value leg gets lifted
    if ((shiftFL >= shiftFR) && (shiftFL >= shiftRL) && (shiftFL >= shiftRR) && (shiftFL > STEP_SIZE)) {
      // lift fl
      m_legsValue.fl.state = LEG_BEFORE_LIFTING;
    } else if ((shiftFR >= shiftFL) && (shiftFR >= shiftRL) && (shiftFR >= shiftRR) && (shiftFR > STEP_SIZE)) {
      // lift fr
      m_legsValue.fr.state = LEG_BEFORE_LIFTING;
    } else if ((shiftRL >= shiftRR) && (shiftRL >= shiftFL) && (shiftRL >= shiftFR) && (shiftRL > STEP_SIZE)) {
      // lift rl
      m_legsValue.rl.state = LEG_BEFORE_LIFTING;
    } else if ((shiftRR >= shiftFL) && (shiftRR >= shiftFR) && (shiftRR >= shiftRL) && (shiftRR > STEP_SIZE)) {
      // lift rr
      m_legsValue.rr.state = LEG_BEFORE_LIFTING;
    }
  } else {
    // not all linear
    if (m_legsValue.fl.state != LEG_LINEAR) {
      m_legsValue.fl.state = _processLegState(m_legsValue.fl.state, m_legsValue.fl.shift, m_legsValue.fl.hight, m_legsValue.fl.targeth, m_legsValue.fl.targets);
    } else if (m_legsValue.fr.state != LEG_LINEAR) {
      m_legsValue.fr.state = _processLegState(m_legsValue.fr.state, m_legsValue.fr.shift, m_legsValue.fr.hight, m_legsValue.fr.targeth, m_legsValue.fl.targets);
    } else if (m_legsValue.rl.state != LEG_LINEAR) {
      m_legsValue.rl.state = _processLegState(m_legsValue.rl.state, m_legsValue.rl.shift, m_legsValue.rl.hight, m_legsValue.rl.targeth, m_legsValue.fl.targets);
    } else if (m_legsValue.rr.state != LEG_LINEAR) {
      m_legsValue.rr.state = _processLegState(m_legsValue.rr.state, m_legsValue.rr.shift, m_legsValue.rr.hight, m_legsValue.rr.targeth, m_legsValue.fl.targets);
    }
  }
  //Serial.println(" ");
  //_printCounterState(m_legsValue.fl.state);
  //_printCounterState(m_legsValue.fr.state);
  //_printCounterState(m_legsValue.rl.state);
  //_printCounterState(m_legsValue.rr.state);
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
