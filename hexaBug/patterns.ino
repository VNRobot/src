/*
Walking Hexapod Robot
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Robot legs motion patterns
*/

char cBuffer;
// points to currentSequence for every leg
unsigned char mainCounter = 0;
// 24 positions per sequence
char sequenceFL1[24] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
char sequenceFL2[24] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
char sequenceFR1[24] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
char sequenceFR2[24] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
char sequenceSL1[24] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
char sequenceSL2[24] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
char sequenceSR1[24] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
char sequenceSR2[24] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
char sequenceRL1[24] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
char sequenceRL2[24] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
char sequenceRR1[24] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
char sequenceRR2[24] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
// sequence for leg lift
char sequenceLiftFL1[24] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
char sequenceLiftFL2[24] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
char sequenceLiftFR1[24] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
char sequenceLiftFR2[24] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
char sequenceLiftSL1[24] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
char sequenceLiftSL2[24] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
char sequenceLiftSR1[24] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
char sequenceLiftSR2[24] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
char sequenceLiftRL1[24] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
char sequenceLiftRL2[24] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
char sequenceLiftRR1[24] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
char sequenceLiftRR2[24] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
// motors values for 10 motors
allMotors motorValue = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
allMotors motorLift = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
// pointer to array
unsigned char apoint = 0;

// update servo motors values
unsigned char updateCountPatterns(void) {
  // update sequence shift 
  mainCounter ++;
  if (mainCounter >= m_fullCycle) {
    mainCounter = 0;
  }
  return mainCounter;
}

// motor value for walk
void _setSequenceValue(char endPosition, char * sequence) {
  for (i = 0; i < m_fullCycle; i++) {
    sequence[i] = endPosition;
  }
}

// linear motor movement from point to point
void _updateSequenceLinearLift(char endPosition, char * sequence) {
  cBuffer = sequence[m_fullCycle - 1];
  sequence[0] = cBuffer;
  for (i = 1; i < (m_fullCycle - 1); i++) {
    sequence[i] = cBuffer + ((endPosition - cBuffer) * i) / m_fullCycle;
  }
  sequence[m_fullCycle - 1] = endPosition;
}

// linear motor movement from point to point at the end of sequence
void _updateSequenceLinearLiftEnd(char endPosition, char * sequence) {
  cBuffer = sequence[m_fullCycle - 1];
  for (i = 0; i < (m_fullCycle - 4); i++) {
    sequence[i] = cBuffer;
  }
  sequence[m_fullCycle - 4] = cBuffer + (endPosition - cBuffer) / 4;
  sequence[m_fullCycle - 3] = cBuffer + ((endPosition - cBuffer) * 2) / 4;
  sequence[m_fullCycle - 2] = cBuffer + ((endPosition - cBuffer) * 3) / 4;
  sequence[m_fullCycle - 1] = endPosition;
}

// linear motor movement from point to point at the start of sequence
void _updateSequenceLinearLiftStart(char endPosition, char * sequence) {
  cBuffer = sequence[m_fullCycle - 1];
  sequence[0] = cBuffer;
  sequence[1] = cBuffer + (endPosition - cBuffer) / 4;
  sequence[2] = cBuffer + ((endPosition - cBuffer) * 2) / 4;
  sequence[3] = cBuffer + ((endPosition - cBuffer) * 3) / 4;
  for (i = 4; i < m_fullCycle; i++) {
    sequence[i] = endPosition;
  }
}

void _updateApointer(unsigned char pointer, unsigned char shift) {
  apoint = pointer + shift;
  if (apoint >= m_fullCycle) {
    apoint -= m_fullCycle;
  }
}

// create lift sequence for stand
void _updateSequenceLiftStandWalk(char * lift, unsigned char shift) {
  _updateApointer(0, shift);
  lift[apoint] = - m_legPatternLift;
  _updateApointer(1, shift);
  lift[apoint] = - m_legPatternLift;
  _updateApointer(2, shift);
  lift[apoint] = - m_legPatternLift / 2;
  for (i = 3; i < m_fullCycle - 2; i++) {
    _updateApointer(i, shift);
    lift[apoint] = 0;
  }
  _updateApointer(m_fullCycle - 2, shift);
  lift[apoint] = - m_legPatternLift;
  _updateApointer(m_fullCycle - 1, shift);
  lift[apoint] = - m_legPatternLift;
}

// create lift sequence for walk
void _updateSequenceLiftWalk1(char * lift, unsigned char shift) {
  _updateApointer(0, shift);
  lift[apoint] = - m_legPatternLift * 2;
  _updateApointer(1, shift);
  lift[apoint] = - m_legPatternLift * 2;
  _updateApointer(2, shift);
  lift[apoint] = - m_legPatternLift * 2;
  for (i = 3; i < m_fullCycle - 2; i++) {
    _updateApointer(i, shift);
    lift[apoint] = 0;
  }
  _updateApointer(m_fullCycle - 2, shift);
  lift[apoint] = 0;
  _updateApointer(m_fullCycle - 1, shift);
  lift[apoint] = - m_legPatternLift * 2;
}

// create lift sequence for walk
void _updateSequenceLiftWalk2(char * lift, unsigned char shift) {
  _updateApointer(0, shift);
  lift[apoint] = - m_legPatternLift * 2;
  _updateApointer(1, shift);
  lift[apoint] = - m_legPatternLift * 2;
  _updateApointer(2, shift);
  lift[apoint] = 0;
  for (i = 3; i < m_fullCycle - 2; i++) {
    _updateApointer(i, shift);
    lift[apoint] = 0;
  }
  _updateApointer(m_fullCycle - 2, shift);
  lift[apoint] = - m_legPatternLift * 2;
  _updateApointer(m_fullCycle - 1, shift);
  lift[apoint] = - m_legPatternLift * 2;
}

// create walk sequence
void _updateSequenceWalk(char * sequence, unsigned char shift) {
  _updateApointer(0, shift);
  sequence[apoint] = 0;
  _updateApointer(1, shift);
  sequence[apoint] = - m_halfCycle;
  for (i = 2; i < m_fullCycle - 1; i++) {
    _updateApointer(i, shift);
    sequence[apoint] = (i - m_halfCycle) * 2;
  }
  _updateApointer(m_fullCycle - 1, shift);
  sequence[apoint] = m_halfCycle;
}

// create walk sequence
void _updateSequenceWalkSlow(char * sequence, unsigned char shift) {
  _updateApointer(0, shift);
  sequence[apoint] = 0;
  _updateApointer(1, shift);
  sequence[apoint] = - m_halfCycle / 2;
  for (i = 2; i < m_fullCycle - 1; i++) {
    _updateApointer(i, shift);
    sequence[apoint] = i - m_halfCycle;
  }
  _updateApointer(m_fullCycle - 1, shift);
  sequence[apoint] = m_halfCycle / 2;
}

// create walk sequence
void _updateSequenceWalkBack(char * sequence, unsigned char shift) {
  _updateApointer(0, shift);
  sequence[apoint] = 0;
  _updateApointer(1, shift);
  sequence[apoint] = m_halfCycle / 2;
  for (i = 2; i < m_fullCycle - 1; i++) {
    _updateApointer(i, shift);
    sequence[apoint] = m_halfCycle - i;
  }
  _updateApointer(m_fullCycle - 1, shift);
  sequence[apoint] = - m_halfCycle / 2;
}

// create recover sequence
void _setSequenceRecoverLift(char * sequence1, char * sequence2) {
  // leg 1
  sequence1[0] = -15;
  sequence1[1] = -45;
  for (i = 2; i < m_fullCycle - 2; i++) {
    sequence1[i] = -75;
  }
  sequence1[m_fullCycle - 2] = -15;
  sequence1[m_fullCycle - 1] = 0;
  // leg 2
  sequence2[0] = 15;
  sequence2[1] = 35;
  sequence2[2] = 45;
  sequence2[3] = 45;
  sequence2[4] = 45;
  sequence2[5] = 45;
  for (i = 6; i < m_fullCycle - 2; i++) {
    sequence2[i] = -45;
  }
  sequence2[m_fullCycle - 2] = -15;
  sequence2[m_fullCycle - 1] = 0;
}

void _setStandWalkLiftSequence(void) {
  _updateSequenceLiftStandWalk(sequenceLiftFL1, 20);
  _updateSequenceLiftStandWalk(sequenceLiftFL2, 20);
  _updateSequenceLiftStandWalk(sequenceLiftFR1, 8);
  _updateSequenceLiftStandWalk(sequenceLiftFR2, 8);
  _updateSequenceLiftStandWalk(sequenceLiftSL1, 4);
  _updateSequenceLiftStandWalk(sequenceLiftSL2, 4);
  _updateSequenceLiftStandWalk(sequenceLiftSR1, 16);
  _updateSequenceLiftStandWalk(sequenceLiftSR2, 16);
  _updateSequenceLiftStandWalk(sequenceLiftRL1, 12);
  _updateSequenceLiftStandWalk(sequenceLiftRL2, 12);
  _updateSequenceLiftStandWalk(sequenceLiftRR1, 0);
  _updateSequenceLiftStandWalk(sequenceLiftRR2, 0);
}

void _setWalkLiftSequence(void) {
  _updateSequenceLiftWalk1(sequenceLiftFL1, 20);
  _updateSequenceLiftWalk2(sequenceLiftFL2, 20);
  _updateSequenceLiftWalk1(sequenceLiftFR1, 8);
  _updateSequenceLiftWalk2(sequenceLiftFR2, 8);
  _updateSequenceLiftWalk1(sequenceLiftSL1, 4);
  _updateSequenceLiftWalk2(sequenceLiftSL2, 4);
  _updateSequenceLiftWalk1(sequenceLiftSR1, 16);
  _updateSequenceLiftWalk2(sequenceLiftSR2, 16);
  _updateSequenceLiftWalk1(sequenceLiftRL1, 12);
  _updateSequenceLiftWalk2(sequenceLiftRL2, 12);
  _updateSequenceLiftWalk1(sequenceLiftRR1, 0);
  _updateSequenceLiftWalk2(sequenceLiftRR2, 0);
}

void _setWalkSequenceLeft(void) {
  _updateSequenceWalk(sequenceFL1, 20);
  _updateSequenceWalk(sequenceFL2, 20);
  _updateSequenceWalk(sequenceSL1, 4);
  _updateSequenceWalk(sequenceSL2, 4);
  _updateSequenceWalk(sequenceRL1, 12);
  _updateSequenceWalk(sequenceRL2, 12);
}

void _setWalkSequenceRight(void) {
  _updateSequenceWalk(sequenceFR1, 8);
  _updateSequenceWalk(sequenceFR2, 8);
  _updateSequenceWalk(sequenceSR1, 16);
  _updateSequenceWalk(sequenceSR2, 16);
  _updateSequenceWalk(sequenceRR1, 0);
  _updateSequenceWalk(sequenceRR2, 0);
}

void _setWalkSlowSequenceLeft(void) {
  _updateSequenceWalkSlow(sequenceFL1, 20);
  _updateSequenceWalkSlow(sequenceFL2, 20);
  _updateSequenceWalkSlow(sequenceSL1, 4);
  _updateSequenceWalkSlow(sequenceSL2, 4);
  _updateSequenceWalkSlow(sequenceRL1, 12);
  _updateSequenceWalkSlow(sequenceRL2, 12);
}

void _setWalkSlowSequenceRight(void) {
  _updateSequenceWalkSlow(sequenceFR1, 8);
  _updateSequenceWalkSlow(sequenceFR2, 8);
  _updateSequenceWalkSlow(sequenceSR1, 16);
  _updateSequenceWalkSlow(sequenceSR2, 16);
  _updateSequenceWalkSlow(sequenceRR1, 0);
  _updateSequenceWalkSlow(sequenceRR2, 0);
}

void _setWalkBackSequenceLeft(void) {
  _updateSequenceWalkBack(sequenceFL1, 20);
  _updateSequenceWalkBack(sequenceFL2, 20);
  _updateSequenceWalkBack(sequenceSL1, 4);
  _updateSequenceWalkBack(sequenceSL2, 4);
  _updateSequenceWalkBack(sequenceRL1, 12);
  _updateSequenceWalkBack(sequenceRL2, 12);
}

void _setWalkBackSequenceRight(void) {
  _updateSequenceWalkBack(sequenceFR1, 8);
  _updateSequenceWalkBack(sequenceFR2, 8);
  _updateSequenceWalkBack(sequenceSR1, 16);
  _updateSequenceWalkBack(sequenceSR2, 16);
  _updateSequenceWalkBack(sequenceRR1, 0);
  _updateSequenceWalkBack(sequenceRR2, 0);
}

void _setStandSequenceWalkLeft(char position1, char position2) {
  _setSequenceValue(position1, sequenceFL1);
  _setSequenceValue(position2, sequenceFL2);
  _setSequenceValue(position1, sequenceSL1);
  _setSequenceValue(position2, sequenceSL2);
  _setSequenceValue(position1, sequenceRL1);
  _setSequenceValue(position2, sequenceRL2);
}

void _setStandSequenceWalkRight(char position1, char position2) {
  _setSequenceValue(position1, sequenceFR1);
  _setSequenceValue(position2, sequenceFR2);
  _setSequenceValue(position1, sequenceSR1);
  _setSequenceValue(position2, sequenceSR2);
  _setSequenceValue(position1, sequenceRR1);
  _setSequenceValue(position2, sequenceRR2);
}

void _setStandToGoLift(void) {
  _updateSequenceLinearLiftEnd(0, sequenceLiftFL1);
  _updateSequenceLinearLiftEnd(0, sequenceLiftFL2);
  _updateSequenceLinearLiftEnd(0, sequenceLiftFR1);
  _updateSequenceLinearLiftEnd(0, sequenceLiftFR2);
  _updateSequenceLinearLiftEnd(0, sequenceLiftSL1);
  _updateSequenceLinearLiftEnd(0, sequenceLiftSL2);
  _updateSequenceLinearLiftEnd(0, sequenceLiftSR1);
  _updateSequenceLinearLiftEnd(0, sequenceLiftSR2);
  _updateSequenceLinearLiftEnd(0, sequenceLiftRL1);
  _updateSequenceLinearLiftEnd(0, sequenceLiftRL2);
  _updateSequenceLinearLiftEnd(-m_legPatternLift, sequenceLiftRR1);
  _updateSequenceLinearLiftEnd(-m_legPatternLift, sequenceLiftRR2);
}

void _setGoToStandLift(void) {
  _updateSequenceLinearLiftStart(0, sequenceLiftFL1);
  _updateSequenceLinearLiftStart(0, sequenceLiftFL2);
  _updateSequenceLinearLiftStart(0, sequenceLiftFR1);
  _updateSequenceLinearLiftStart(0, sequenceLiftFR2);
  _updateSequenceLinearLiftStart(0, sequenceLiftSL1);
  _updateSequenceLinearLiftStart(0, sequenceLiftSL2);
  _updateSequenceLinearLiftStart(0, sequenceLiftSR1);
  _updateSequenceLinearLiftStart(0, sequenceLiftSR2);
  _updateSequenceLinearLiftStart(0, sequenceLiftRL1);
  _updateSequenceLinearLiftStart(0, sequenceLiftRL2);
  _updateSequenceLinearLiftStart(0, sequenceLiftRR1);
  _updateSequenceLinearLiftStart(0, sequenceLiftRR2);
}

void _setLinearLift(char FL1, char FL2, char FR1, char FR2, char SL1, char SL2, char SR1, char SR2, char RL1, char RL2, char RR1, char RR2) {
  _updateSequenceLinearLift(FL1, sequenceLiftFL1);
  _updateSequenceLinearLift(FL2, sequenceLiftFL2);
  _updateSequenceLinearLift(FR1, sequenceLiftFR1);
  _updateSequenceLinearLift(FR2, sequenceLiftFR2);
  _updateSequenceLinearLift(SL1, sequenceLiftSL1);
  _updateSequenceLinearLift(SL2, sequenceLiftSL2);
  _updateSequenceLinearLift(SR1, sequenceLiftSR1);
  _updateSequenceLinearLift(SR2, sequenceLiftSR2);
  _updateSequenceLinearLift(RL1, sequenceLiftRL1);
  _updateSequenceLinearLift(RL2, sequenceLiftRL2);
  _updateSequenceLinearLift(RR1, sequenceLiftRR1);
  _updateSequenceLinearLift(RR2, sequenceLiftRR2);
}

// get next sequence
void setPattern(unsigned char currentTaskItem, char angleTurn) {
  // get new sequence array
  switch (currentTaskItem) {
    case P_DOSTAND:
    {
      _setStandSequenceWalkLeft(0, 0);
      _setStandSequenceWalkRight(0, 0);
      _setLinearLift(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    }
    break;
    case P_STANDTOGO:
    {
      _setStandSequenceWalkLeft(0, 0);
      _setStandSequenceWalkRight(0, 0);
      _setStandToGoLift();
    }
    break;
    case P_GOTOSTAND:
    {
      _setStandSequenceWalkLeft(0, 0);
      _setStandSequenceWalkRight(0, 0);
      _setGoToStandLift();
    }
    break;
    case P_STANDGO:
    {
      // positive - turned left
      // negative - turned right
      _setStandWalkLiftSequence();
      if (angleTurn > 2) {
        _setWalkSlowSequenceLeft();
        _setWalkBackSequenceRight();
      } else if (angleTurn < -2) {
        _setWalkBackSequenceLeft();
        _setWalkSlowSequenceRight();
      } else {
        _setStandSequenceWalkLeft(0, 0);
        _setStandSequenceWalkRight(0, 0);
      }
    }
    break;
    case P_STANDGOLEFT:
    {
      _setWalkBackSequenceLeft();
      _setWalkSlowSequenceRight();
      _setStandWalkLiftSequence();
    }
    break;
    case P_STANDGORIGHT:
    {
      _setWalkSlowSequenceLeft();
      _setWalkBackSequenceRight();
      _setStandWalkLiftSequence();
    }
    break;
    case P_GOFORWARD:
    {
      if (m_fastWalkingPatternEnabled) {
        _setWalkLiftSequence();
        if (angleTurn > 2) {
          _setWalkSequenceLeft();
          _setWalkSlowSequenceRight();
        } else if (angleTurn < -2) {
          _setWalkSlowSequenceLeft();
          _setWalkSequenceRight();
        } else {
          _setWalkSequenceLeft();
          _setWalkSequenceRight();
        }
      } else {
        _setStandWalkLiftSequence();
        if (angleTurn > 2) {
          _setWalkSlowSequenceLeft();
          _setStandSequenceWalkRight(0, 0);
        } else if (angleTurn < -2) {
          _setStandSequenceWalkLeft(0, 0);
          _setWalkSlowSequenceRight();
        } else {
          _setWalkSlowSequenceLeft();
          _setWalkSlowSequenceRight();
        }
      }
    }
    break;
    case P_GOFORWARDSLOW:
    {
      _setStandWalkLiftSequence();
      if (angleTurn > 2) {
        _setWalkSlowSequenceLeft();
        _setStandSequenceWalkRight(0, 0);
      } else if (angleTurn < -2) {
        _setStandSequenceWalkLeft(0, 0);
        _setWalkSlowSequenceRight();
      } else {
        _setWalkSlowSequenceLeft();
        _setWalkSlowSequenceRight();
      }
    }
    break;
    case P_GOLEFT:
    {
      if (m_fastWalkingPatternEnabled) {
        _setWalkLiftSequence();
        _setStandSequenceWalkLeft(0, 0);
        _setWalkSequenceRight();
      } else {
        _setStandWalkLiftSequence();
        _setStandSequenceWalkLeft(0, 0);
        _setWalkSlowSequenceRight();
      }
    }
    break;
    case P_GORIGHT:
    {
      if (m_fastWalkingPatternEnabled) {
        _setWalkLiftSequence();
        _setWalkSequenceLeft();
        _setStandSequenceWalkRight(0, 0);
      } else {
        _setStandWalkLiftSequence();
        _setWalkSlowSequenceLeft();
        _setStandSequenceWalkRight(0, 0);
      }
    }
    break;
    case P_GOBACK:
    {
      _setWalkBackSequenceLeft();
      _setWalkBackSequenceRight();
      _setStandWalkLiftSequence();
    }
    break;
    case P_GOBACKLEFT:
    {
      _setWalkBackSequenceLeft();
      _setStandSequenceWalkRight(0, 0);
      _setStandWalkLiftSequence();
    }
    break;
    case P_GOBACKRIGHT:
    {
      _setStandSequenceWalkLeft(0, 0);
      _setWalkBackSequenceRight();
      _setStandWalkLiftSequence();
    }
    break;
    case P_DODOWN:
    {
      _setStandSequenceWalkLeft(0, 0);
      _setStandSequenceWalkRight(0, 0);
      _setLinearLift(-30, -30, -30, -30, -30, -30, -30, -30, -30, -30, -30, -30);
    }
    break;
    case P_DODOWNLEFT:
    {
      _setStandSequenceWalkLeft(0, 0);
      _setStandSequenceWalkRight(0, 0);
      _setLinearLift(-30, -30, -45, -45, -30, -30, -45, -45, -30, -30, -45, -45);
    }
    break;
    case P_DODOWNRIGHT:
    {
      _setStandSequenceWalkLeft(0, 0);
      _setStandSequenceWalkRight(0, 0);
      _setLinearLift(-45, -45, -30, -30, -45, -45, -30, -30, -45, -45, -30, -30);
    }
    break;
    case P_DODOWNFRONT:
    {
      _setStandSequenceWalkLeft(0, 0);
      _setStandSequenceWalkRight(0, 0);
      _setLinearLift(-35, -35, -35, -35, -45, -45, -45, -45, -55, -55, -55, -55);
    }
    break;
    case P_DODOWNREAR:
    {
      _setStandSequenceWalkLeft(0, 0);
      _setStandSequenceWalkRight(0, 0);
      _setLinearLift(-55, -55, -55, -55, -45, -45, -45, -45, -35, -35, -35, -35);
    }
    break;
    case P_RECOVERLEFT:
    {
      _setStandSequenceWalkLeft(0, 0);
      _setStandSequenceWalkRight(0, 0);
      _setSequenceRecoverLift(sequenceLiftFL1, sequenceLiftFR1);
      _setSequenceRecoverLift(sequenceLiftFL2, sequenceLiftFR2);
      _setSequenceRecoverLift(sequenceLiftSL1, sequenceLiftSR1);
      _setSequenceRecoverLift(sequenceLiftSL2, sequenceLiftSR2);
      _setSequenceRecoverLift(sequenceLiftRL1, sequenceLiftRR1);
      _setSequenceRecoverLift(sequenceLiftRL2, sequenceLiftRR2);
    }
    break;
    case P_RECOVERRIGHT:
    {
      _setStandSequenceWalkLeft(0, 0);
      _setStandSequenceWalkRight(0, 0);
      _setSequenceRecoverLift(sequenceLiftFR1, sequenceLiftFL1);
      _setSequenceRecoverLift(sequenceLiftFR2, sequenceLiftFL2);
      _setSequenceRecoverLift(sequenceLiftSR1, sequenceLiftSL1);
      _setSequenceRecoverLift(sequenceLiftSR2, sequenceLiftSL2);
      _setSequenceRecoverLift(sequenceLiftRR1, sequenceLiftRL1);
      _setSequenceRecoverLift(sequenceLiftRR2, sequenceLiftRL2);
    }
    break;
    default:
    break;
  }
}

// update servo motors values
allMotors updateWalkPatterns(void) {
  // set motors angle values
  motorValue.m.fl.motor1 = sequenceFL1[mainCounter];
  motorValue.m.fl.motor2 = sequenceFL2[mainCounter];
  motorValue.m.fr.motor1 = sequenceFR1[mainCounter];
  motorValue.m.fr.motor2 = sequenceFR2[mainCounter];
  motorValue.m.sl.motor1 = sequenceSL1[mainCounter];
  motorValue.m.sl.motor2 = sequenceSL2[mainCounter];
  motorValue.m.sr.motor1 = sequenceSR1[mainCounter];
  motorValue.m.sr.motor2 = sequenceSR2[mainCounter];
  motorValue.m.rl.motor1 = sequenceRL1[mainCounter];
  motorValue.m.rl.motor2 = sequenceRL2[mainCounter];
  motorValue.m.rr.motor1 = sequenceRR1[mainCounter];
  motorValue.m.rr.motor2 = sequenceRR2[mainCounter];
  return motorValue;
}

// update servo motors values for lift
allMotors updateLiftPatterns(void) {
  // set motors angle values
  motorLift.m.fl.motor1 = sequenceLiftFL1[mainCounter];
  motorLift.m.fl.motor2 = sequenceLiftFL2[mainCounter];
  motorLift.m.fr.motor1 = sequenceLiftFR1[mainCounter];
  motorLift.m.fr.motor2 = sequenceLiftFR2[mainCounter];
  motorLift.m.sl.motor1 = sequenceLiftSL1[mainCounter];
  motorLift.m.sl.motor2 = sequenceLiftSL2[mainCounter];
  motorLift.m.sr.motor1 = sequenceLiftSR1[mainCounter];
  motorLift.m.sr.motor2 = sequenceLiftSR2[mainCounter];
  motorLift.m.rl.motor1 = sequenceLiftRL1[mainCounter];
  motorLift.m.rl.motor2 = sequenceLiftRL2[mainCounter];
  motorLift.m.rr.motor1 = sequenceLiftRR1[mainCounter];
  motorLift.m.rr.motor2 = sequenceLiftRR2[mainCounter];
  return motorLift;
}

/*
// print sequence name
void printPatternName(unsigned char currentTaskItem) {
  // get new sequence array
  switch (currentTaskItem) {
    case P_DOSTAND:
    {
      Serial.println(" P_DOSTAND ");
    }
    break;
    case P_STANDTOGO:
    {
      Serial.println(" P_STANDTOGO ");
    }
    break;
    case P_GOTOSTAND:
    {
      Serial.println(" P_GOTOSTAND ");
    }
    break;
    case P_STANDGO:
    {
      Serial.println(" P_STANDGO ");
    }
    break;
    case P_STANDGOLEFT:
    {
      Serial.println(" P_STANDGOLEFT ");
    }
    break;
    case P_STANDGORIGHT:
    {
      Serial.println(" P_STANDGORIGHT ");
    }
    break;
    case P_GOFORWARD:
    {
      Serial.println(" P_GOFORWARD ");
    }
    break;
    case P_GOFORWARDSLOW:
    {
      Serial.println(" P_GOFORWARDSLOW ");
    }
    break;
    case P_GOLEFT:
    {
      Serial.println(" P_GOLEFT ");
    }
    break;
    case P_GORIGHT:
    {
      Serial.println(" P_GORIGHT ");
    }
    break;
    case P_GOBACK:
    {
      Serial.println(" P_GOBACK ");
    }
    break;
    case P_GOBACKLEFT:
    {
      Serial.println(" P_GOBACKLEFT ");
    }
    break;
    case P_GOBACKRIGHT:
    {
      Serial.println(" P_GOBACKRIGHT ");
    }
    break;
    case P_DONE:
    {
      Serial.println(" P_DONE ");
    }
    break;
    case P_END:
    {
      Serial.println(" P_END ");
    }
    break;
    case P_RESETDIRECTION:
    {
      Serial.println(" P_RESETDIRECTION ");
    }
    case P_RESETGIRO:
    {
      Serial.println(" P_RESETGIRO ");
    }
    break;
    case P_RESTOREDIRECTION:
    {
      Serial.println(" P_RESTOREDIRECTION ");
    }
    break;
    case P_ENABLEINPUTS:
    {
      Serial.println(" P_ENABLEINPUTS ");
    }
    break;
    case P_DISABLEINPUTS:
    {
      Serial.println(" P_DISABLEINPUTS ");
    }
    break;
    case P_STANDGOSHIFTLEFT:
    {
      Serial.println(" P_STANDGOSHIFTLEFT ");
    }
    break;
    case P_STANDGOSHIFTRIGHT:
    {
      Serial.println(" P_STANDGOSHIFTRIGHT ");
    }
    break;
    case P_GOSHIFTLEFT:
    {
      Serial.println(" P_GOSHIFTLEFT ");
    }
    break;
    case P_GOSHIFTRIGHT:
    {
      Serial.println(" P_GOSHIFTRIGHT ");
    }
    break;
    case P_DODOWN:
    {
      Serial.println(" P_DODOWN ");
    }
    break;
    case P_DODOWNLEFT:
    {
      Serial.println(" P_DODOWNLEFT ");
    }
    break;
    case P_DODOWNRIGHT:
    {
      Serial.println(" P_DODOWNRIGHT ");
    }
    break;
    case P_DODOWNFRONT:
    {
      Serial.println(" P_DODOWNFRONT ");
    }
    break;
    case P_DODOWNREAR:
    {
      Serial.println(" P_DODOWNREAR ");
    }
    break;
    case P_RECOVERLEFT:
    {
      Serial.println(" P_RECOVERLEFT ");
    }
    break;
    case P_RECOVERRIGHT:
    {
      Serial.println(" P_RECOVERRIGHT ");
    }
    break;
    default:
      Serial.println(" unknown pattern ");
    break;
  }
}
*/
