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
void _updateCenterMotor(char * sequenceCenter, char cAngle) {
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
  _updateCenterMotor(sequenceCenterF, cAngle);
  _updateCenterMotor(sequenceCenterR, cAngle);
}

// update shift
void _updateShift(char cAngle) {
  _updateCenterMotor(sequenceCenterF, cAngle);
  _updateCenterMotor(sequenceCenterR, -cAngle);
}

// create walk sequence
void _setSequenceRecoverCenter(char * sequenceCenter) {
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
  _updateSequenceLiftStandWalk(sequenceLiftFL1, 0);
  _updateSequenceLiftStandWalk(sequenceLiftFL2, 0);
  _updateSequenceLiftStandWalk(sequenceLiftFR1, m_halfCycle);
  _updateSequenceLiftStandWalk(sequenceLiftFR2, m_halfCycle);
  _updateSequenceLiftStandWalk(sequenceLiftRL1, m_halfCycle);
  _updateSequenceLiftStandWalk(sequenceLiftRL2, m_halfCycle);
  _updateSequenceLiftStandWalk(sequenceLiftRR1, 0);
  _updateSequenceLiftStandWalk(sequenceLiftRR2, 0);
}

void _setWalkLiftSequence(void) {
  _updateSequenceLiftWalk1(sequenceLiftFL1, 0);
  _updateSequenceLiftWalk2(sequenceLiftFL2, 0);
  _updateSequenceLiftWalk1(sequenceLiftFR1, m_halfCycle);
  _updateSequenceLiftWalk2(sequenceLiftFR2, m_halfCycle);
  _updateSequenceLiftWalk1(sequenceLiftRL1, m_halfCycle);
  _updateSequenceLiftWalk2(sequenceLiftRL2, m_halfCycle);
  _updateSequenceLiftWalk1(sequenceLiftRR1, 0);
  _updateSequenceLiftWalk2(sequenceLiftRR2, 0);
}

void _setWalkSequence(void) {
  _updateSequenceWalk(sequenceFL1, 0);
  _updateSequenceWalk(sequenceFL2, 0);
  _updateSequenceWalk(sequenceFR1, m_halfCycle);
  _updateSequenceWalk(sequenceFR2, m_halfCycle);
  _updateSequenceWalk(sequenceRL1, m_halfCycle);
  _updateSequenceWalk(sequenceRL2, m_halfCycle);
  _updateSequenceWalk(sequenceRR1, 0);
  _updateSequenceWalk(sequenceRR2, 0);
}

void _setWalkSlowSequence(void) {
  _updateSequenceWalkSlow(sequenceFL1, 0);
  _updateSequenceWalkSlow(sequenceFL2, 0);
  _updateSequenceWalkSlow(sequenceFR1, m_halfCycle);
  _updateSequenceWalkSlow(sequenceFR2, m_halfCycle);
  _updateSequenceWalkSlow(sequenceRL1, m_halfCycle);
  _updateSequenceWalkSlow(sequenceRL2, m_halfCycle);
  _updateSequenceWalkSlow(sequenceRR1, 0);
  _updateSequenceWalkSlow(sequenceRR2, 0);
}

void _setWalkBackSequence(void) {
  _updateSequenceWalkBack(sequenceFL1, 0);
  _updateSequenceWalkBack(sequenceFL2, 0);
  _updateSequenceWalkBack(sequenceFR1, m_halfCycle);
  _updateSequenceWalkBack(sequenceFR2, m_halfCycle);
  _updateSequenceWalkBack(sequenceRL1, m_halfCycle);
  _updateSequenceWalkBack(sequenceRL2, m_halfCycle);
  _updateSequenceWalkBack(sequenceRR1, 0);
  _updateSequenceWalkBack(sequenceRR2, 0);
}

void _setStandSequenceWalk(char position1, char position2) {
  _setSequenceValue(position1, sequenceFL1);
  _setSequenceValue(position2, sequenceFL2);
  _setSequenceValue(position1, sequenceFR1);
  _setSequenceValue(position2, sequenceFR2);
  _setSequenceValue(position1, sequenceRL1);
  _setSequenceValue(position2, sequenceRL2);
  _setSequenceValue(position1, sequenceRR1);
  _setSequenceValue(position2, sequenceRR2);
}

void _setStandToGoLift(void) {
  _updateSequenceLinearLiftEnd(-m_legPatternLift, sequenceLiftFL1);
  _updateSequenceLinearLiftEnd(-m_legPatternLift, sequenceLiftFL2);
  _updateSequenceLinearLiftEnd(0, sequenceLiftFR1);
  _updateSequenceLinearLiftEnd(0, sequenceLiftFR2);
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
  _updateSequenceLinearLiftStart(0, sequenceLiftRL1);
  _updateSequenceLinearLiftStart(0, sequenceLiftRL2);
  _updateSequenceLinearLiftStart(0, sequenceLiftRR1);
  _updateSequenceLinearLiftStart(0, sequenceLiftRR2);
}

void _setLinearLift(char FL1, char FL2, char FR1, char FR2, char RL1, char RL2, char RR1, char RR2) {
  _updateSequenceLinearLift(FL1, sequenceLiftFL1);
  _updateSequenceLinearLift(FL2, sequenceLiftFL2);
  _updateSequenceLinearLift(FR1, sequenceLiftFR1);
  _updateSequenceLinearLift(FR2, sequenceLiftFR2);
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
      _setStandSequenceWalk(0, 0);
      _setLinearLift(0, 0, 0, 0, 0, 0, 0, 0);
      _updateTurn(0);
    }
    break;
    case P_STANDTOGO:
    {
      _setStandSequenceWalk(0, 0);
      _setStandToGoLift();
      _updateTurn(0);
    }
    break;
    case P_GOTOSTAND:
    {
      _setStandSequenceWalk(0, 0);
      _setGoToStandLift();
      _updateTurn(0);
    }
    break;
    case P_STANDGO:
    {
      _setStandSequenceWalk(0, 0);
      _setStandWalkLiftSequence();
      _updateTurn(angleTurn);
    }
    break;
    case P_STANDGOLEFT:
    {
      _setStandSequenceWalk(0, 0);
      _setStandWalkLiftSequence();
      _updateTurn(-10);
    }
    break;
    case P_STANDGORIGHT:
    {
      _setStandSequenceWalk(0, 0);
      _setStandWalkLiftSequence();
      _updateTurn(10);
    }
    break;
    case P_STANDGOSHIFTLEFT:
    {
      _setStandSequenceWalk(0, 0);
      _setStandWalkLiftSequence();
      _updateShift(-10);
    }
    break;
    case P_STANDGOSHIFTRIGHT:
    {
      _setStandSequenceWalk(0, 0);
      _setStandWalkLiftSequence();
      _updateShift(10);
    }
    break;
    case P_GOFORWARD:
    {
      if (m_fastWalkingPatternEnabled) {
        _setWalkSequence();
      } else {
        _setWalkSlowSequence();
      }
      _setWalkLiftSequence();
      _updateTurn(angleTurn);
    }
    break;
    case P_GOFORWARDSLOW:
    {
      _setWalkSlowSequence();
      _setWalkLiftSequence();
      _updateTurn(angleTurn);
    }
    break;
    case P_GOLEFT:
    {
      if (m_fastWalkingPatternEnabled) {
        _setWalkSequence();
      } else {
        _setWalkSlowSequence();
      }
      _setWalkLiftSequence();
      _updateTurn(-10);
    }
    break;
    case P_GORIGHT:
    {
      if (m_fastWalkingPatternEnabled) {
        _setWalkSequence();
      } else {
        _setWalkSlowSequence();
      }
      _setWalkLiftSequence();
      _updateTurn(10);
    }
    break;
    case P_GOSHIFTLEFT:
    {
      if (m_fastWalkingPatternEnabled) {
        _setWalkSequence();
      } else {
        _setWalkSlowSequence();
      }
      _setWalkLiftSequence();
      _updateShift(-10);
    }
    break;
    case P_GOSHIFTRIGHT:
    {
      if (m_fastWalkingPatternEnabled) {
        _setWalkSequence();
      } else {
        _setWalkSlowSequence();
      }
      _setWalkLiftSequence();
      _updateShift(10);
    }
    break;
    case P_GOBACK:
    {
      _setWalkBackSequence();
      _setStandWalkLiftSequence();
      _updateTurn(0);
    }
    break;
    case P_GOBACKLEFT:
    {
      _setWalkBackSequence();
      _setStandWalkLiftSequence();
      _updateTurn(-10);
    }
    break;
    case P_GOBACKRIGHT:
    {
      _setWalkBackSequence();
      _setStandWalkLiftSequence();
      _updateTurn(10);
    }
    break;
    case P_DODOWN:
    {
      _setStandSequenceWalk(0, 0);
      _setLinearLift(-30, -30, -30, -30, -30, -30, -30, -30);
      _updateTurn(0);
    }
    break;
    case P_DODOWNLEFT:
    {
      _setStandSequenceWalk(0, 0);
      _setLinearLift(-30, -30, -45, -45, -30, -30, -45, -45);
      _updateTurn(0);
    }
    break;
    case P_DODOWNRIGHT:
    {
      _setStandSequenceWalk(0, 0);
      _setLinearLift(-45, -45, -30, -30, -45, -45, -30, -30);
      _updateTurn(0);
    }
    break;
    case P_DODOWNFRONT:
    {
      _setStandSequenceWalk(0, 0);
      _setLinearLift(-30, -30, -30, -30, -45, -45, -45, -45);
      _updateTurn(0);
    }
    break;
    case P_DODOWNREAR:
    {
      _setStandSequenceWalk(0, 0);
      _setLinearLift(-45, -45, -45, -45, -30, -30, -30, -30);
      _updateTurn(0);
    }
    break;
    case P_RECOVERLEFT:
    {
      _setStandSequenceWalk(0, 0);
      _setSequenceRecoverLift(sequenceLiftFL1, sequenceLiftFR1);
      _setSequenceRecoverLift(sequenceLiftFL2, sequenceLiftFR2);
      _setSequenceRecoverLift(sequenceLiftRL1, sequenceLiftRR1);
      _setSequenceRecoverLift(sequenceLiftRL2, sequenceLiftRR2);
      _setSequenceRecoverCenter(sequenceCenterF);
      _setSequenceRecoverCenter(sequenceCenterR);
    }
    break;
    case P_RECOVERRIGHT:
    {
      _setStandSequenceWalk(0, 0);
      _setSequenceRecoverLift(sequenceLiftFR1, sequenceLiftFL1);
      _setSequenceRecoverLift(sequenceLiftFR2, sequenceLiftFL2);
      _setSequenceRecoverLift(sequenceLiftRR1, sequenceLiftRL1);
      _setSequenceRecoverLift(sequenceLiftRR2, sequenceLiftRL2);
      _setSequenceRecoverCenter(sequenceCenterF);
      _setSequenceRecoverCenter(sequenceCenterR);
    }
    break;
    default:
    break;
  }
}

// update servo motors values
allMotors updateWalkPatterns(void) {
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

// update servo motors values for lift
allMotors updateLiftPatterns(void) {
  // set motors angle values
  motorLift.m.st.motor1 = 0;
  motorLift.m.st.motor2 = 0;
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
