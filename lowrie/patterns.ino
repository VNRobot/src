/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Robot legs motion patterns
*/

char cBuffer;
// points to currentSequence for every leg
unsigned char mainCounter = 0;
// 24 positions per sequence
char sequenceCenterF[24] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
char sequenceCenterR[24] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
char sequenceFL1[24] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
char sequenceFL2[24] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
char sequenceFR1[24] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
char sequenceFR2[24] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
char sequenceRL1[24] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
char sequenceRL2[24] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
char sequenceRR1[24] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
char sequenceRR2[24] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
// sequence for leg lift
char sequenceLiftFL1[24] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
char sequenceLiftFL2[24] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
char sequenceLiftFR1[24] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
char sequenceLiftFR2[24] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
char sequenceLiftRL1[24] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
char sequenceLiftRL2[24] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
char sequenceLiftRR1[24] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
char sequenceLiftRR2[24] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
// motors values for 10 motors
allMotors motorValue = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
allMotors motorLift = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
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

// set new values of center motors
void _setCenterMotor(char * sequenceCenter, char cAngle) {
  sequenceCenter[0] = - cAngle / 2;
  for (i = 1; i < m_halfCycle - 1; i++) {
    sequenceCenter[i] = - cAngle;
  }
  sequenceCenter[m_halfCycle - 1] = - cAngle / 2;
  sequenceCenter[m_halfCycle] = cAngle / 2;
  for (i = m_halfCycle + 1; i < m_fullCycle - 1; i++) {
    sequenceCenter[i] = cAngle;
  }
  sequenceCenter[m_fullCycle - 1] = cAngle / 2;
}

// update turning
void _updateTurn(char cAngle) {
  _setCenterMotor(sequenceCenterF, cAngle);
  _setCenterMotor(sequenceCenterR, cAngle);
}

// update shift
void _updateShift(char cAngle) {
  _setCenterMotor(sequenceCenterF, cAngle);
  _setCenterMotor(sequenceCenterR, -cAngle);
}

// create walk sequence
void _setRecoverCenterMotor(char * sequenceCenter) {
  // motor center
  for (i = 0; i < m_halfCycle + 2; i++) {
    sequenceCenter[i] = -40;
  }
  for (i = m_halfCycle + 2; i < m_fullCycle - 2; i++) {
    sequenceCenter[i] = 50;
  }
  sequenceCenter[m_fullCycle - 2] = 20;
  sequenceCenter[m_fullCycle - 1] = 0;
}

// create recover sequence
void _setRecoverLift(char * sequence1, char * sequence2) {
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

// linear motor movement from point to point
void _setLinearValue(char endPosition, char * sequence) {
  cBuffer = sequence[m_fullCycle - 1];
  sequence[0] = cBuffer;
  for (i = 1; i < (m_fullCycle - 1); i++) {
    sequence[i] = cBuffer + ((endPosition - cBuffer) * i) / m_fullCycle;
  }
  sequence[m_fullCycle - 1] = endPosition;
}

// linear motor movement from point to point at the end of sequence
void _setLinearValueEnd(char endPosition, char * sequence) {
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
void _setLinearValueStart(char endPosition, char * sequence) {
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
void _setWalkLift(char * lift, unsigned char shift) {
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
void _setWalkLiftHigh1(char * lift, unsigned char shift) {
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
void _setWalkLiftHigh2(char * lift, unsigned char shift) {
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
void _setForwardWalk(char * sequence, unsigned char shift) {
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
void _setForwardWalkSlow(char * sequence, unsigned char shift) {
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
void _setForwardWalkBack(char * sequence, unsigned char shift) {
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

void _setWalkLiftSequence(void) {
  _setWalkLift(sequenceLiftFL1, 0);
  _setWalkLift(sequenceLiftFL2, 0);
  _setWalkLift(sequenceLiftFR1, m_halfCycle);
  _setWalkLift(sequenceLiftFR2, m_halfCycle);
  _setWalkLift(sequenceLiftRL1, m_halfCycle);
  _setWalkLift(sequenceLiftRL2, m_halfCycle);
  _setWalkLift(sequenceLiftRR1, 0);
  _setWalkLift(sequenceLiftRR2, 0);
}

void _setWalkLiftHighSequence(void) {
  _setWalkLiftHigh1(sequenceLiftFL1, 0);
  _setWalkLiftHigh2(sequenceLiftFL2, 0);
  _setWalkLiftHigh1(sequenceLiftFR1, m_halfCycle);
  _setWalkLiftHigh2(sequenceLiftFR2, m_halfCycle);
  _setWalkLiftHigh1(sequenceLiftRL1, m_halfCycle);
  _setWalkLiftHigh2(sequenceLiftRL2, m_halfCycle);
  _setWalkLiftHigh1(sequenceLiftRR1, 0);
  _setWalkLiftHigh2(sequenceLiftRR2, 0);
}

void _setWalkFastSequenceL(void) {
  _setForwardWalk(sequenceFL1, 0);
  _setForwardWalk(sequenceFL2, 0);
  _setForwardWalk(sequenceRL1, m_halfCycle);
  _setForwardWalk(sequenceRL2, m_halfCycle);
}

void _setWalkFastSequenceR(void) {
  _setForwardWalk(sequenceFR1, m_halfCycle);
  _setForwardWalk(sequenceFR2, m_halfCycle);
  _setForwardWalk(sequenceRR1, 0);
  _setForwardWalk(sequenceRR2, 0);
}

void _setWalkSlowSequenceL(void) {
  _setForwardWalkSlow(sequenceFL1, 0);
  _setForwardWalkSlow(sequenceFL2, 0);
  _setForwardWalkSlow(sequenceRL1, m_halfCycle);
  _setForwardWalkSlow(sequenceRL2, m_halfCycle);
}

void _setWalkSlowSequenceR(void) {
  _setForwardWalkSlow(sequenceFR1, m_halfCycle);
  _setForwardWalkSlow(sequenceFR2, m_halfCycle);
  _setForwardWalkSlow(sequenceRR1, 0);
  _setForwardWalkSlow(sequenceRR2, 0);
}

void _setWalkBackSequenceL(void) {
  _setForwardWalkBack(sequenceFL1, 0);
  _setForwardWalkBack(sequenceFL2, 0);
  _setForwardWalkBack(sequenceRL1, m_halfCycle);
  _setForwardWalkBack(sequenceRL2, m_halfCycle);
}

void _setWalkBackSequenceR(void) {
  _setForwardWalkBack(sequenceFR1, m_halfCycle);
  _setForwardWalkBack(sequenceFR2, m_halfCycle);
  _setForwardWalkBack(sequenceRR1, 0);
  _setForwardWalkBack(sequenceRR2, 0);
}

void _setWalkStandSequenceL(char position1, char position2) {
  _setLinearValue(position1, sequenceFL1);
  _setLinearValue(position2, sequenceFL2);
  _setLinearValue(position1, sequenceRL1);
  _setLinearValue(position2, sequenceRL2);
}

void _setWalkStandSequenceR(char position1, char position2) {
  _setLinearValue(position1, sequenceFR1);
  _setLinearValue(position2, sequenceFR2);
  _setLinearValue(position1, sequenceRR1);
  _setLinearValue(position2, sequenceRR2);
}

void _setStandToGoLiftSequence(void) {
  _setLinearValueEnd(-m_legPatternLift, sequenceLiftFL1);
  _setLinearValueEnd(-m_legPatternLift, sequenceLiftFL2);
  _setLinearValueEnd(0, sequenceLiftFR1);
  _setLinearValueEnd(0, sequenceLiftFR2);
  _setLinearValueEnd(0, sequenceLiftRL1);
  _setLinearValueEnd(0, sequenceLiftRL2);
  _setLinearValueEnd(-m_legPatternLift, sequenceLiftRR1);
  _setLinearValueEnd(-m_legPatternLift, sequenceLiftRR2);
}

void _setGoToStandLiftSequence(void) {
  _setLinearValueStart(0, sequenceLiftFL1);
  _setLinearValueStart(0, sequenceLiftFL2);
  _setLinearValueStart(0, sequenceLiftFR1);
  _setLinearValueStart(0, sequenceLiftFR2);
  _setLinearValueStart(0, sequenceLiftRL1);
  _setLinearValueStart(0, sequenceLiftRL2);
  _setLinearValueStart(0, sequenceLiftRR1);
  _setLinearValueStart(0, sequenceLiftRR2);
}

void _setLinearLiftSequence(char FL1, char FL2, char FR1, char FR2, char RL1, char RL2, char RR1, char RR2) {
  _setLinearValue(FL1, sequenceLiftFL1);
  _setLinearValue(FL2, sequenceLiftFL2);
  _setLinearValue(FR1, sequenceLiftFR1);
  _setLinearValue(FR2, sequenceLiftFR2);
  _setLinearValue(RL1, sequenceLiftRL1);
  _setLinearValue(RL2, sequenceLiftRL2);
  _setLinearValue(RR1, sequenceLiftRR1);
  _setLinearValue(RR2, sequenceLiftRR2);
}

// get next sequence
void setPattern(unsigned char currentTaskItem, char angleTurn) {
  // get new sequence array
  switch (currentTaskItem) {
    case P_DOSTAND:
    {
      _setWalkStandSequenceL(0, 0);
      _setWalkStandSequenceR(0, 0);
      _setLinearLiftSequence(0, 0, 0, 0, 0, 0, 0, 0);
      _updateTurn(0);
    }
    break;
    case P_STANDTOGO:
    {
      _setWalkStandSequenceL(0, 0);
      _setWalkStandSequenceR(0, 0);
      _setStandToGoLiftSequence();
      _updateTurn(0);
    }
    break;
    case P_GOTOSTAND:
    {
      _setWalkStandSequenceL(0, 0);
      _setWalkStandSequenceR(0, 0);
      _setGoToStandLiftSequence();
      _updateTurn(0);
    }
    break;
    case P_STANDGO:
    {
      _setWalkStandSequenceL(0, 0);
      _setWalkStandSequenceR(0, 0);
      _setWalkLiftSequence();
      _updateTurn(angleTurn);
    }
    break;
    case P_STANDGOLEFT:
    {
      _setWalkStandSequenceL(0, 0);
      _setWalkStandSequenceR(0, 0);
      _setWalkLiftSequence();
      _updateTurn(-10);
    }
    break;
    case P_STANDGORIGHT:
    {
      _setWalkStandSequenceL(0, 0);
      _setWalkStandSequenceR(0, 0);
      _setWalkLiftSequence();
      _updateTurn(10);
    }
    break;
    case P_STANDGOSHIFTLEFT:
    {
      _setWalkStandSequenceL(0, 0);
      _setWalkStandSequenceR(0, 0);
      _setWalkLiftSequence();
      _updateShift(-10);
    }
    break;
    case P_STANDGOSHIFTRIGHT:
    {
      _setWalkStandSequenceL(0, 0);
      _setWalkStandSequenceR(0, 0);
      _setWalkLiftSequence();
      _updateShift(10);
    }
    break;
    case P_GOFORWARD:
    {
      if (m_fastWalkingPatternEnabled) {
        _setWalkFastSequenceL();
        _setWalkFastSequenceR();
      } else {
        _setWalkSlowSequenceL();
        _setWalkSlowSequenceR();
      }
      _setWalkLiftHighSequence();
      _updateTurn(angleTurn);
    }
    break;
    case P_GOFORWARDSLOW:
    {
      _setWalkSlowSequenceL();
      _setWalkSlowSequenceR();
      _setWalkLiftHighSequence();
      _updateTurn(angleTurn);
    }
    break;
    case P_GOLEFT:
    {
      if (m_fastWalkingPatternEnabled) {
        _setWalkFastSequenceL();
        _setWalkFastSequenceR();
      } else {
        _setWalkSlowSequenceL();
        _setWalkSlowSequenceR();
      }
      _setWalkLiftHighSequence();
      _updateTurn(-10);
    }
    break;
    case P_GORIGHT:
    {
      if (m_fastWalkingPatternEnabled) {
        _setWalkFastSequenceL();
        _setWalkFastSequenceR();
      } else {
        _setWalkSlowSequenceL();
        _setWalkSlowSequenceR();
      }
      _setWalkLiftHighSequence();
      _updateTurn(10);
    }
    break;
    case P_GOSHIFTLEFT:
    {
      if (m_fastWalkingPatternEnabled) {
        _setWalkFastSequenceL();
        _setWalkFastSequenceR();
      } else {
        _setWalkSlowSequenceL();
        _setWalkSlowSequenceR();
      }
      _setWalkLiftHighSequence();
      _updateShift(-10);
    }
    break;
    case P_GOSHIFTRIGHT:
    {
      if (m_fastWalkingPatternEnabled) {
        _setWalkFastSequenceL();
        _setWalkFastSequenceR();
      } else {
        _setWalkSlowSequenceL();
        _setWalkSlowSequenceR();
      }
      _setWalkLiftHighSequence();
      _updateShift(10);
    }
    break;
    case P_GOBACK:
    {
      _setWalkBackSequenceL();
      _setWalkBackSequenceR();
      _setWalkLiftSequence();
      _updateTurn(0);
    }
    break;
    case P_GOBACKLEFT:
    {
      _setWalkBackSequenceL();
      _setWalkBackSequenceR();
      _setWalkLiftSequence();
      _updateTurn(-10);
    }
    break;
    case P_GOBACKRIGHT:
    {
      _setWalkBackSequenceL();
      _setWalkBackSequenceR();
      _setWalkLiftSequence();
      _updateTurn(10);
    }
    break;
    case P_DODOWN:
    {
      _setWalkStandSequenceL(0, 0);
      _setWalkStandSequenceR(0, 0);
      _setLinearLiftSequence(-30, -30, -30, -30, -30, -30, -30, -30);
      _updateTurn(0);
    }
    break;
    case P_DODOWNLEFT:
    {
      _setWalkStandSequenceL(0, 0);
      _setWalkStandSequenceR(0, 0);
      _setLinearLiftSequence(-30, -30, -45, -45, -30, -30, -45, -45);
      _updateTurn(0);
    }
    break;
    case P_DODOWNRIGHT:
    {
      _setWalkStandSequenceL(0, 0);
      _setWalkStandSequenceR(0, 0);
      _setLinearLiftSequence(-45, -45, -30, -30, -45, -45, -30, -30);
      _updateTurn(0);
    }
    break;
    case P_DODOWNFRONT:
    {
      _setWalkStandSequenceL(0, 0);
      _setWalkStandSequenceR(0, 0);
      _setLinearLiftSequence(-30, -30, -30, -30, -45, -45, -45, -45);
      _updateTurn(0);
    }
    break;
    case P_DODOWNREAR:
    {
      _setWalkStandSequenceL(0, 0);
      _setWalkStandSequenceR(0, 0);
      _setLinearLiftSequence(-45, -45, -45, -45, -30, -30, -30, -30);
      _updateTurn(0);
    }
    break;
    case P_RECOVERLEFT:
    {
      _setWalkStandSequenceL(0, 0);
      _setWalkStandSequenceR(0, 0);
      _setRecoverLift(sequenceLiftFL1, sequenceLiftFR1);
      _setRecoverLift(sequenceLiftFL2, sequenceLiftFR2);
      _setRecoverLift(sequenceLiftRL1, sequenceLiftRR1);
      _setRecoverLift(sequenceLiftRL2, sequenceLiftRR2);
      _setRecoverCenterMotor(sequenceCenterF);
      _setRecoverCenterMotor(sequenceCenterR);
    }
    break;
    case P_RECOVERRIGHT:
    {
      _setWalkStandSequenceL(0, 0);
      _setWalkStandSequenceR(0, 0);
      _setRecoverLift(sequenceLiftFR1, sequenceLiftFL1);
      _setRecoverLift(sequenceLiftFR2, sequenceLiftFL2);
      _setRecoverLift(sequenceLiftRR1, sequenceLiftRL1);
      _setRecoverLift(sequenceLiftRR2, sequenceLiftRL2);
      _setRecoverCenterMotor(sequenceCenterF);
      _setRecoverCenterMotor(sequenceCenterR);
    }
    break;
    default:
    break;
  }
}

// get servo motors values
allMotors getWalkPatterns(void) {
  // set motors angle values
  motorValue.m.st.motor1 = sequenceCenterF[mainCounter];
  motorValue.m.st.motor2 = sequenceCenterR[mainCounter];
  motorValue.m.fl.motor1 = sequenceFL1[mainCounter];
  motorValue.m.fl.motor2 = sequenceFL2[mainCounter];
  motorValue.m.fr.motor1 = sequenceFR1[mainCounter];
  motorValue.m.fr.motor2 = sequenceFR2[mainCounter];
  motorValue.m.rl.motor1 = sequenceRL1[mainCounter];
  motorValue.m.rl.motor2 = sequenceRL2[mainCounter];
  motorValue.m.rr.motor1 = sequenceRR1[mainCounter];
  motorValue.m.rr.motor2 = sequenceRR2[mainCounter];
  return motorValue;
}

// get servo motors values for lift
allMotors getLiftPatterns(void) {
  // set motors angle values
  motorLift.m.fl.motor1 = sequenceLiftFL1[mainCounter];
  motorLift.m.fl.motor2 = sequenceLiftFL2[mainCounter];
  motorLift.m.fr.motor1 = sequenceLiftFR1[mainCounter];
  motorLift.m.fr.motor2 = sequenceLiftFR2[mainCounter];
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
