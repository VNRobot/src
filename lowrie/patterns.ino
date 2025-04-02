/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Robot legs motion patterns
*/

// legs1.1 slower walking
char motor1Walk[25] =  { 21,  21,  21,  21,  22,  24,  26,  28,  30,  34,  38,  42,  45,  50,  55,  60,  65,  70,  75,  81,  87,  93, 100, 109, 119};
char motor2Walk[25] =  {119, 109, 100,  93,  87,  81,  75,  70,  65,  60,  55,  50,  45,  42,  38,  34,  30,  28,  26,  24,  22,  21,  21,  21,  21};
// char buffer for temporary use
char cBuffer1;
char cBuffer2;
// leg lift angles
unsigned char _liftm = 16;
// center motor default position
char centerF = 0; // positive more distance between legs
char centerR = 0; // positive more distance between legs
// points to currentSequence for every leg
unsigned char mainCounter = 0;
// 20 positions per sequence
motors sequenceCenter[20] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
motors sequenceFL[20] = {30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30};
motors sequenceFR[20] = {30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30};
motors sequenceRL[20] = {30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30};
motors sequenceRR[20] = {30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30};
// motors values for 10 motors
allMotors motorValue = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
// full cycle
unsigned char fullCycle = 20;
// pointer to array
unsigned char apoint = 0;

// set new values of center motors
void _updateCenterMotor(char cAngleF, char cAngleR) {
  sequenceCenter[0].motor1 = centerF - cAngleF/2;
  sequenceCenter[0].motor2 = centerR - cAngleR/2;
  for (i = 1; i < 9; i++) {
    sequenceCenter[i].motor1 = centerF - cAngleF;
    sequenceCenter[i].motor2 = centerR - cAngleR;
  }
  sequenceCenter[9].motor1 = centerF - cAngleF/2;
  sequenceCenter[9].motor2 = centerR - cAngleR/2;
  sequenceCenter[10].motor1 = centerF + cAngleF/2;
  sequenceCenter[10].motor2 = centerR + cAngleR/2;
  for (i = 11; i < 19; i++) {
    sequenceCenter[i].motor1 = centerF + cAngleF;
    sequenceCenter[i].motor2 = centerR + cAngleR;
  }
  sequenceCenter[19].motor1 = centerF + cAngleF/2;
  sequenceCenter[19].motor2 = centerR + cAngleR/2;
}

// update turning
void _updateTurn(char cAngle) {
  _updateCenterMotor(cAngle, cAngle);
}

// update shift
void _updateShift(char cAngle) {
  _updateCenterMotor(cAngle, -cAngle);
}

// linear motor movement from point to point
void _updateSequenceLinear(char endPosition1, char endPosition2, motors * sequence) {
  cBuffer1 = sequence[fullCycle - 1].motor1;
  cBuffer2 = sequence[fullCycle - 1].motor2;
  sequence[0].motor1 = cBuffer1;
  sequence[0].motor2 = cBuffer2;
  for (i = 1; i < (fullCycle - 1); i++) {
    sequence[i].motor1 = cBuffer1 + ((endPosition1 - cBuffer1) * i) / fullCycle;
    sequence[i].motor2 = cBuffer2 + ((endPosition2 - cBuffer2) * i) / fullCycle;
  }
  sequence[fullCycle - 1].motor1 = endPosition1;
  sequence[fullCycle - 1].motor2 = endPosition2;
}

// linear motor movement from point to point at the end of sequence
void _updateSequenceLinearEnd(char endPosition1, char endPosition2, motors * sequence) {
  cBuffer1 = sequence[fullCycle - 1].motor1;
  cBuffer2 = sequence[fullCycle - 1].motor2;
  for (i = 0; i < (fullCycle - 4); i++) {
    sequence[i].motor1 = cBuffer1;
    sequence[i].motor2 = cBuffer2;
  }
  sequence[fullCycle - 4].motor1 = cBuffer1 + (endPosition1 - cBuffer1) / 4;
  sequence[fullCycle - 3].motor1 = cBuffer1 + ((endPosition1 - cBuffer1) * 2) / 4;
  sequence[fullCycle - 2].motor1 = cBuffer1 + ((endPosition1 - cBuffer1) * 3) / 4;
  sequence[fullCycle - 1].motor1 = endPosition1;
  sequence[fullCycle - 4].motor2 = cBuffer2 + (endPosition2 - cBuffer2) / 4;
  sequence[fullCycle - 3].motor2 = cBuffer2 + ((endPosition2 - cBuffer2) * 2) / 4;
  sequence[fullCycle - 2].motor2 = cBuffer2 + ((endPosition2 - cBuffer2) * 3) / 4;
  sequence[fullCycle - 1].motor2 = endPosition2;
}

// linear motor movement from point to point at the start of sequence
void _updateSequenceLinearStart(char endPosition1, char endPosition2, motors * sequence) {
  cBuffer1 = sequence[fullCycle - 1].motor1;
  cBuffer2 = sequence[fullCycle - 1].motor2;
  sequence[0].motor1 = cBuffer1;
  sequence[1].motor1 = cBuffer1 + (endPosition1 - cBuffer1) / 4;
  sequence[2].motor1 = cBuffer1 + ((endPosition1 - cBuffer1) * 2) / 4;
  sequence[3].motor1 = cBuffer1 + ((endPosition1 - cBuffer1) * 3) / 4;
  sequence[0].motor2 = cBuffer2;
  sequence[1].motor2 = cBuffer2 + (endPosition2 - cBuffer2) / 4;
  sequence[2].motor2 = cBuffer2 + ((endPosition2 - cBuffer2) * 2) / 4;
  sequence[3].motor2 = cBuffer2 + ((endPosition2 - cBuffer2) * 3) / 4;
  for (i = 4; i < fullCycle; i++) {
    sequence[i].motor1 = endPosition1;
    sequence[i].motor2 = endPosition2;
  }
}

void _updateApointer(unsigned char pointer, unsigned char shift) {
  apoint = pointer + shift;
  if (apoint >= fullCycle) {
    apoint -= fullCycle;
  }
}

// create stand walk sequence
void _updateSequenceStandWalk(char m1, char m2, motors * sequence, unsigned char shift) {
  _updateApointer(0, shift);
  sequence[apoint].motor1 = m1 - _liftm;
  sequence[apoint].motor2 = m2 - _liftm;
  _updateApointer(1, shift);
  sequence[apoint].motor1 = m1 - _liftm / 2;
  sequence[apoint].motor2 = m2 - _liftm / 2;
  _updateApointer(2, shift);
  sequence[apoint].motor1 = m1 - _liftm / 4;
  sequence[apoint].motor2 = m2 - _liftm / 4;
  for (i = 3; i < (fullCycle - 2); i++) {
    _updateApointer(i, shift);
    sequence[apoint].motor1 = m1;
    sequence[apoint].motor2 = m2;
  }
  _updateApointer(fullCycle - 2, shift);
  sequence[apoint].motor1 = m1 - _liftm / 2;
  sequence[apoint].motor2 = m2 - _liftm / 2;
  _updateApointer(fullCycle - 1, shift);
  sequence[apoint].motor1 = m1 - _liftm;
  sequence[apoint].motor2 = m2 - _liftm;
}

// create walk sequence
void _updateSequenceWalk(unsigned char center, char m1[], char m2[], motors * sequence, unsigned char shift) {
  _updateApointer(0, shift);
  sequence[apoint].motor1 = m1[center] - _liftm;
  sequence[apoint].motor2 = m2[center] - _liftm;
  _updateApointer(1, shift);
  sequence[apoint].motor1 = m1[center - 5] - _liftm;
  sequence[apoint].motor2 = m2[center - 5] - _liftm;
  _updateApointer(2, shift);
  sequence[apoint].motor1 = m1[center - 8] - _liftm;
  sequence[apoint].motor2 = m2[center - 8];
  for (i = 3; i < 18; i++) {
    _updateApointer(i, shift);
    sequence[apoint].motor1 = m1[center - 10 + i];
    sequence[apoint].motor2 = m2[center - 10 + i];
  }
  _updateApointer(18, shift);
  sequence[apoint].motor1 = m1[center + 8];
  sequence[apoint].motor2 = m2[center + 8] - _liftm;
  _updateApointer(19, shift);
  sequence[apoint].motor1 = m1[center + 3] - _liftm;
  sequence[apoint].motor2 = m2[center + 3] - _liftm;
}

// create slow walk sequence
void _updateSequenceWalkSlow(unsigned char center, char m1[], char m2[], motors * sequence, unsigned char shift) {
  _updateApointer(0, shift);
  sequence[apoint].motor1 = m1[center] - _liftm;
  sequence[apoint].motor2 = m2[center] - _liftm;
  _updateApointer(1, shift);
  sequence[apoint].motor1 = m1[center - 2] - _liftm;
  sequence[apoint].motor2 = m2[center - 2] - _liftm;
  _updateApointer(2, shift);
  sequence[apoint].motor1 = m1[center - 4] - _liftm;
  sequence[apoint].motor2 = m2[center - 4];
  for (i = 3; i < 11; i++) {
    _updateApointer(i, shift);
    sequence[apoint].motor1 = m1[center + (i - 10) / 2];
    sequence[apoint].motor2 = m2[center + (i - 10) / 2];
  }
  for (i = 11; i < 18; i++) {
    _updateApointer(i, shift);
    sequence[apoint].motor1 = m1[center + (i - 9) / 2];
    sequence[apoint].motor2 = m2[center + (i - 9) / 2];
  }
  _updateApointer(18, shift);
  sequence[apoint].motor1 = m1[center + 4];
  sequence[apoint].motor2 = m2[center + 4] - _liftm;
  _updateApointer(19, shift);
  sequence[apoint].motor1 = m1[center + 2] - _liftm;
  sequence[apoint].motor2 = m2[center + 2] - _liftm;
}

// create slow walk sequence
void _updateSequenceWalkBack(unsigned char center, char m1[], char m2[], motors * sequence, unsigned char shift) {
  _updateApointer(0, shift);
  sequence[apoint].motor1 = m1[center] - _liftm;
  sequence[apoint].motor2 = m2[center] - _liftm;
  _updateApointer(1, shift);
  sequence[apoint].motor1 = m1[center + 2] - _liftm;
  sequence[apoint].motor2 = m2[center + 2] - _liftm;
  _updateApointer(2, shift);
  sequence[apoint].motor1 = m1[center + 4] - _liftm;
  sequence[apoint].motor2 = m2[center + 4];
  for (i = 3; i < 11; i++) {
    _updateApointer(i, shift);
    sequence[apoint].motor1 = m1[center + (10 - i) / 2];
    sequence[apoint].motor2 = m2[center + (10 - i) / 2];
  }
  for (i = 11; i < 18; i++) {
    _updateApointer(i, shift);
    sequence[apoint].motor1 = m1[center + (9 - i) / 2];
    sequence[apoint].motor2 = m2[center + (9 - i) / 2];
  }
  _updateApointer(18, shift);
  sequence[apoint].motor1 = m1[center - 4];
  sequence[apoint].motor2 = m2[center - 4] - _liftm;
  _updateApointer(19, shift);
  sequence[apoint].motor1 = m1[center - 2] - _liftm;
  sequence[apoint].motor2 = m2[center - 2] - _liftm;
}

// create walk sequence
void _setSequenceRecoverCenter(void) {
  // motor center
  for (i = 0; i < 12; i++) {
    sequenceCenter[i].motor1 = -40; sequenceCenter[i].motor2 = -40;
  }
  for (i = 12; i < 18; i++) {
    sequenceCenter[i].motor1 = 60; sequenceCenter[i].motor2 = 60;
  }
  sequenceCenter[18].motor1 = 20; sequenceCenter[18].motor2 = 20;
  sequenceCenter[19].motor1 = 0; sequenceCenter[19].motor2 = 0;
}

// create walk sequence
void _setSequenceRecover(motors * sequence1, motors * sequence2) {
  // leg 1
  sequence1[0].motor1 = 30; sequence1[0].motor2 = 30;
  sequence1[1].motor1 = 0; sequence1[1].motor2 = 0;
  for (i = 2; i < 18; i++) {
    sequence1[i].motor1 = -40; sequence1[i].motor2 = -40;
  }
  sequence1[18].motor1 = 0; sequence1[18].motor2 = 0;
  sequence1[19].motor1 = 30; sequence1[19].motor2 = 30;
  // leg 2
  sequence2[0].motor1 = 30; sequence2[0].motor2 = 30;
  sequence2[1].motor1 = 15; sequence2[1].motor2 = 15;
  sequence2[2].motor1 = 0; sequence2[2].motor2 = 0;
  sequence2[3].motor1 = 0; sequence2[3].motor2 = 0;
  sequence2[4].motor1 = 0; sequence2[4].motor2 = 0;
  sequence2[5].motor1 = 0; sequence2[5].motor2 = 0;
  for (i = 6; i < 18; i++) {
    sequence2[i].motor1 = 90; sequence2[i].motor2 = 90;
  }
  sequence2[18].motor1 = 60; sequence2[18].motor2 = 60;
  sequence2[19].motor1 = 30; sequence2[19].motor2 = 30;
}

void _setStandWalkSequence(unsigned char centerStatic) {
  _updateSequenceStandWalk(motor1Walk[centerStatic], motor2Walk[centerStatic], & sequenceFL[0], 0);
  _updateSequenceStandWalk(motor1Walk[centerStatic], motor2Walk[centerStatic], & sequenceFR[0], 10);
  _updateSequenceStandWalk(motor1Walk[centerStatic], motor2Walk[centerStatic], & sequenceRL[0], 10);
  _updateSequenceStandWalk(motor1Walk[centerStatic], motor2Walk[centerStatic], & sequenceRR[0], 0);
}

void _setWalkSequence(unsigned char centerStatic) {
  _updateSequenceWalk(centerStatic, motor1Walk, motor2Walk, & sequenceFL[0], 0);
  _updateSequenceWalk(centerStatic, motor1Walk, motor2Walk, & sequenceFR[0], 10);
  _updateSequenceWalk(centerStatic, motor1Walk, motor2Walk, & sequenceRL[0], 10);
  _updateSequenceWalk(centerStatic, motor1Walk, motor2Walk, & sequenceRR[0], 0);
}

void _setWalkSlowSequence(unsigned char centerStatic) {
  _updateSequenceWalkSlow(centerStatic, motor1Walk, motor2Walk, & sequenceFL[0], 0);
  _updateSequenceWalkSlow(centerStatic, motor1Walk, motor2Walk, & sequenceFR[0], 10);
  _updateSequenceWalkSlow(centerStatic, motor1Walk, motor2Walk, & sequenceRL[0], 10);
  _updateSequenceWalkSlow(centerStatic, motor1Walk, motor2Walk, & sequenceRR[0], 0);
}

void _setWalkBackSequence(unsigned char centerStatic) {
  _updateSequenceWalkBack(centerStatic, motor1Walk, motor2Walk, & sequenceFL[0], 0);
  _updateSequenceWalkBack(centerStatic, motor1Walk, motor2Walk, & sequenceFR[0], 10);
  _updateSequenceWalkBack(centerStatic, motor1Walk, motor2Walk, & sequenceRL[0], 10);
  _updateSequenceWalkBack(centerStatic, motor1Walk, motor2Walk, & sequenceRR[0], 0);
}

// get next sequence
void setPattern(unsigned char currentTaskItem, char angleTurn, unsigned char centerStatic) {
  // get new sequence array
  switch (currentTaskItem) {
    case P_DOSTAND:
    {
      _updateSequenceLinear(motor1Walk[centerStatic], motor2Walk[centerStatic], & sequenceFL[0]);
      _updateSequenceLinear(motor1Walk[centerStatic], motor2Walk[centerStatic], & sequenceFR[0]);
      _updateSequenceLinear(motor1Walk[centerStatic], motor2Walk[centerStatic], & sequenceRL[0]);
      _updateSequenceLinear(motor1Walk[centerStatic], motor2Walk[centerStatic], & sequenceRR[0]);
      _updateTurn(0);
    }
    break;
    case P_STANDTOGO:
    {
      _updateSequenceLinearEnd(motor1Walk[centerStatic] - _liftm, motor2Walk[centerStatic] - _liftm, & sequenceFL[0]);
      _updateSequenceLinearEnd(motor1Walk[centerStatic], motor2Walk[centerStatic], & sequenceFR[0]);
      _updateSequenceLinearEnd(motor1Walk[centerStatic], motor2Walk[centerStatic], & sequenceRL[0]);
      _updateSequenceLinearEnd(motor1Walk[centerStatic] - _liftm, motor2Walk[centerStatic] - _liftm, & sequenceRR[0]);
      _updateTurn(0);
    }
    break;
    case P_GOTOSTAND:
    {
      _updateSequenceLinearStart(motor1Walk[centerStatic], motor2Walk[centerStatic], & sequenceFL[0]);
      _updateSequenceLinearStart(motor1Walk[centerStatic], motor2Walk[centerStatic], & sequenceFR[0]);
      _updateSequenceLinearStart(motor1Walk[centerStatic], motor2Walk[centerStatic], & sequenceRL[0]);
      _updateSequenceLinearStart(motor1Walk[centerStatic], motor2Walk[centerStatic], & sequenceRR[0]);
      _updateTurn(0);
    }
    break;
    case P_STANDGO:
    {
      _setStandWalkSequence(centerStatic);
      _updateTurn(angleTurn);
    }
    break;
    case P_STANDGOLEFT:
    {
      _setStandWalkSequence(centerStatic);
      _updateTurn(-10);
    }
    break;
    case P_STANDGORIGHT:
    {
      _setStandWalkSequence(centerStatic);
      _updateTurn(10);
    }
    break;
    case P_STANDGOSHIFTLEFT:
    {
      _setStandWalkSequence(centerStatic);
      _updateShift(-10);
    }
    break;
    case P_STANDGOSHIFTRIGHT:
    {
      _setStandWalkSequence(centerStatic);
      _updateShift(10);
    }
    break;
    case P_GOFORWARD:
    {
      _setWalkSequence(centerStatic);
      _updateTurn(angleTurn);
    }
    break;
    case P_GOFORWARDSLOW:
    {
      _setWalkSlowSequence(centerStatic);
      _updateTurn(angleTurn);
    }
    break;
    case P_GOLEFT:
    {
      _setWalkSequence(centerStatic);
      _updateTurn(-10);
    }
    break;
    case P_GORIGHT:
    {
      _setWalkSequence(centerStatic);
      _updateTurn(10);
    }
    break;
    case P_GOSHIFTLEFT:
    {
      _setWalkSequence(centerStatic);
      _updateShift(-10);
    }
    break;
    case P_GOSHIFTRIGHT:
    {
      _setWalkSequence(centerStatic);
      _updateShift(10);
    }
    break;
    case P_GOBACK:
    {
      _setWalkBackSequence(centerStatic);
      _updateTurn(0);
    }
    break;
    case P_GOBACKLEFT:
    {
      _setWalkBackSequence(centerStatic);
      _updateTurn(-10);
    }
    break;
    case P_GOBACKRIGHT:
    {
      _setWalkBackSequence(centerStatic);
      _updateTurn(10);
    }
    break;
    case P_DODOWN:
    {
      _updateSequenceLinear(30, 30, & sequenceFL[0]);
      _updateSequenceLinear(30, 30, & sequenceFR[0]);
      _updateSequenceLinear(30, 30, & sequenceRL[0]);
      _updateSequenceLinear(30, 30, & sequenceRR[0]);
      _updateTurn(0);
    }
    break;
    case P_DODOWNLEFT:
    {
      _updateSequenceLinear(30, 30, & sequenceFL[0]);
      _updateSequenceLinear(45, 45, & sequenceFR[0]);
      _updateSequenceLinear(30, 30, & sequenceRL[0]);
      _updateSequenceLinear(45, 45, & sequenceRR[0]);
      _updateTurn(0);
    }
    break;
    case P_DODOWNRIGHT:
    {
      _updateSequenceLinear(45, 45, & sequenceFL[0]);
      _updateSequenceLinear(30, 30, & sequenceFR[0]);
      _updateSequenceLinear(45, 45, & sequenceRL[0]);
      _updateSequenceLinear(30, 30, & sequenceRR[0]);
      _updateTurn(0);
    }
    break;
    case P_DODOWNFRONT:
    {
      _updateSequenceLinear(30, 30, & sequenceFL[0]);
      _updateSequenceLinear(30, 30, & sequenceFR[0]);
      _updateSequenceLinear(45, 45, & sequenceRL[0]);
      _updateSequenceLinear(45, 45, & sequenceRR[0]);
      _updateTurn(0);
    }
    break;
    case P_DODOWNREAR:
    {
      _updateSequenceLinear(45, 45, & sequenceFL[0]);
      _updateSequenceLinear(45, 45, & sequenceFR[0]);
      _updateSequenceLinear(30, 30, & sequenceRL[0]);
      _updateSequenceLinear(30, 30, & sequenceRR[0]);
      _updateTurn(0);
    }
    break;
    case P_RECOVERLEFT:
    {
      _setSequenceRecover(& sequenceFL[0], & sequenceFR[0]);
      _setSequenceRecover(& sequenceRL[0], & sequenceRR[0]);
      _setSequenceRecoverCenter();
    }
    break;
    case P_RECOVERRIGHT:
    {
      _setSequenceRecover(& sequenceFR[0], & sequenceFL[0]);
      _setSequenceRecover(& sequenceRR[0], & sequenceRL[0]);
      _setSequenceRecoverCenter();
    }
    break;
    default:
    break;
  }
}

// update servo motors values
allMotors updateMotorsPatterns(void) {
  // set motors angle values
  motorValue.m.st.motor1 = sequenceCenter[mainCounter].motor1;
  motorValue.m.st.motor2 = sequenceCenter[mainCounter].motor2;
  motorValue.m.fl.motor1 = sequenceFL[mainCounter].motor1;
  motorValue.m.fl.motor2 = sequenceFL[mainCounter].motor2;
  motorValue.m.fr.motor1 = sequenceFR[mainCounter].motor1;
  motorValue.m.fr.motor2 = sequenceFR[mainCounter].motor2;
  motorValue.m.rl.motor1 = sequenceRL[mainCounter].motor1;
  motorValue.m.rl.motor2 = sequenceRL[mainCounter].motor2;
  motorValue.m.rr.motor1 = sequenceRR[mainCounter].motor1;
  motorValue.m.rr.motor2 = sequenceRR[mainCounter].motor2;
  return motorValue;
}

// update servo motors values
unsigned char updateCountPatterns(void) {
  // update sequence shift 
  mainCounter ++;
  if (mainCounter > 19) {
    mainCounter = 0;
  }
  return mainCounter;
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
