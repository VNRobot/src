/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Robot legs motion patterns
*/

// servos angles for forward shift
// legs 1.0 
// forward shift(mm)          -40,           -30,           -20,           -10,          0,         10,         20,         30
//char motor1Walk[24] =  { 46,  47,  48,  50,  52,  54,  56,  58,  60,  62,  64, 66, 69, 72, 75, 78, 81, 84, 87, 90, 94, 98,102,106};
//char motor2Walk[24] =  { 96,  90,  85,  81,  77,  73,  69,  65,  61,  57,  54, 51, 48, 45, 42, 40, 38, 36, 34, 33, 32, 31, 30, 29};
// legs1.1 faster walking
// forward shift(mm)               -40       -30       -20       -10         0        10        20        30        40
//char motor1Walk[21] =  {  24,  22,  22,  23,  25,  27,  30,  33,  37,  41,  45,  51,  57,  63,  70,  77,  84,  91, 101, 111, 126};
//char motor2Walk[21] =  { 126, 111, 101,  91,  84,  77,  70,  63,  57,  51,  45,  41,  37,  33,  30,  27,  25,  23,  22,  22,  24};
// legs1.1 slower walking
char motor1Walk[25] =  { 21,  21,  21,  21,  22,  24,  26,  28,  30,  34,  38,  42,  45,  50,  55,  60,  65,  70,  75,  81,  87,  93, 100, 109, 119};
char motor2Walk[25] =  {119, 109, 100,  93,  87,  81,  75,  70,  65,  60,  55,  50,  45,  42,  38,  34,  30,  28,  26,  24,  22,  21,  21,  21,  21};
// center position in the pattern array
int _centerAbsolute = 12; // (range 8 to 16) bigger the number more weight on front
// static forward ballance
int _centerStatic = _centerAbsolute;
// char buffer for temporary use
char cBuffer1;
char cBuffer2;
// leg lift angles
unsigned char _liftm = 16;
// center motor default position
char _centerF = 0; // positive more distance between legs
char _centerR = 0; // positive more distance between legs
// points to currentSequence for every leg
unsigned char mainCounter = 0;
// dynamic ballance
allMotors legUp = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
// 20 positions per sequence
motors sequenceCenter[20] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
motors sequenceFL[20] = {30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30};
motors sequenceFR[20] = {30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30};
motors sequenceRL[20] = {30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30};
motors sequenceRR[20] = {30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30};
// motors values for 10 motors
allMotors motorValue = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
allMotors motorValueBuffer = {60, 60, 60, 60, 60, 60, 60, 60, 60, 60};
allMotors motorValueBufferOld = {60, 60, 60, 60, 60, 60, 60, 60, 60, 60};
// full cycle
unsigned char fullCycle = 20;
// pointer to array
unsigned char apoint = 0;
// position correction
int correctRoll  = 0;
int correctPitch = 0;

// set new values of center motors
void _updateCenterMotor(char cAngleF, char cAngleR) {
  sequenceCenter[0].motor1 = _centerF - cAngleF/2;
  sequenceCenter[0].motor2 = _centerR - cAngleR/2;
  for (i = 1; i < 9; i++) {
    sequenceCenter[i].motor1 = _centerF - cAngleF;
    sequenceCenter[i].motor2 = _centerR - cAngleR;
  }
  sequenceCenter[9].motor1 = _centerF - cAngleF/2;
  sequenceCenter[9].motor2 = _centerR - cAngleR/2;
  sequenceCenter[10].motor1 = _centerF + cAngleF/2;
  sequenceCenter[10].motor2 = _centerR + cAngleR/2;
  for (i = 11; i < 19; i++) {
    sequenceCenter[i].motor1 = _centerF + cAngleF;
    sequenceCenter[i].motor2 = _centerR + cAngleR;
  }
  sequenceCenter[19].motor1 = _centerF + cAngleF/2;
  sequenceCenter[19].motor2 = _centerR + cAngleR/2;
}

void updateTurnPattern(char cAngle) {
  _updateTurn(cAngle);
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

void _setStandWalkSequence(void) {
  _updateSequenceStandWalk(motor1Walk[_centerStatic], motor2Walk[_centerStatic], & sequenceFL[0], 0);
  _updateSequenceStandWalk(motor1Walk[_centerStatic], motor2Walk[_centerStatic], & sequenceFR[0], 10);
  _updateSequenceStandWalk(motor1Walk[_centerStatic], motor2Walk[_centerStatic], & sequenceRL[0], 10);
  _updateSequenceStandWalk(motor1Walk[_centerStatic], motor2Walk[_centerStatic], & sequenceRR[0], 0);
}

void _setWalkSequence(void) {
  _updateSequenceWalk(_centerStatic, motor1Walk, motor2Walk, & sequenceFL[0], 0);
  _updateSequenceWalk(_centerStatic, motor1Walk, motor2Walk, & sequenceFR[0], 10);
  _updateSequenceWalk(_centerStatic, motor1Walk, motor2Walk, & sequenceRL[0], 10);
  _updateSequenceWalk(_centerStatic, motor1Walk, motor2Walk, & sequenceRR[0], 0);
}

void _setWalkSlowSequence(void) {
  _updateSequenceWalkSlow(_centerStatic, motor1Walk, motor2Walk, & sequenceFL[0], 0);
  _updateSequenceWalkSlow(_centerStatic, motor1Walk, motor2Walk, & sequenceFR[0], 10);
  _updateSequenceWalkSlow(_centerStatic, motor1Walk, motor2Walk, & sequenceRL[0], 10);
  _updateSequenceWalkSlow(_centerStatic, motor1Walk, motor2Walk, & sequenceRR[0], 0);
}

void _setWalkBackSequence(void) {
  _updateSequenceWalkBack(_centerStatic, motor1Walk, motor2Walk, & sequenceFL[0], 0);
  _updateSequenceWalkBack(_centerStatic, motor1Walk, motor2Walk, & sequenceFR[0], 10);
  _updateSequenceWalkBack(_centerStatic, motor1Walk, motor2Walk, & sequenceRL[0], 10);
  _updateSequenceWalkBack(_centerStatic, motor1Walk, motor2Walk, & sequenceRR[0], 0);
}

// get next sequence
void setPattern(unsigned char currentTaskItem, char angleTurn) {
  // get new sequence array
  switch (currentTaskItem) {
    case P_DOSTAND:
    {
      _updateSequenceLinear(motor1Walk[_centerStatic], motor2Walk[_centerStatic], & sequenceFL[0]);
      _updateSequenceLinear(motor1Walk[_centerStatic], motor2Walk[_centerStatic], & sequenceFR[0]);
      _updateSequenceLinear(motor1Walk[_centerStatic], motor2Walk[_centerStatic], & sequenceRL[0]);
      _updateSequenceLinear(motor1Walk[_centerStatic], motor2Walk[_centerStatic], & sequenceRR[0]);
      _updateTurn(0);
    }
    break;
    case P_STANDTOGO:
    {
      _updateSequenceLinearEnd(motor1Walk[_centerStatic] - _liftm, motor2Walk[_centerStatic] - _liftm, & sequenceFL[0]);
      _updateSequenceLinearEnd(motor1Walk[_centerStatic], motor2Walk[_centerStatic], & sequenceFR[0]);
      _updateSequenceLinearEnd(motor1Walk[_centerStatic], motor2Walk[_centerStatic], & sequenceRL[0]);
      _updateSequenceLinearEnd(motor1Walk[_centerStatic] - _liftm, motor2Walk[_centerStatic] - _liftm, & sequenceRR[0]);
      _updateTurn(0);
    }
    break;
    case P_GOTOSTAND:
    {
      _updateSequenceLinearStart(motor1Walk[_centerStatic], motor2Walk[_centerStatic], & sequenceFL[0]);
      _updateSequenceLinearStart(motor1Walk[_centerStatic], motor2Walk[_centerStatic], & sequenceFR[0]);
      _updateSequenceLinearStart(motor1Walk[_centerStatic], motor2Walk[_centerStatic], & sequenceRL[0]);
      _updateSequenceLinearStart(motor1Walk[_centerStatic], motor2Walk[_centerStatic], & sequenceRR[0]);
      _updateTurn(0);
    }
    break;
    case P_STANDGO:
    {
      _setStandWalkSequence();
      _updateTurn(angleTurn);
    }
    break;
    case P_STANDGOLEFT:
    {
      _setStandWalkSequence();
      _updateTurn(-10);
    }
    break;
    case P_STANDGORIGHT:
    {
      _setStandWalkSequence();
      _updateTurn(10);
    }
    break;
    case P_STANDGOSHIFTLEFT:
    {
      _setStandWalkSequence();
      _updateShift(-10);
    }
    break;
    case P_STANDGOSHIFTRIGHT:
    {
      _setStandWalkSequence();
      _updateShift(10);
    }
    break;
    case P_GOFORWARD:
    {
      _setWalkSequence();
      _updateTurn(angleTurn);
    }
    break;
    case P_GOFORWARDSLOW:
    {
      _setWalkSlowSequence();
      _updateTurn(angleTurn);
    }
    break;
    case P_GOLEFT:
    {
      _setWalkSequence();
      _updateTurn(-10);
    }
    break;
    case P_GORIGHT:
    {
      _setWalkSequence();
      _updateTurn(10);
    }
    break;
    case P_GOSHIFTLEFT:
    {
      _setWalkSequence();
      _updateShift(-10);
    }
    break;
    case P_GOSHIFTRIGHT:
    {
      _setWalkSequence();
      _updateShift(10);
    }
    break;
    case P_GOBACK:
    {
      _setWalkBackSequence();
      _updateTurn(0);
    }
    break;
    case P_GOBACKLEFT:
    {
      _setWalkBackSequence();
      _updateTurn(-10);
    }
    break;
    case P_GOBACKRIGHT:
    {
      _setWalkBackSequence();
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

// limit angle value
char limitValue(char mAngle) {
  if (mAngle > 90) {
    mAngle = 90;
  } else if (mAngle < -90) {
    mAngle = -90;
  }
  return mAngle;
}

// update servo motors buffer values
void updateBufferPatterns(allMotors calibration) {
  motorValueBuffer.m.st.motor1 = limitValue(sequenceCenter[mainCounter].motor1 + calibration.m.st.motor1);
  motorValueBuffer.m.st.motor2 = limitValue(sequenceCenter[mainCounter].motor2 + calibration.m.st.motor2);
  motorValueBuffer.m.fl.motor1 = limitValue(sequenceFL[mainCounter].motor1 - 30 + calibration.m.fl.motor1 + legUp.m.fl.motor1);
  motorValueBuffer.m.fl.motor2 = limitValue(sequenceFL[mainCounter].motor2 - 30 + calibration.m.fl.motor2 + legUp.m.fl.motor2);
  motorValueBuffer.m.fr.motor1 = limitValue(sequenceFR[mainCounter].motor1 - 30 + calibration.m.fr.motor1 + legUp.m.fr.motor1);
  motorValueBuffer.m.fr.motor2 = limitValue(sequenceFR[mainCounter].motor2 - 30 + calibration.m.fr.motor2 + legUp.m.fr.motor2);
  motorValueBuffer.m.rl.motor1 = limitValue(sequenceRL[mainCounter].motor1 - 30 + calibration.m.rl.motor1 + legUp.m.rl.motor1);
  motorValueBuffer.m.rl.motor2 = limitValue(sequenceRL[mainCounter].motor2 - 30 + calibration.m.rl.motor2 + legUp.m.rl.motor2);
  motorValueBuffer.m.rr.motor1 = limitValue(sequenceRR[mainCounter].motor1 - 30 + calibration.m.rr.motor1 + legUp.m.rr.motor1);
  motorValueBuffer.m.rr.motor2 = limitValue(sequenceRR[mainCounter].motor2 - 30 + calibration.m.rr.motor2 + legUp.m.rr.motor2);
}

// update servo motors buffer old values
void updateBufferOldPatterns(void) {
  motorValueBufferOld.m.st.motor1 = motorValueBuffer.m.st.motor1;
  motorValueBufferOld.m.st.motor2 = motorValueBuffer.m.st.motor2;
  motorValueBufferOld.m.fl.motor1 = motorValueBuffer.m.fl.motor1;
  motorValueBufferOld.m.fl.motor2 = motorValueBuffer.m.fl.motor2;
  motorValueBufferOld.m.fr.motor1 = motorValueBuffer.m.fr.motor1;
  motorValueBufferOld.m.fr.motor2 = motorValueBuffer.m.fr.motor2;
  motorValueBufferOld.m.rl.motor1 = motorValueBuffer.m.rl.motor1;
  motorValueBufferOld.m.rl.motor2 = motorValueBuffer.m.rl.motor2;
  motorValueBufferOld.m.rr.motor1 = motorValueBuffer.m.rr.motor1;
  motorValueBufferOld.m.rr.motor2 = motorValueBuffer.m.rr.motor2;
}

allMotors updateMotorsHalfPatterns(void) {
  motorValue.m.st.motor1 = (motorValueBuffer.m.st.motor1 + motorValueBufferOld.m.st.motor1) / 2;
  motorValue.m.st.motor2 = (motorValueBuffer.m.st.motor2 + motorValueBufferOld.m.st.motor2) / 2;
  motorValue.m.fl.motor1 = (motorValueBuffer.m.fl.motor1 + motorValueBufferOld.m.fl.motor1) / 2;
  motorValue.m.fl.motor2 = (motorValueBuffer.m.fl.motor2 + motorValueBufferOld.m.fl.motor2) / 2;
  motorValue.m.fr.motor1 = (motorValueBuffer.m.fr.motor1 + motorValueBufferOld.m.fr.motor1) / 2;
  motorValue.m.fr.motor2 = (motorValueBuffer.m.fr.motor2 + motorValueBufferOld.m.fr.motor2) / 2;
  motorValue.m.rl.motor1 = (motorValueBuffer.m.rl.motor1 + motorValueBufferOld.m.rl.motor1) / 2;
  motorValue.m.rl.motor2 = (motorValueBuffer.m.rl.motor2 + motorValueBufferOld.m.rl.motor2) / 2;
  motorValue.m.rr.motor1 = (motorValueBuffer.m.rr.motor1 + motorValueBufferOld.m.rr.motor1) / 2;
  motorValue.m.rr.motor2 = (motorValueBuffer.m.rr.motor2 + motorValueBufferOld.m.rr.motor2) / 2;
  return motorValue;
}

// update servo motors values
allMotors updateMotorsPatterns(void) {
  // set motors angle values
  motorValue.m.st.motor1 = motorValueBuffer.m.st.motor1;
  motorValue.m.st.motor2 = motorValueBuffer.m.st.motor2;
  motorValue.m.fl.motor1 = motorValueBuffer.m.fl.motor1;
  motorValue.m.fl.motor2 = motorValueBuffer.m.fl.motor2;
  motorValue.m.fr.motor1 = motorValueBuffer.m.fr.motor1;
  motorValue.m.fr.motor2 = motorValueBuffer.m.fr.motor2;
  motorValue.m.rl.motor1 = motorValueBuffer.m.rl.motor1;
  motorValue.m.rl.motor2 = motorValueBuffer.m.rl.motor2;
  motorValue.m.rr.motor1 = motorValueBuffer.m.rr.motor1;
  motorValue.m.rr.motor2 = motorValueBuffer.m.rr.motor2;
  return motorValue;
}

void updateStaticBallanceInPattern(accRoll gyroState) {
  if (gyroState.accAngleX > 2) {
    if (correctRoll < 10) {
      correctRoll ++;
    }
  } else if (gyroState.accAngleX < -2) {
    if (correctRoll > -10) {
      correctRoll --;
    }
  }
  if (gyroState.accAngleY > 2) {
    if (correctPitch < 10) {
      correctPitch ++;
    }
  } else if (gyroState.accAngleY < -2) {
    if (correctPitch > -10) {
      correctPitch --;
    }
  }
  legUp.m.fl.motor1 = -correctRoll + correctPitch;
  legUp.m.fl.motor2 = -correctRoll + correctPitch;
  legUp.m.fr.motor1 = correctRoll + correctPitch;
  legUp.m.fr.motor2 = correctRoll + correctPitch;
  legUp.m.rl.motor1 = -correctRoll - correctPitch;
  legUp.m.rl.motor2 = -correctRoll - correctPitch;
  legUp.m.rr.motor1 = correctRoll - correctPitch;
  legUp.m.rr.motor2 = correctRoll - correctPitch;
  // nose down increase waight on rear
  // nose up increase waight on front
  _centerStatic = _centerAbsolute - gyroState.accAngleY / 2;
  if (_centerStatic > 16) {
    _centerStatic = 16;
  }
  if (_centerStatic < 8) {
    _centerStatic = 8;
  }
}

void updateDynamicBallanceInPattern(accRoll gyroState) {
  if (gyroState.rollMax - gyroState.rollMin > 2) {
    // body rolls
    if ((gyroState.rollMinTime < 10) && (gyroState.rollMaxTime > 9)) {
      // front is too heavy
      // increase weight on rear
      _centerF -= 1;
    }
    if ((gyroState.rollMinTime > 9) && (gyroState.rollMaxTime < 10)) {
      // rear is too heavy
      //increase wight on front
      _centerF += 1;
    }
  }
  if (_centerF < -10) {
    _centerF = -10;
  } else if (_centerF > 10) {
    _centerF = 10;
  }
  _centerR = - _centerF;
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
