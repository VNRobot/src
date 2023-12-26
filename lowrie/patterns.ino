/*
Wlking Robot Lowrie
Licended GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Robot legs motion patterns
*/

// servos angles for forward shift 
// forward shift(mm)          -40,           -30,           -20,           -10,          0,         10,         20,         30
char _motor1Walk[24] =  { 46,  47,  48,  50,  52,  54,  56,  58,  60,  62,  64, 66, 69, 72, 75, 78, 81, 84, 87, 90, 94, 98,102,106};
char _motor2Walk[24] =  { 96,  90,  85,  81,  77,  73,  69,  65,  61,  57,  54, 51, 48, 45, 42, 40, 38, 36, 34, 33, 32, 31, 30, 29};
// char buffer for temporary use
char _cBuffer;
// leg lift angles
unsigned char liftm1 = 12;
unsigned char liftm2 = 12;

// set new values of center motors
void _updateCenterMotor(char cAngleF, char cAngleR) {
  // front motor
  m_currentSequence[0].front = m_centerF - cAngleF/2;
  m_currentSequence[1].front = m_centerF - cAngleF;
  m_currentSequence[2].front = m_centerF - cAngleF;
  m_currentSequence[3].front = m_centerF - cAngleF;
  m_currentSequence[4].front = m_centerF - cAngleF;
  m_currentSequence[5].front = m_centerF - cAngleF;
  m_currentSequence[6].front = m_centerF - cAngleF;
  m_currentSequence[7].front = m_centerF - cAngleF;
  m_currentSequence[8].front = m_centerF - cAngleF;
  m_currentSequence[9].front = m_centerF - cAngleF/2;
  m_currentSequence[10].front = m_centerF + cAngleF/2;
  m_currentSequence[11].front = m_centerF + cAngleF;
  m_currentSequence[12].front = m_centerF + cAngleF;
  m_currentSequence[13].front = m_centerF + cAngleF;
  m_currentSequence[14].front = m_centerF + cAngleF;
  m_currentSequence[15].front = m_centerF + cAngleF;
  m_currentSequence[16].front = m_centerF + cAngleF;
  m_currentSequence[17].front = m_centerF + cAngleF;
  m_currentSequence[18].front = m_centerF + cAngleF;
  m_currentSequence[19].front = m_centerF + cAngleF/2;
  // rear motor
  m_currentSequence[0].rear = m_centerR - cAngleR/2;
  m_currentSequence[1].rear = m_centerR - cAngleR;
  m_currentSequence[2].rear = m_centerR - cAngleR;
  m_currentSequence[3].rear = m_centerR - cAngleR;
  m_currentSequence[4].rear = m_centerR - cAngleR;
  m_currentSequence[5].rear = m_centerR - cAngleR;
  m_currentSequence[6].rear = m_centerR - cAngleR;
  m_currentSequence[7].rear = m_centerR - cAngleR;
  m_currentSequence[8].rear = m_centerR - cAngleR;
  m_currentSequence[9].rear = m_centerR - cAngleR/2;
  m_currentSequence[10].rear = m_centerR + cAngleR/2;
  m_currentSequence[11].rear = m_centerR + cAngleR;
  m_currentSequence[12].rear = m_centerR + cAngleR;
  m_currentSequence[13].rear = m_centerR + cAngleR;
  m_currentSequence[14].rear = m_centerR + cAngleR;
  m_currentSequence[15].rear = m_centerR + cAngleR;
  m_currentSequence[16].rear = m_centerR + cAngleR;
  m_currentSequence[17].rear = m_centerR + cAngleR;
  m_currentSequence[18].rear = m_centerR + cAngleR;
  m_currentSequence[19].rear = m_centerR + cAngleR/2;
}

// set default values of center motors
// use 0 for now
void _defaultCenterMotor(void) {
  // front motor
  m_currentSequence[0].front = m_centerF;
  m_currentSequence[1].front = m_centerF;
  m_currentSequence[2].front = m_centerF;
  m_currentSequence[3].front = m_centerF;
  m_currentSequence[4].front = m_centerF;
  m_currentSequence[5].front = m_centerF;
  m_currentSequence[6].front = m_centerF;
  m_currentSequence[7].front = m_centerF;
  m_currentSequence[8].front = m_centerF;
  m_currentSequence[9].front = m_centerF;
  m_currentSequence[10].front = m_centerF;
  m_currentSequence[11].front = m_centerF;
  m_currentSequence[12].front = m_centerF;
  m_currentSequence[13].front = m_centerF;
  m_currentSequence[14].front = m_centerF;
  m_currentSequence[15].front = m_centerF;
  m_currentSequence[16].front = m_centerF;
  m_currentSequence[17].front = m_centerF;
  m_currentSequence[18].front = m_centerF;
  m_currentSequence[19].front = m_centerF;
  // rear motor
  m_currentSequence[0].rear = m_centerR;
  m_currentSequence[1].rear = m_centerR;
  m_currentSequence[2].rear = m_centerR;
  m_currentSequence[3].rear = m_centerR;
  m_currentSequence[4].rear = m_centerR;
  m_currentSequence[5].rear = m_centerR;
  m_currentSequence[6].rear = m_centerR;
  m_currentSequence[7].rear = m_centerR;
  m_currentSequence[8].rear = m_centerR;
  m_currentSequence[9].rear = m_centerR;
  m_currentSequence[10].rear = m_centerR;
  m_currentSequence[11].rear = m_centerR;
  m_currentSequence[12].rear = m_centerR;
  m_currentSequence[13].rear = m_centerR;
  m_currentSequence[14].rear = m_centerR;
  m_currentSequence[15].rear = m_centerR;
  m_currentSequence[16].rear = m_centerR;
  m_currentSequence[17].rear = m_centerR;
  m_currentSequence[18].rear = m_centerR;
  m_currentSequence[19].rear = m_centerR;
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
void _updateSequenceLinear(char endPosition1, char endPosition2) {
  // motor 1
  _cBuffer = m_currentSequence[19].m.motor1;
  m_currentSequence[0].m.motor1 = _cBuffer;
  m_currentSequence[1].m.motor1 = _cBuffer + (endPosition1 - _cBuffer) / 14;
  m_currentSequence[2].m.motor1 = _cBuffer + ((endPosition1 - _cBuffer) * 2) / 18;
  m_currentSequence[3].m.motor1 = _cBuffer + ((endPosition1 - _cBuffer) * 3) / 18;
  m_currentSequence[4].m.motor1 = _cBuffer + ((endPosition1 - _cBuffer) * 4) / 18;
  m_currentSequence[5].m.motor1 = _cBuffer + ((endPosition1 - _cBuffer) * 5) / 18;
  m_currentSequence[6].m.motor1 = _cBuffer + ((endPosition1 - _cBuffer) * 6) / 18;
  m_currentSequence[7].m.motor1 = _cBuffer + ((endPosition1 - _cBuffer) * 7) / 18;
  m_currentSequence[8].m.motor1 = _cBuffer + ((endPosition1 - _cBuffer) * 8) / 18;
  m_currentSequence[9].m.motor1 = _cBuffer + ((endPosition1 - _cBuffer) * 9) / 18;
  m_currentSequence[10].m.motor1 = _cBuffer + ((endPosition1 - _cBuffer) * 10) / 18;
  m_currentSequence[11].m.motor1 = _cBuffer + ((endPosition1 - _cBuffer) * 11) / 18;
  m_currentSequence[12].m.motor1 = _cBuffer + ((endPosition1 - _cBuffer) * 12) / 18;
  m_currentSequence[13].m.motor1 = _cBuffer + ((endPosition1 - _cBuffer) * 13) / 18;
  m_currentSequence[14].m.motor1 = _cBuffer + ((endPosition1 - _cBuffer) * 14) / 18;
  m_currentSequence[15].m.motor1 = _cBuffer + ((endPosition1 - _cBuffer) * 15) / 18;
  m_currentSequence[16].m.motor1 = _cBuffer + ((endPosition1 - _cBuffer) * 16) / 18;
  m_currentSequence[17].m.motor1 = _cBuffer + ((endPosition1 - _cBuffer) * 17) / 18;
  m_currentSequence[18].m.motor1 = endPosition1;
  m_currentSequence[19].m.motor1 = endPosition1;
  // motor 2
  _cBuffer = m_currentSequence[19].m.motor2;
  m_currentSequence[0].m.motor2 = _cBuffer;
  m_currentSequence[1].m.motor2 = _cBuffer + (endPosition2 - _cBuffer) / 14;
  m_currentSequence[2].m.motor2 = _cBuffer + ((endPosition2 - _cBuffer) * 2) / 18;
  m_currentSequence[3].m.motor2 = _cBuffer + ((endPosition2 - _cBuffer) * 3) / 18;
  m_currentSequence[4].m.motor2 = _cBuffer + ((endPosition2 - _cBuffer) * 4) / 18;
  m_currentSequence[5].m.motor2 = _cBuffer + ((endPosition2 - _cBuffer) * 5) / 18;
  m_currentSequence[6].m.motor2 = _cBuffer + ((endPosition2 - _cBuffer) * 6) / 18;
  m_currentSequence[7].m.motor2 = _cBuffer + ((endPosition2 - _cBuffer) * 7) / 18;
  m_currentSequence[8].m.motor2 = _cBuffer + ((endPosition2 - _cBuffer) * 8) / 18;
  m_currentSequence[9].m.motor2 = _cBuffer + ((endPosition2 - _cBuffer) * 9) / 18;
  m_currentSequence[10].m.motor2 = _cBuffer + ((endPosition2 - _cBuffer) * 10) / 18;
  m_currentSequence[11].m.motor2 = _cBuffer + ((endPosition2 - _cBuffer) * 11) / 18;
  m_currentSequence[12].m.motor2 = _cBuffer + ((endPosition2 - _cBuffer) * 12) / 18;
  m_currentSequence[13].m.motor2 = _cBuffer + ((endPosition2 - _cBuffer) * 13) / 18;
  m_currentSequence[14].m.motor2 = _cBuffer + ((endPosition2 - _cBuffer) * 14) / 18;
  m_currentSequence[15].m.motor2 = _cBuffer + ((endPosition2 - _cBuffer) * 15) / 18;
  m_currentSequence[16].m.motor2 = _cBuffer + ((endPosition2 - _cBuffer) * 16) / 18;
  m_currentSequence[17].m.motor2 = _cBuffer + ((endPosition2 - _cBuffer) * 17) / 18;
  m_currentSequence[18].m.motor2 = endPosition2;
  m_currentSequence[19].m.motor2 = endPosition2;
}

// linear motor movement from point to point at the end of sequence
void _updateSequenceLinearEnd(char endPosition1, char endPosition2) {
  // motor 1
  _cBuffer = m_currentSequence[19].m.motor1;
  m_currentSequence[0].m.motor1 = _cBuffer;
  m_currentSequence[1].m.motor1 = _cBuffer;
  m_currentSequence[2].m.motor1 = _cBuffer;
  m_currentSequence[3].m.motor1 = _cBuffer;
  m_currentSequence[4].m.motor1 = _cBuffer;
  m_currentSequence[5].m.motor1 = _cBuffer;
  m_currentSequence[6].m.motor1 = _cBuffer;
  m_currentSequence[7].m.motor1 = _cBuffer;
  m_currentSequence[8].m.motor1 = _cBuffer;
  m_currentSequence[9].m.motor1 = _cBuffer;
  m_currentSequence[10].m.motor1 = _cBuffer;
  m_currentSequence[11].m.motor1 = _cBuffer;
  m_currentSequence[12].m.motor1 = _cBuffer;
  m_currentSequence[13].m.motor1 = _cBuffer;
  m_currentSequence[14].m.motor1 = _cBuffer;
  m_currentSequence[15].m.motor1 = _cBuffer;
  m_currentSequence[16].m.motor1 = _cBuffer + (endPosition1 - _cBuffer) / 4;
  m_currentSequence[17].m.motor1 = _cBuffer + ((endPosition1 - _cBuffer) * 2) / 4;
  m_currentSequence[18].m.motor1 = _cBuffer + ((endPosition1 - _cBuffer) * 3) / 4;
  m_currentSequence[19].m.motor1 = endPosition1;
  // motor 2
  _cBuffer = m_currentSequence[19].m.motor2;
  m_currentSequence[0].m.motor2 = _cBuffer;
  m_currentSequence[1].m.motor2 = _cBuffer;
  m_currentSequence[2].m.motor2 = _cBuffer;
  m_currentSequence[3].m.motor2 = _cBuffer;
  m_currentSequence[4].m.motor2 = _cBuffer;
  m_currentSequence[5].m.motor2 = _cBuffer;
  m_currentSequence[6].m.motor2 = _cBuffer;
  m_currentSequence[7].m.motor2 = _cBuffer;
  m_currentSequence[8].m.motor2 = _cBuffer;
  m_currentSequence[9].m.motor2 = _cBuffer;
  m_currentSequence[10].m.motor2 = _cBuffer;
  m_currentSequence[11].m.motor2 = _cBuffer;
  m_currentSequence[12].m.motor2 = _cBuffer;
  m_currentSequence[13].m.motor2 = _cBuffer;
  m_currentSequence[14].m.motor2 = _cBuffer;
  m_currentSequence[15].m.motor2 = _cBuffer;
  m_currentSequence[16].m.motor2 = _cBuffer + (endPosition2 - _cBuffer) / 4;
  m_currentSequence[17].m.motor2 = _cBuffer + ((endPosition2 - _cBuffer) * 2) / 4;
  m_currentSequence[18].m.motor2 = _cBuffer + ((endPosition2 - _cBuffer) * 3) / 4;
  m_currentSequence[19].m.motor2 = endPosition2;
}

// linear motor movement from point to point at the start of sequence
void _updateSequenceLinearStart(char endPosition1, char endPosition2) {
  // motor 1
  _cBuffer = m_currentSequence[19].m.motor1;
  m_currentSequence[0].m.motor1 = _cBuffer;
  m_currentSequence[1].m.motor1 = _cBuffer + (endPosition1 - _cBuffer) / 4;
  m_currentSequence[2].m.motor1 = _cBuffer + ((endPosition1 - _cBuffer) * 2) / 4;
  m_currentSequence[3].m.motor1 = _cBuffer + ((endPosition1 - _cBuffer) * 3) / 4;
  m_currentSequence[4].m.motor1 = endPosition1;
  m_currentSequence[5].m.motor1 = endPosition1;
  m_currentSequence[6].m.motor1 = endPosition1;
  m_currentSequence[7].m.motor1 = endPosition1;
  m_currentSequence[8].m.motor1 = endPosition1;
  m_currentSequence[9].m.motor1 = endPosition1;
  m_currentSequence[10].m.motor1 = endPosition1;
  m_currentSequence[11].m.motor1 = endPosition1;
  m_currentSequence[12].m.motor1 = endPosition1;
  m_currentSequence[13].m.motor1 = endPosition1;
  m_currentSequence[14].m.motor1 = endPosition1;
  m_currentSequence[15].m.motor1 = endPosition1;
  m_currentSequence[16].m.motor1 = endPosition1;
  m_currentSequence[17].m.motor1 = endPosition1;
  m_currentSequence[18].m.motor1 = endPosition1;
  m_currentSequence[19].m.motor1 = endPosition1;
  // motor 2
  _cBuffer = m_currentSequence[19].m.motor2;
  m_currentSequence[0].m.motor2 = _cBuffer;
  m_currentSequence[1].m.motor2 = _cBuffer + (endPosition2 - _cBuffer) / 4;
  m_currentSequence[2].m.motor2 = _cBuffer + ((endPosition2 - _cBuffer) * 2) / 4;
  m_currentSequence[3].m.motor2 = _cBuffer + ((endPosition2 - _cBuffer) * 3) / 4;
  m_currentSequence[4].m.motor2 = endPosition2;
  m_currentSequence[5].m.motor2 = endPosition2;
  m_currentSequence[6].m.motor2 = endPosition2;
  m_currentSequence[7].m.motor2 = endPosition2;
  m_currentSequence[8].m.motor2 = endPosition2;
  m_currentSequence[9].m.motor2 = endPosition2;
  m_currentSequence[10].m.motor2 = endPosition2;
  m_currentSequence[11].m.motor2 = endPosition2;
  m_currentSequence[12].m.motor2 = endPosition2;
  m_currentSequence[13].m.motor2 = endPosition2;
  m_currentSequence[14].m.motor2 = endPosition2;
  m_currentSequence[15].m.motor2 = endPosition2;
  m_currentSequence[16].m.motor2 = endPosition2;
  m_currentSequence[17].m.motor2 = endPosition2;
  m_currentSequence[18].m.motor2 = endPosition2;
  m_currentSequence[19].m.motor2 = endPosition2;
}

// create stand walk sequence
void _updateSequenceStandWalk(char m1, char m2) {
  // motor 1
  m_currentSequence[0].m.motor1 = m1 - liftm1;
  m_currentSequence[1].m.motor1 = m1 - liftm1;
  m_currentSequence[2].m.motor1 = m1 - liftm1 / 4;
  m_currentSequence[3].m.motor1 = m1;
  m_currentSequence[4].m.motor1 = m1;
  m_currentSequence[5].m.motor1 = m1;
  m_currentSequence[6].m.motor1 = m1;
  m_currentSequence[7].m.motor1 = m1;
  m_currentSequence[8].m.motor1 = m1;
  m_currentSequence[9].m.motor1 = m1;
  m_currentSequence[10].m.motor1 = m1;
  m_currentSequence[11].m.motor1 = m1;
  m_currentSequence[12].m.motor1 = m1;
  m_currentSequence[13].m.motor1 = m1;
  m_currentSequence[14].m.motor1 = m1;
  m_currentSequence[15].m.motor1 = m1;
  m_currentSequence[16].m.motor1 = m1;
  m_currentSequence[17].m.motor1 = m1;
  m_currentSequence[18].m.motor1 = m1 - liftm1 / 2;
  m_currentSequence[19].m.motor1 = m1 - liftm1;
  // motor 2
  m_currentSequence[0].m.motor2 = m2 - liftm2;
  m_currentSequence[1].m.motor2 = m2 - liftm2;
  m_currentSequence[2].m.motor2 = m2 - liftm2 / 4;
  m_currentSequence[3].m.motor2 = m2;
  m_currentSequence[4].m.motor2 = m2;
  m_currentSequence[5].m.motor2 = m2;
  m_currentSequence[6].m.motor2 = m2;
  m_currentSequence[7].m.motor2 = m2;
  m_currentSequence[8].m.motor2 = m2;
  m_currentSequence[9].m.motor2 = m2;
  m_currentSequence[10].m.motor2 = m2;
  m_currentSequence[11].m.motor2 = m2;
  m_currentSequence[12].m.motor2 = m2;
  m_currentSequence[13].m.motor2 = m2;
  m_currentSequence[14].m.motor2 = m2;
  m_currentSequence[15].m.motor2 = m2;
  m_currentSequence[16].m.motor2 = m2;
  m_currentSequence[17].m.motor2 = m2;
  m_currentSequence[18].m.motor2 = m2 - liftm2 / 2;
  m_currentSequence[19].m.motor2 = m2 - liftm2;
}

// create walk sequence
void _updateSequenceWalk(char m1[], char m2[]) {
  // motor 1
  m_currentSequence[0].m.motor1 = m1[m_centerStatic] - liftm1;
  m_currentSequence[1].m.motor1 = m1[m_centerStatic - 4] - liftm1;
  m_currentSequence[2].m.motor1 = m1[m_centerStatic - 8] - liftm1;
  m_currentSequence[3].m.motor1 = m1[m_centerStatic - 7] - liftm1 / 4;
  m_currentSequence[4].m.motor1 = m1[m_centerStatic - 6];
  m_currentSequence[5].m.motor1 = m1[m_centerStatic - 5];
  m_currentSequence[6].m.motor1 = m1[m_centerStatic - 4];
  m_currentSequence[7].m.motor1 = m1[m_centerStatic - 3];
  m_currentSequence[8].m.motor1 = m1[m_centerStatic - 2];
  m_currentSequence[9].m.motor1 = m1[m_centerStatic - 1];
  m_currentSequence[10].m.motor1 = m1[m_centerStatic];
  m_currentSequence[11].m.motor1 = m1[m_centerStatic + 1];
  m_currentSequence[12].m.motor1 = m1[m_centerStatic + 2];
  m_currentSequence[13].m.motor1 = m1[m_centerStatic + 3];
  m_currentSequence[14].m.motor1 = m1[m_centerStatic + 4];
  m_currentSequence[15].m.motor1 = m1[m_centerStatic + 5];
  m_currentSequence[16].m.motor1 = m1[m_centerStatic + 6];
  m_currentSequence[17].m.motor1 = m1[m_centerStatic + 7] - liftm1 / 2;
  m_currentSequence[18].m.motor1 = m1[m_centerStatic + 8] - liftm1;
  m_currentSequence[19].m.motor1 = m1[m_centerStatic + 4] - liftm1;
  // motor 2
  m_currentSequence[0].m.motor2 = m2[m_centerStatic] - liftm2;
  m_currentSequence[1].m.motor2 = m2[m_centerStatic - 4] - liftm2;
  m_currentSequence[2].m.motor2 = m2[m_centerStatic - 8] - liftm2;
  m_currentSequence[3].m.motor2 = m2[m_centerStatic - 7] - liftm2 / 4;
  m_currentSequence[4].m.motor2 = m2[m_centerStatic - 6];
  m_currentSequence[5].m.motor2 = m2[m_centerStatic - 5];
  m_currentSequence[6].m.motor2 = m2[m_centerStatic - 4];
  m_currentSequence[7].m.motor2 = m2[m_centerStatic - 3];
  m_currentSequence[8].m.motor2 = m2[m_centerStatic - 2];
  m_currentSequence[9].m.motor2 = m2[m_centerStatic - 1];
  m_currentSequence[10].m.motor2 = m2[m_centerStatic];
  m_currentSequence[11].m.motor2 = m2[m_centerStatic + 1];
  m_currentSequence[12].m.motor2 = m2[m_centerStatic + 2];
  m_currentSequence[13].m.motor2 = m2[m_centerStatic + 3];
  m_currentSequence[14].m.motor2 = m2[m_centerStatic + 4];
  m_currentSequence[15].m.motor2 = m2[m_centerStatic + 5];
  m_currentSequence[16].m.motor2 = m2[m_centerStatic + 6];
  m_currentSequence[17].m.motor2 = m2[m_centerStatic + 7] - liftm2 / 2;
  m_currentSequence[18].m.motor2 = m2[m_centerStatic + 8] - liftm2;
  m_currentSequence[19].m.motor2 = m2[m_centerStatic + 4] - liftm2;
}

// create walk back sequence
void _updateSequenceWalkBack(char m1[], char m2[]) {
  // motor 1
  m_currentSequence[0].m.motor1 = m1[m_centerStatic] - liftm1;
  m_currentSequence[19].m.motor1 = m1[m_centerStatic - 4] - liftm1;
  m_currentSequence[18].m.motor1 = m1[m_centerStatic - 8] - liftm1;
  m_currentSequence[17].m.motor1 = m1[m_centerStatic - 7] - liftm1 / 4;
  m_currentSequence[16].m.motor1 = m1[m_centerStatic - 6];
  m_currentSequence[15].m.motor1 = m1[m_centerStatic - 5];
  m_currentSequence[14].m.motor1 = m1[m_centerStatic - 4];
  m_currentSequence[13].m.motor1 = m1[m_centerStatic - 3];
  m_currentSequence[12].m.motor1 = m1[m_centerStatic - 2];
  m_currentSequence[11].m.motor1 = m1[m_centerStatic - 1];
  m_currentSequence[10].m.motor1 = m1[m_centerStatic];
  m_currentSequence[9].m.motor1 = m1[m_centerStatic + 1];
  m_currentSequence[8].m.motor1 = m1[m_centerStatic + 2];
  m_currentSequence[7].m.motor1 = m1[m_centerStatic + 3];
  m_currentSequence[6].m.motor1 = m1[m_centerStatic + 4];
  m_currentSequence[5].m.motor1 = m1[m_centerStatic + 5];
  m_currentSequence[4].m.motor1 = m1[m_centerStatic + 6];
  m_currentSequence[3].m.motor1 = m1[m_centerStatic + 7] - liftm1 / 2;
  m_currentSequence[2].m.motor1 = m1[m_centerStatic + 8] - liftm1;
  m_currentSequence[1].m.motor1 = m1[m_centerStatic + 4] - liftm1;
  // motor 2
  m_currentSequence[0].m.motor2 = m2[m_centerStatic] - liftm2;
  m_currentSequence[19].m.motor2 = m2[m_centerStatic - 4] - liftm2;
  m_currentSequence[18].m.motor2 = m2[m_centerStatic - 8] - liftm2;
  m_currentSequence[17].m.motor2 = m2[m_centerStatic - 7] - liftm2 / 4;
  m_currentSequence[16].m.motor2 = m2[m_centerStatic - 6];
  m_currentSequence[15].m.motor2 = m2[m_centerStatic - 5];
  m_currentSequence[14].m.motor2 = m2[m_centerStatic - 4];
  m_currentSequence[13].m.motor2 = m2[m_centerStatic - 3];
  m_currentSequence[12].m.motor2 = m2[m_centerStatic - 2];
  m_currentSequence[11].m.motor2 = m2[m_centerStatic - 1];
  m_currentSequence[10].m.motor2 = m2[m_centerStatic];
  m_currentSequence[9].m.motor2 = m2[m_centerStatic + 1];
  m_currentSequence[8].m.motor2 = m2[m_centerStatic + 2];
  m_currentSequence[7].m.motor2 = m2[m_centerStatic + 3];
  m_currentSequence[6].m.motor2 = m2[m_centerStatic + 4];
  m_currentSequence[5].m.motor2 = m2[m_centerStatic + 5];
  m_currentSequence[4].m.motor2 = m2[m_centerStatic + 6];
  m_currentSequence[3].m.motor2 = m2[m_centerStatic + 7] - liftm2 / 2;
  m_currentSequence[2].m.motor2 = m2[m_centerStatic + 8] - liftm2;
  m_currentSequence[1].m.motor2 = m2[m_centerStatic + 4] - liftm2;
}


// get next sequence
unsigned char setNextPattern(unsigned char currentTaskItem) {
  // get new sequence array
  switch (currentTaskItem) {
    case DOSTAND:
    {
      _updateSequenceLinear(_motor1Walk[m_centerStatic], _motor2Walk[m_centerStatic]);
      m_leftRightShift = 0;
      _updateTurn(0);
      m_patternCounter ++;
    }
    break;
    case STANDTOWALK:
    {
      _updateSequenceLinearEnd(_motor1Walk[m_centerStatic] - liftm1, _motor2Walk[m_centerStatic] - liftm2);
      m_leftRightShift = 20;
      _updateTurn(0);
      m_patternCounter ++;
    }
    break;
    case WALKTOSTAND:
    {
      _updateSequenceLinearStart(_motor1Walk[m_centerStatic], _motor2Walk[m_centerStatic]);
      m_leftRightShift = 20;
      _updateTurn(0);
      m_patternCounter ++;
    }
    break;
    case STANDWALK:
    {
      _updateSequenceStandWalk(_motor1Walk[m_centerStatic], _motor2Walk[m_centerStatic]);
      m_leftRightShift = 10;
      m_patternCounter ++;
      return STANDWALK; // used to set direction
    }
    break;
    case STANDWALKLEFT:
    {
      _updateSequenceStandWalk(_motor1Walk[m_centerStatic], _motor2Walk[m_centerStatic]);
      m_leftRightShift = 10;
      _updateTurn(-10);
      m_patternCounter ++;
    }
    break;
    case STANDWALKRIGHT:
    {
      _updateSequenceStandWalk(_motor1Walk[m_centerStatic], _motor2Walk[m_centerStatic]);
      m_leftRightShift = 10;
      _updateTurn(10);
      m_patternCounter ++;
    }
    break;
    case STANDWALKSHIFTLEFT:
    {
      _updateSequenceStandWalk(_motor1Walk[m_centerStatic], _motor2Walk[m_centerStatic]);
      m_leftRightShift = 10;
      _updateShift(-10);
      m_patternCounter ++;
    }
    break;
    case STANDWALKSHIFTRIGHT:
    {
      _updateSequenceStandWalk(_motor1Walk[m_centerStatic], _motor2Walk[m_centerStatic]);
      m_leftRightShift = 10;
      _updateShift(10);
      m_patternCounter ++;
    }
    break;
    case WALKFORWARD:
    {
      _updateSequenceWalk(_motor1Walk, _motor2Walk);
      m_leftRightShift = 10;
      m_patternCounter ++;
      return WALKFORWARD; // used to set direction
    }
    break;
    case WALKLEFT:
    {
      _updateSequenceWalk(_motor1Walk, _motor2Walk);
      m_leftRightShift = 10;
      _updateTurn(-10);
      m_patternCounter ++;
    }
    break;
    case WALKRIGHT:
    {
      _updateSequenceWalk(_motor1Walk, _motor2Walk);
      m_leftRightShift = 10;
      _updateTurn(10);
      m_patternCounter ++;
    }
    break;
    case WALKSHIFTLEFT:
    {
      _updateSequenceWalk(_motor1Walk, _motor2Walk);
      m_leftRightShift = 10;
      _updateShift(-10);
      m_patternCounter ++;
    }
    break;
    case WALKSHIFTRIGHT:
    {
      _updateSequenceWalk(_motor1Walk, _motor2Walk);
      m_leftRightShift = 10;
      _updateShift(10);
      m_patternCounter ++;
    }
    break;
    case WALKBACK:
    {
      _updateSequenceWalkBack(_motor1Walk, _motor2Walk);
      m_leftRightShift = 10;
      _updateTurn(0);
      m_patternCounter ++;
    }
    break;
    case DODOWN:
    {
      _updateSequenceLinear(30, 30);
      m_leftRightShift = 10;
      _updateTurn(0);
      m_patternCounter ++;
    }
    break;
    case RESET:
    {
      m_patternCounter = 0;
    }
    break;
    case DOREPEAT:
    {
      if (m_patternCounter > 0) {
        m_patternCounter --;
      }
    }
    break;
    case DONE:
      m_patternCounter = 0;
      return DONE;
    break;
    case DOCALIBRATION:
      _updateSequenceLinear(90, 90);
      m_leftRightShift = 0;
      return DOCALIBRATION;
    break;
    case DOCALIBRATION_1:
      _updateSequenceLinear(90, 70);
      m_leftRightShift = 0;
      return DOCALIBRATION;
    break;
    case DOCALIBRATION_2:
      _updateSequenceLinear(70, 90);
      m_leftRightShift = 0;
      return DOCALIBRATION;
    break;
    case RESETDIRECTION:
      m_patternCounter ++;
      return RESETDIRECTION;
    break;
    default:
      m_patternCounter = 0;
      return currentTaskItem;
    break;
  }
  return INPROGRESS; 
}

// update turning
void updateTurnPattern(char cAngle) {
  if (cAngle == 0) {
    _defaultCenterMotor();
  } else {
    _updateTurn(cAngle);
  }
}
