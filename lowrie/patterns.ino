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
// legs2.0
// forward shift(mm)               -40       -30       -20       -10         0        10        20        30        40
char motor1Walk[21] =  {  24,  22,  22,  23,  25,  27,  30,  33,  37,  41,  45,  51,  57,  63,  70,  77,  84,  91, 101, 111, 126};
char motor2Walk[21] =  { 126, 111, 101,  91,  84,  77,  70,  63,  57,  51,  45,  41,  37,  33,  30,  27,  25,  23,  22,  22,  24};


// center position in the pattern array
int _centerAbsolute = 10; // (range 8 to 12) bigger the number more weight on front
// static forward ballance
int _centerStatic = _centerAbsolute;
// char buffer for temporary use
char cBuffer1;
char cBuffer2;
// leg lift angles
unsigned char _liftm1 = 16;
unsigned char _liftm2 = 16;
// center motor default position
char _centerF = 0; // positive more distance between legs
char _centerR = 0; // positive more distance between legs
// points to currentSequence for every leg
unsigned char mainCounter = 0;
// dynamic side ballance
char _sideBallance = 0;
char _sideUpLeft = 0;
char _sideUpRight = 0;
// 20 positions per sequence
motors sequenceCenter[20] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
motors sequenceFL[20] = {30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30};
motors sequenceFR[20] = {30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30};
motors sequenceRL[20] = {30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30};
motors sequenceRR[20] = {30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30};

// motors values for 10 motors
allMotors motorValue = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

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

// set default values of center motors
void _defaultCenterMotor(void) {
  for (i = 0; i < 20; i++) {
    sequenceCenter[i].motor1 = _centerF;
    sequenceCenter[i].motor2 = _centerR;
  }
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
void _updateSequenceLinear(char endPosition1, char endPosition2, motors * sequence1, motors * sequence2) {
  // motor 1
  cBuffer1 = sequence2[9].motor1;
  cBuffer2 = sequence2[9].motor2;
  sequence1[0].motor1 = cBuffer1;
  sequence1[0].motor2 = cBuffer2;
  sequence2[0].motor1 = cBuffer1 + ((endPosition1 - cBuffer1) * 10) / 18;
  sequence2[0].motor2 = cBuffer2 + ((endPosition2 - cBuffer2) * 10) / 18;
  for (i = 1; i < 10; i++) {
    sequence1[i].motor1 = cBuffer1 + ((endPosition1 - cBuffer1) * i) / 18;
    sequence1[i].motor2 = cBuffer2 + ((endPosition2 - cBuffer2) * i) / 18;
    sequence2[i].motor1 = cBuffer1 + ((endPosition1 - cBuffer1) * (i + 10)) / 18;
    sequence2[i].motor2 = cBuffer2 + ((endPosition2 - cBuffer2) * (i + 10)) / 18;
  }
  sequence2[8].motor1 = endPosition1;
  sequence2[8].motor2 = endPosition2;
  sequence2[9].motor1 = endPosition1;
  sequence2[9].motor2 = endPosition2;
}

// linear motor movement from point to point at the end of sequence
void _updateSequenceLinearEnd(char endPosition1, char endPosition2, motors * sequence1, motors * sequence2) {
  // motor 1
  cBuffer1 = sequence2[9].motor1;
  cBuffer2 = sequence2[9].motor2;
  for (i = 0; i < 10; i++) {
    sequence1[i].motor1 = cBuffer1;
    sequence1[i].motor2 = cBuffer2;
    sequence2[i].motor1 = cBuffer1;
    sequence2[i].motor2 = cBuffer2;
  }
  sequence2[6].motor1 = cBuffer1 + (endPosition1 - cBuffer1) / 4;
  sequence2[7].motor1 = cBuffer1 + ((endPosition1 - cBuffer1) * 2) / 4;
  sequence2[8].motor1 = cBuffer1 + ((endPosition1 - cBuffer1) * 3) / 4;
  sequence2[9].motor1 = endPosition1;
  sequence2[6].motor2 = cBuffer2 + (endPosition2 - cBuffer2) / 4;
  sequence2[7].motor2 = cBuffer2 + ((endPosition2 - cBuffer2) * 2) / 4;
  sequence2[8].motor2 = cBuffer2 + ((endPosition2 - cBuffer2) * 3) / 4;
  sequence2[9].motor2 = endPosition2;
}

// linear motor movement from point to point at the start of sequence
void _updateSequenceLinearStart(char endPosition1, char endPosition2, motors * sequence1, motors * sequence2) {
  // motor 1
  cBuffer1 = sequence2[9].motor1;
  cBuffer2 = sequence2[9].motor2;
  for (i = 0; i < 10; i++) {
    sequence1[i].motor1 = endPosition1;
    sequence1[i].motor2 = endPosition2;
    sequence2[i].motor1 = endPosition1;
    sequence2[i].motor2 = endPosition2;
  }
  sequence1[0].motor1 = cBuffer1;
  sequence1[1].motor1 = cBuffer1 + (endPosition1 - cBuffer1) / 4;
  sequence1[2].motor1 = cBuffer1 + ((endPosition1 - cBuffer1) * 2) / 4;
  sequence1[3].motor1 = cBuffer1 + ((endPosition1 - cBuffer1) * 3) / 4;
  sequence1[0].motor2 = cBuffer2;
  sequence1[1].motor2 = cBuffer2 + (endPosition2 - cBuffer2) / 4;
  sequence1[2].motor2 = cBuffer2 + ((endPosition2 - cBuffer2) * 2) / 4;
  sequence1[3].motor2 = cBuffer2 + ((endPosition2 - cBuffer2) * 3) / 4;
}

// create stand walk sequence
void _updateSequenceStandWalk(char m1, char m2, motors * sequence1, motors * sequence2) {
  // motor 1
  for (i = 0; i < 10; i++) {
    sequence1[i].motor1 = m1;
    sequence1[i].motor2 = m2;
    sequence2[i].motor1 = m1;
    sequence2[i].motor2 = m2;
  }
  sequence1[0].motor1 = m1 - _liftm1;
  sequence1[1].motor1 = m1 - _liftm1 / 2;
  sequence1[2].motor1 = m1 - _liftm1 / 4;
  sequence2[8].motor1 = m1 - _liftm1 / 2;
  sequence2[9].motor1 = m1 - _liftm1;
  sequence1[0].motor2 = m2 - _liftm2;
  sequence1[1].motor2 = m2 - _liftm2 / 2;
  sequence1[2].motor2 = m2 - _liftm2 / 4;
  sequence2[8].motor2 = m2 - _liftm2 / 2;
  sequence2[9].motor2 = m2 - _liftm2;
}

// create walk sequence
void _updateSequenceWalk(char m1[], char m2[], motors * sequence1, motors * sequence2) {
  // motor 1
  sequence1[0].motor1 = m1[_centerStatic] - _liftm1;
  sequence1[1].motor1 = m1[_centerStatic - 5] - _liftm1;
  sequence1[2].motor1 = m1[_centerStatic - 8] - _liftm1;
  sequence1[3].motor1 = m1[_centerStatic - 7] - _liftm1;
  sequence1[4].motor1 = m1[_centerStatic - 6];
  sequence1[5].motor1 = m1[_centerStatic - 5];
  sequence1[6].motor1 = m1[_centerStatic - 4];
  sequence1[7].motor1 = m1[_centerStatic - 3];
  sequence1[8].motor1 = m1[_centerStatic - 2];
  sequence1[9].motor1 = m1[_centerStatic - 1];
  //
  sequence2[0].motor1 = m1[_centerStatic];
  sequence2[1].motor1 = m1[_centerStatic + 1];
  sequence2[2].motor1 = m1[_centerStatic + 2];
  sequence2[3].motor1 = m1[_centerStatic + 3];
  sequence2[4].motor1 = m1[_centerStatic + 4];
  sequence2[5].motor1 = m1[_centerStatic + 5];
  sequence2[6].motor1 = m1[_centerStatic + 6];
  sequence2[7].motor1 = m1[_centerStatic + 7];
  sequence2[8].motor1 = m1[_centerStatic + 8];
  sequence2[9].motor1 = m1[_centerStatic + 5] - _liftm1;
  // motor 2
  sequence1[0].motor2 = m2[_centerStatic] - _liftm2;
  sequence1[1].motor2 = m2[_centerStatic - 5] - _liftm2;
  sequence1[2].motor2 = m2[_centerStatic - 8] - _liftm2;
  sequence1[3].motor2 = m2[_centerStatic - 7];
  sequence1[4].motor2 = m2[_centerStatic - 6];
  sequence1[5].motor2 = m2[_centerStatic - 5];
  sequence1[6].motor2 = m2[_centerStatic - 4];
  sequence1[7].motor2 = m2[_centerStatic - 3];
  sequence1[8].motor2 = m2[_centerStatic - 2];
  sequence1[9].motor2 = m2[_centerStatic - 1];
  //
  sequence2[0].motor2 = m2[_centerStatic];
  sequence2[1].motor2 = m2[_centerStatic + 1];
  sequence2[2].motor2 = m2[_centerStatic + 2];
  sequence2[3].motor2 = m2[_centerStatic + 3];
  sequence2[4].motor2 = m2[_centerStatic + 4];
  sequence2[5].motor2 = m2[_centerStatic + 5];
  sequence2[6].motor2 = m2[_centerStatic + 6];
  sequence2[7].motor2 = m2[_centerStatic + 7];
  sequence2[8].motor2 = m2[_centerStatic + 8] - _liftm2;
  sequence2[9].motor2 = m2[_centerStatic + 5] - _liftm2;
}

// create walk back sequence
void _updateSequenceWalkBack(char m1[], char m2[], motors * sequence1, motors * sequence2) {
  // motor 1
  sequence2[9].motor1 = m1[_centerStatic - 5] - _liftm1;
  sequence2[8].motor1 = m1[_centerStatic - 8];
  sequence2[7].motor1 = m1[_centerStatic - 7];
  sequence2[6].motor1 = m1[_centerStatic - 6];
  sequence2[5].motor1 = m1[_centerStatic - 5];
  sequence2[4].motor1 = m1[_centerStatic - 4];
  sequence2[3].motor1 = m1[_centerStatic - 3];
  sequence2[2].motor1 = m1[_centerStatic - 2];
  sequence2[1].motor1 = m1[_centerStatic - 1];
  sequence2[0].motor1 = m1[_centerStatic];
  //
  sequence1[9].motor1 = m1[_centerStatic + 1];
  sequence1[8].motor1 = m1[_centerStatic + 2];
  sequence1[7].motor1 = m1[_centerStatic + 3];
  sequence1[6].motor1 = m1[_centerStatic + 4];
  sequence1[5].motor1 = m1[_centerStatic + 5];
  sequence1[4].motor1 = m1[_centerStatic + 6];
  sequence1[3].motor1 = m1[_centerStatic + 7] - _liftm1;
  sequence1[2].motor1 = m1[_centerStatic + 8] - _liftm1;
  sequence1[1].motor1 = m1[_centerStatic + 5] - _liftm1;
  sequence1[0].motor1 = m1[_centerStatic] - _liftm1;
  // motor 2
  sequence2[9].motor2 = m2[_centerStatic - 5] - _liftm2;
  sequence2[8].motor2 = m2[_centerStatic - 8] - _liftm2;
  sequence2[7].motor2 = m2[_centerStatic - 7];
  sequence2[6].motor2 = m2[_centerStatic - 6];
  sequence2[5].motor2 = m2[_centerStatic - 5];
  sequence2[4].motor2 = m2[_centerStatic - 4];
  sequence2[3].motor2 = m2[_centerStatic - 3];
  sequence2[2].motor2 = m2[_centerStatic - 2];
  sequence2[1].motor2 = m2[_centerStatic - 1];
  sequence2[0].motor2 = m2[_centerStatic];
  //
  sequence1[9].motor2 = m2[_centerStatic + 1];
  sequence1[8].motor2 = m2[_centerStatic + 2];
  sequence1[7].motor2 = m2[_centerStatic + 3];
  sequence1[6].motor2 = m2[_centerStatic + 4];
  sequence1[5].motor2 = m2[_centerStatic + 5];
  sequence1[4].motor2 = m2[_centerStatic + 6];
  sequence1[3].motor2 = m2[_centerStatic + 7];
  sequence1[2].motor2 = m2[_centerStatic + 8] - _liftm2;
  sequence1[1].motor2 = m2[_centerStatic + 5] - _liftm2;
  sequence1[0].motor2 = m2[_centerStatic] - _liftm2;
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
    sequence1[i].motor1 = -30; sequence1[i].motor2 = -30;
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

// get next sequence
void setPattern(unsigned char currentTaskItem) {
  // get new sequence array
  switch (currentTaskItem) {
    case P_DOSTAND:
    {
      _updateSequenceLinear(motor1Walk[_centerStatic], motor2Walk[_centerStatic], & sequenceFL[0], & sequenceFL[10]);
      _updateSequenceLinear(motor1Walk[_centerStatic], motor2Walk[_centerStatic], & sequenceFR[0], & sequenceFR[10]);
      _updateSequenceLinear(motor1Walk[_centerStatic], motor2Walk[_centerStatic], & sequenceRL[0], & sequenceRL[10]);
      _updateSequenceLinear(motor1Walk[_centerStatic], motor2Walk[_centerStatic], & sequenceRR[0], & sequenceRR[10]);
      _updateTurn(0);
    }
    break;
    case P_STANDTOGO:
    {
      _updateSequenceLinearEnd(motor1Walk[_centerStatic] - _liftm1, motor2Walk[_centerStatic] - _liftm2, & sequenceFL[0], & sequenceFL[10]);
      _updateSequenceLinearEnd(motor1Walk[_centerStatic], motor2Walk[_centerStatic], & sequenceFR[0], & sequenceFR[10]);
      _updateSequenceLinearEnd(motor1Walk[_centerStatic], motor2Walk[_centerStatic], & sequenceRL[0], & sequenceRL[10]);
      _updateSequenceLinearEnd(motor1Walk[_centerStatic] - _liftm1, motor2Walk[_centerStatic] - _liftm2, & sequenceRR[0], & sequenceRR[10]);
      _updateTurn(0);
    }
    break;
    case P_GOTOSTAND:
    {
      _updateSequenceLinearStart(motor1Walk[_centerStatic], motor2Walk[_centerStatic], & sequenceFL[0], & sequenceFL[10]);
      _updateSequenceLinearStart(motor1Walk[_centerStatic], motor2Walk[_centerStatic], & sequenceFR[0], & sequenceFR[10]);
      _updateSequenceLinearStart(motor1Walk[_centerStatic], motor2Walk[_centerStatic], & sequenceRL[0], & sequenceRL[10]);
      _updateSequenceLinearStart(motor1Walk[_centerStatic], motor2Walk[_centerStatic], & sequenceRR[0], & sequenceRR[10]);
      _updateTurn(0);
    }
    break;
    case P_STANDGO:
    {
      _updateSequenceStandWalk(motor1Walk[_centerStatic], motor2Walk[_centerStatic], & sequenceFL[0], & sequenceFL[10]);
      _updateSequenceStandWalk(motor1Walk[_centerStatic], motor2Walk[_centerStatic], & sequenceFR[10], & sequenceFR[0]);
      _updateSequenceStandWalk(motor1Walk[_centerStatic], motor2Walk[_centerStatic], & sequenceRL[10], & sequenceRL[0]);
      _updateSequenceStandWalk(motor1Walk[_centerStatic], motor2Walk[_centerStatic], & sequenceRR[0], & sequenceRR[10]);
    }
    break;
    case P_STANDGOLEFT:
    {
      _updateSequenceStandWalk(motor1Walk[_centerStatic], motor2Walk[_centerStatic], & sequenceFL[0], & sequenceFL[10]);
      _updateSequenceStandWalk(motor1Walk[_centerStatic], motor2Walk[_centerStatic], & sequenceFR[10], & sequenceFR[0]);
      _updateSequenceStandWalk(motor1Walk[_centerStatic], motor2Walk[_centerStatic], & sequenceRL[10], & sequenceRL[0]);
      _updateSequenceStandWalk(motor1Walk[_centerStatic], motor2Walk[_centerStatic], & sequenceRR[0], & sequenceRR[10]);
      _updateTurn(-10);
    }
    break;
    case P_STANDGORIGHT:
    {
      _updateSequenceStandWalk(motor1Walk[_centerStatic], motor2Walk[_centerStatic], & sequenceFL[0], & sequenceFL[10]);
      _updateSequenceStandWalk(motor1Walk[_centerStatic], motor2Walk[_centerStatic], & sequenceFR[10], & sequenceFR[0]);
      _updateSequenceStandWalk(motor1Walk[_centerStatic], motor2Walk[_centerStatic], & sequenceRL[10], & sequenceRL[0]);
      _updateSequenceStandWalk(motor1Walk[_centerStatic], motor2Walk[_centerStatic], & sequenceRR[0], & sequenceRR[10]);
      _updateTurn(10);
    }
    break;
    case P_STANDGOSHIFTLEFT:
    {
      _updateSequenceStandWalk(motor1Walk[_centerStatic], motor2Walk[_centerStatic], & sequenceFL[0], & sequenceFL[10]);
      _updateSequenceStandWalk(motor1Walk[_centerStatic], motor2Walk[_centerStatic], & sequenceFR[10], & sequenceFR[0]);
      _updateSequenceStandWalk(motor1Walk[_centerStatic], motor2Walk[_centerStatic], & sequenceRL[10], & sequenceRL[0]);
      _updateSequenceStandWalk(motor1Walk[_centerStatic], motor2Walk[_centerStatic], & sequenceRR[0], & sequenceRR[10]);
      _updateShift(-10);
    }
    break;
    case P_STANDGOSHIFTRIGHT:
    {
      _updateSequenceStandWalk(motor1Walk[_centerStatic], motor2Walk[_centerStatic], & sequenceFL[0], & sequenceFL[10]);
      _updateSequenceStandWalk(motor1Walk[_centerStatic], motor2Walk[_centerStatic], & sequenceFR[10], & sequenceFR[0]);
      _updateSequenceStandWalk(motor1Walk[_centerStatic], motor2Walk[_centerStatic], & sequenceRL[10], & sequenceRL[0]);
      _updateSequenceStandWalk(motor1Walk[_centerStatic], motor2Walk[_centerStatic], & sequenceRR[0], & sequenceRR[10]);
      _updateShift(10);
    }
    break;
    case P_GOFORWARD:
    {
      _updateSequenceWalk(motor1Walk, motor2Walk, & sequenceFL[0], & sequenceFL[10]);
      _updateSequenceWalk(motor1Walk, motor2Walk, & sequenceFR[10], & sequenceFR[0]);
      _updateSequenceWalk(motor1Walk, motor2Walk, & sequenceRL[10], & sequenceRL[0]);
      _updateSequenceWalk(motor1Walk, motor2Walk, & sequenceRR[0], & sequenceRR[10]);
    }
    break;
    case P_GOLEFT:
    {
      _updateSequenceWalk(motor1Walk, motor2Walk, & sequenceFL[0], & sequenceFL[10]);
      _updateSequenceWalk(motor1Walk, motor2Walk, & sequenceFR[10], & sequenceFR[0]);
      _updateSequenceWalk(motor1Walk, motor2Walk, & sequenceRL[10], & sequenceRL[0]);
      _updateSequenceWalk(motor1Walk, motor2Walk, & sequenceRR[0], & sequenceRR[10]);
      _updateTurn(-10);
    }
    break;
    case P_GORIGHT:
    {
      _updateSequenceWalk(motor1Walk, motor2Walk, & sequenceFL[0], & sequenceFL[10]);
      _updateSequenceWalk(motor1Walk, motor2Walk, & sequenceFR[10], & sequenceFR[0]);
      _updateSequenceWalk(motor1Walk, motor2Walk, & sequenceRL[10], & sequenceRL[0]);
      _updateSequenceWalk(motor1Walk, motor2Walk, & sequenceRR[0], & sequenceRR[10]);
      _updateTurn(10);
    }
    break;
    case P_GOSHIFTLEFT:
    {
      _updateSequenceWalk(motor1Walk, motor2Walk, & sequenceFL[0], & sequenceFL[10]);
      _updateSequenceWalk(motor1Walk, motor2Walk, & sequenceFR[10], & sequenceFR[0]);
      _updateSequenceWalk(motor1Walk, motor2Walk, & sequenceRL[10], & sequenceRL[0]);
      _updateSequenceWalk(motor1Walk, motor2Walk, & sequenceRR[0], & sequenceRR[10]);
      _updateShift(-10);
    }
    break;
    case P_GOSHIFTRIGHT:
    {
      _updateSequenceWalk(motor1Walk, motor2Walk, & sequenceFL[0], & sequenceFL[10]);
      _updateSequenceWalk(motor1Walk, motor2Walk, & sequenceFR[10], & sequenceFR[0]);
      _updateSequenceWalk(motor1Walk, motor2Walk, & sequenceRL[10], & sequenceRL[0]);
      _updateSequenceWalk(motor1Walk, motor2Walk, & sequenceRR[0], & sequenceRR[10]);
      _updateShift(10);
    }
    break;
    case P_GOBACK:
    {
      _updateSequenceWalkBack(motor1Walk, motor2Walk, & sequenceFL[0], & sequenceFL[10]);
      _updateSequenceWalkBack(motor1Walk, motor2Walk, & sequenceFR[10], & sequenceFR[0]);
      _updateSequenceWalkBack(motor1Walk, motor2Walk, & sequenceRL[10], & sequenceRL[0]);
      _updateSequenceWalkBack(motor1Walk, motor2Walk, & sequenceRR[0], & sequenceRR[10]);
      _updateTurn(0);
    }
    break;
    case P_GOBACKLEFT:
    {
      _updateSequenceWalkBack(motor1Walk, motor2Walk, & sequenceFL[0], & sequenceFL[10]);
      _updateSequenceWalkBack(motor1Walk, motor2Walk, & sequenceFR[10], & sequenceFR[0]);
      _updateSequenceWalkBack(motor1Walk, motor2Walk, & sequenceRL[10], & sequenceRL[0]);
      _updateSequenceWalkBack(motor1Walk, motor2Walk, & sequenceRR[0], & sequenceRR[10]);
      _updateTurn(-10);
    }
    break;
    case P_GOBACKRIGHT:
    {
      _updateSequenceWalkBack(motor1Walk, motor2Walk, & sequenceFL[0], & sequenceFL[10]);
      _updateSequenceWalkBack(motor1Walk, motor2Walk, & sequenceFR[10], & sequenceFR[0]);
      _updateSequenceWalkBack(motor1Walk, motor2Walk, & sequenceRL[10], & sequenceRL[0]);
      _updateSequenceWalkBack(motor1Walk, motor2Walk, & sequenceRR[0], & sequenceRR[10]);
      _updateTurn(10);
    }
    break;
    case P_DODOWN:
    {
      _updateSequenceLinear(30, 30, & sequenceFL[0], & sequenceFL[10]);
      _updateSequenceLinear(30, 30, & sequenceFR[0], & sequenceFR[10]);
      _updateSequenceLinear(30, 30, & sequenceRL[0], & sequenceRL[10]);
      _updateSequenceLinear(30, 30, & sequenceRR[0], & sequenceRR[10]);
      _updateTurn(0);
    }
    break;
    case P_DODOWNLEFT:
    {
      _updateSequenceLinear(30, 30, & sequenceFL[0], & sequenceFL[10]);
      _updateSequenceLinear(45, 45, & sequenceFR[0], & sequenceFR[10]);
      _updateSequenceLinear(30, 30, & sequenceRL[0], & sequenceRL[10]);
      _updateSequenceLinear(45, 45, & sequenceRR[0], & sequenceRR[10]);
      _updateTurn(0);
    }
    break;
    case P_DODOWNRIGHT:
    {
      _updateSequenceLinear(45, 45, & sequenceFL[0], & sequenceFL[10]);
      _updateSequenceLinear(30, 30, & sequenceFR[0], & sequenceFR[10]);
      _updateSequenceLinear(45, 45, & sequenceRL[0], & sequenceRL[10]);
      _updateSequenceLinear(30, 30, & sequenceRR[0], & sequenceRR[10]);
      _updateTurn(0);
    }
    break;
    case P_DODOWNFRONT:
    {
      _updateSequenceLinear(30, 30, & sequenceFL[0], & sequenceFL[10]);
      _updateSequenceLinear(30, 30, & sequenceFR[0], & sequenceFR[10]);
      _updateSequenceLinear(45, 45, & sequenceRL[0], & sequenceRL[10]);
      _updateSequenceLinear(45, 45, & sequenceRR[0], & sequenceRR[10]);
      _updateTurn(0);
    }
    break;
    case P_DODOWNREAR:
    {
      _updateSequenceLinear(45, 45, & sequenceFL[0], & sequenceFL[10]);
      _updateSequenceLinear(45, 45, & sequenceFR[0], & sequenceFR[10]);
      _updateSequenceLinear(30, 30, & sequenceRL[0], & sequenceRL[10]);
      _updateSequenceLinear(30, 30, & sequenceRR[0], & sequenceRR[10]);
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

// update turning
void updateTurnPattern(char cAngle) {
  if (cAngle == 0) {
    _defaultCenterMotor();
  } else {
    _updateTurn(cAngle);
  }
}

// update servo motors values
allMotors updateMotorsPatterns(allMotors calibration) {
  // set motors angle values
  motorValue.front       = (sequenceCenter[mainCounter].motor1 + calibration.front);
  motorValue.rear        = (sequenceCenter[mainCounter].motor2 + calibration.rear);
  motorValue.m.fl.motor1 = (sequenceFL[mainCounter].motor1 - 30 + calibration.m.fl.motor1 - _sideUpLeft / 3);
  motorValue.m.fl.motor2 = (sequenceFL[mainCounter].motor2 - 30 + calibration.m.fl.motor2 - _sideUpLeft);
  motorValue.m.fr.motor1 = (sequenceFR[mainCounter].motor1 - 30 + calibration.m.fr.motor1 + _sideUpRight / 3);
  motorValue.m.fr.motor2 = (sequenceFR[mainCounter].motor2 - 30 + calibration.m.fr.motor2 + _sideUpRight);
  motorValue.m.rl.motor1 = (sequenceRL[mainCounter].motor1 - 30 + calibration.m.rl.motor1 - _sideUpLeft / 3);
  motorValue.m.rl.motor2 = (sequenceRL[mainCounter].motor2 - 30 + calibration.m.rl.motor2 - _sideUpLeft);
  motorValue.m.rr.motor1 = (sequenceRR[mainCounter].motor1 - 30 + calibration.m.rr.motor1 + _sideUpRight / 3);
  motorValue.m.rr.motor2 = (sequenceRR[mainCounter].motor2 - 30 + calibration.m.rr.motor2 + _sideUpRight);
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

// update ballance 
void updateBallancePattern(void) {
  _centerStatic = _compensateStaticBallance(gyroState);
  _sideBallance = _fixSideBalance(_sideBallance, gyroState);
  _centerF = _fixDynamicBalance(_centerF, gyroState);
  _centerR = - _centerF;
  if (_sideBallance > 0) {
    _sideUpLeft = _sideBallance;
    _sideUpRight = 0;
  } else {
    _sideUpLeft = 0;
    _sideUpRight = _sideBallance;
  }
}

// fix balance using gyro
char _fixDynamicBalance(char center, accRoll gyroState) {
  // nose down increase waight on rear
  // nose up increase waight on front
  if (gyroState.rollMax - gyroState.rollMin > 2) {
    // body rolls
    if ((gyroState.rollMinTime < 10) && (gyroState.rollMaxTime > 9)) {
      // front is too heavy
      // increase weight on rear
      center -= 1;
    }
    if ((gyroState.rollMinTime > 9) && (gyroState.rollMaxTime < 10)) {
      // rear is too heavy
      //increase wight on front
      center += 1;
    }
  }
  if (center < -10) {
    center = -10;
  } else if (center > 10) {
    center = 10;
  }
  return center;
}

// compensate nose dive for static ballance
int _compensateStaticBallance(accRoll gyroState) {
  int center = _centerAbsolute - gyroState.accAngleY / 2;
  if (center < 8) {
    center = 8;
  } else if (center > 12) {
    center = 12;
  }
  return center;
}

// fix side balance using gyro
char _fixSideBalance(char sideBallance, accRoll gyroState) {
  if (gyroState.accAngleX > 1) {
    if (sideBallance < 2) {
      sideBallance ++;
    }
  } else if (gyroState.accAngleX < -1) {
    if (sideBallance > -2) {
      sideBallance --;
    }
  }
  return sideBallance;
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
