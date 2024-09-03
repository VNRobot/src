/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Robot legs motion patterns
*/

// structure for four legs
struct legs {
  unsigned char fl;
  unsigned char fr;
  unsigned char rl;
  unsigned char rr;
};
// front motor, rear motor, motor1, motor2
struct sequence {
  char front;
  char rear;
  motors m;
};

// servos angles for forward shift 
// forward shift(mm)          -40,           -30,           -20,           -10,          0,         10,         20,         30
char _motor1Walk[24] =  { 46,  47,  48,  50,  52,  54,  56,  58,  60,  62,  64, 66, 69, 72, 75, 78, 81, 84, 87, 90, 94, 98,102,106};
char _motor2Walk[24] =  { 96,  90,  85,  81,  77,  73,  69,  65,  61,  57,  54, 51, 48, 45, 42, 40, 38, 36, 34, 33, 32, 31, 30, 29};
// center position in the pattern array
int _centerAbsolute = 13; // (range 8 to 15) bigger the number more weight on front
// static forward ballance
int _centerStatic = _centerAbsolute;
// char buffer for temporary use
char _cBuffer;
// leg lift angles
unsigned char _liftm1 = 8;
unsigned char _liftm2 = 24;
// center motor default position
char _centerF = 0; // positive more distance between legs
char _centerR = 0; // positive more distance between legs
// points to _currentSequence for every leg
legs _sequenceCount = { 0, 0, 0, 0};
// dynamic side ballance
char _sideBallance = 0;
char _sideUpLeft = 0;
char _sideUpRight = 0;
// 20 positions per sequence
// all legs using the same sequence with different pozition shift
sequence _currentSequence[20] = {
  0,  0,  30, 30,
  0,  0,  30, 30,
  0,  0,  30, 30,
  0,  0,  30, 30,

  0,  0,  30, 30,
  0,  0,  30, 30,
  0,  0,  30, 30,
  0,  0,  30, 30,

  0,  0,  30, 30,
  0,  0,  30, 30,
  0,  0,  30, 30,
  0,  0,  30, 30,

  0,  0,  30, 30,
  0,  0,  30, 30,
  0,  0,  30, 30,
  0,  0,  30, 30,

  0,  0,  30, 30,
  0,  0,  30, 30,
  0,  0,  30, 30,
  0,  0,  30, 30
};
// sequence shift between right and left legs
unsigned char _leftRightShift = 0;
// motors values for 10 motors
allMotors _motorValue = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// set new values of center motors
void _updateCenterMotor(char cAngleF, char cAngleR) {
  // front motor
  _currentSequence[0].front = _centerF - cAngleF/2;
  _currentSequence[1].front = _centerF - cAngleF;
  _currentSequence[2].front = _centerF - cAngleF;
  _currentSequence[3].front = _centerF - cAngleF;
  _currentSequence[4].front = _centerF - cAngleF;
  _currentSequence[5].front = _centerF - cAngleF;
  _currentSequence[6].front = _centerF - cAngleF;
  _currentSequence[7].front = _centerF - cAngleF;
  _currentSequence[8].front = _centerF - cAngleF;
  _currentSequence[9].front = _centerF - cAngleF/2;
  _currentSequence[10].front = _centerF + cAngleF/2;
  _currentSequence[11].front = _centerF + cAngleF;
  _currentSequence[12].front = _centerF + cAngleF;
  _currentSequence[13].front = _centerF + cAngleF;
  _currentSequence[14].front = _centerF + cAngleF;
  _currentSequence[15].front = _centerF + cAngleF;
  _currentSequence[16].front = _centerF + cAngleF;
  _currentSequence[17].front = _centerF + cAngleF;
  _currentSequence[18].front = _centerF + cAngleF;
  _currentSequence[19].front = _centerF + cAngleF/2;
  // rear motor
  _currentSequence[0].rear = _centerR - cAngleR/2;
  _currentSequence[1].rear = _centerR - cAngleR;
  _currentSequence[2].rear = _centerR - cAngleR;
  _currentSequence[3].rear = _centerR - cAngleR;
  _currentSequence[4].rear = _centerR - cAngleR;
  _currentSequence[5].rear = _centerR - cAngleR;
  _currentSequence[6].rear = _centerR - cAngleR;
  _currentSequence[7].rear = _centerR - cAngleR;
  _currentSequence[8].rear = _centerR - cAngleR;
  _currentSequence[9].rear = _centerR - cAngleR/2;
  _currentSequence[10].rear = _centerR + cAngleR/2;
  _currentSequence[11].rear = _centerR + cAngleR;
  _currentSequence[12].rear = _centerR + cAngleR;
  _currentSequence[13].rear = _centerR + cAngleR;
  _currentSequence[14].rear = _centerR + cAngleR;
  _currentSequence[15].rear = _centerR + cAngleR;
  _currentSequence[16].rear = _centerR + cAngleR;
  _currentSequence[17].rear = _centerR + cAngleR;
  _currentSequence[18].rear = _centerR + cAngleR;
  _currentSequence[19].rear = _centerR + cAngleR/2;
}

// set default values of center motors
// use 0 for now
void _defaultCenterMotor(void) {
  // front motor
  _currentSequence[0].front = _centerF;
  _currentSequence[1].front = _centerF;
  _currentSequence[2].front = _centerF;
  _currentSequence[3].front = _centerF;
  _currentSequence[4].front = _centerF;
  _currentSequence[5].front = _centerF;
  _currentSequence[6].front = _centerF;
  _currentSequence[7].front = _centerF;
  _currentSequence[8].front = _centerF;
  _currentSequence[9].front = _centerF;
  _currentSequence[10].front = _centerF;
  _currentSequence[11].front = _centerF;
  _currentSequence[12].front = _centerF;
  _currentSequence[13].front = _centerF;
  _currentSequence[14].front = _centerF;
  _currentSequence[15].front = _centerF;
  _currentSequence[16].front = _centerF;
  _currentSequence[17].front = _centerF;
  _currentSequence[18].front = _centerF;
  _currentSequence[19].front = _centerF;
  // rear motor
  _currentSequence[0].rear = _centerR;
  _currentSequence[1].rear = _centerR;
  _currentSequence[2].rear = _centerR;
  _currentSequence[3].rear = _centerR;
  _currentSequence[4].rear = _centerR;
  _currentSequence[5].rear = _centerR;
  _currentSequence[6].rear = _centerR;
  _currentSequence[7].rear = _centerR;
  _currentSequence[8].rear = _centerR;
  _currentSequence[9].rear = _centerR;
  _currentSequence[10].rear = _centerR;
  _currentSequence[11].rear = _centerR;
  _currentSequence[12].rear = _centerR;
  _currentSequence[13].rear = _centerR;
  _currentSequence[14].rear = _centerR;
  _currentSequence[15].rear = _centerR;
  _currentSequence[16].rear = _centerR;
  _currentSequence[17].rear = _centerR;
  _currentSequence[18].rear = _centerR;
  _currentSequence[19].rear = _centerR;
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
  _cBuffer = _currentSequence[19].m.motor1;
  _currentSequence[0].m.motor1 = _cBuffer;
  _currentSequence[1].m.motor1 = _cBuffer + (endPosition1 - _cBuffer) / 14;
  _currentSequence[2].m.motor1 = _cBuffer + ((endPosition1 - _cBuffer) * 2) / 18;
  _currentSequence[3].m.motor1 = _cBuffer + ((endPosition1 - _cBuffer) * 3) / 18;
  _currentSequence[4].m.motor1 = _cBuffer + ((endPosition1 - _cBuffer) * 4) / 18;
  _currentSequence[5].m.motor1 = _cBuffer + ((endPosition1 - _cBuffer) * 5) / 18;
  _currentSequence[6].m.motor1 = _cBuffer + ((endPosition1 - _cBuffer) * 6) / 18;
  _currentSequence[7].m.motor1 = _cBuffer + ((endPosition1 - _cBuffer) * 7) / 18;
  _currentSequence[8].m.motor1 = _cBuffer + ((endPosition1 - _cBuffer) * 8) / 18;
  _currentSequence[9].m.motor1 = _cBuffer + ((endPosition1 - _cBuffer) * 9) / 18;
  _currentSequence[10].m.motor1 = _cBuffer + ((endPosition1 - _cBuffer) * 10) / 18;
  _currentSequence[11].m.motor1 = _cBuffer + ((endPosition1 - _cBuffer) * 11) / 18;
  _currentSequence[12].m.motor1 = _cBuffer + ((endPosition1 - _cBuffer) * 12) / 18;
  _currentSequence[13].m.motor1 = _cBuffer + ((endPosition1 - _cBuffer) * 13) / 18;
  _currentSequence[14].m.motor1 = _cBuffer + ((endPosition1 - _cBuffer) * 14) / 18;
  _currentSequence[15].m.motor1 = _cBuffer + ((endPosition1 - _cBuffer) * 15) / 18;
  _currentSequence[16].m.motor1 = _cBuffer + ((endPosition1 - _cBuffer) * 16) / 18;
  _currentSequence[17].m.motor1 = _cBuffer + ((endPosition1 - _cBuffer) * 17) / 18;
  _currentSequence[18].m.motor1 = endPosition1;
  _currentSequence[19].m.motor1 = endPosition1;
  // motor 2
  _cBuffer = _currentSequence[19].m.motor2;
  _currentSequence[0].m.motor2 = _cBuffer;
  _currentSequence[1].m.motor2 = _cBuffer + (endPosition2 - _cBuffer) / 14;
  _currentSequence[2].m.motor2 = _cBuffer + ((endPosition2 - _cBuffer) * 2) / 18;
  _currentSequence[3].m.motor2 = _cBuffer + ((endPosition2 - _cBuffer) * 3) / 18;
  _currentSequence[4].m.motor2 = _cBuffer + ((endPosition2 - _cBuffer) * 4) / 18;
  _currentSequence[5].m.motor2 = _cBuffer + ((endPosition2 - _cBuffer) * 5) / 18;
  _currentSequence[6].m.motor2 = _cBuffer + ((endPosition2 - _cBuffer) * 6) / 18;
  _currentSequence[7].m.motor2 = _cBuffer + ((endPosition2 - _cBuffer) * 7) / 18;
  _currentSequence[8].m.motor2 = _cBuffer + ((endPosition2 - _cBuffer) * 8) / 18;
  _currentSequence[9].m.motor2 = _cBuffer + ((endPosition2 - _cBuffer) * 9) / 18;
  _currentSequence[10].m.motor2 = _cBuffer + ((endPosition2 - _cBuffer) * 10) / 18;
  _currentSequence[11].m.motor2 = _cBuffer + ((endPosition2 - _cBuffer) * 11) / 18;
  _currentSequence[12].m.motor2 = _cBuffer + ((endPosition2 - _cBuffer) * 12) / 18;
  _currentSequence[13].m.motor2 = _cBuffer + ((endPosition2 - _cBuffer) * 13) / 18;
  _currentSequence[14].m.motor2 = _cBuffer + ((endPosition2 - _cBuffer) * 14) / 18;
  _currentSequence[15].m.motor2 = _cBuffer + ((endPosition2 - _cBuffer) * 15) / 18;
  _currentSequence[16].m.motor2 = _cBuffer + ((endPosition2 - _cBuffer) * 16) / 18;
  _currentSequence[17].m.motor2 = _cBuffer + ((endPosition2 - _cBuffer) * 17) / 18;
  _currentSequence[18].m.motor2 = endPosition2;
  _currentSequence[19].m.motor2 = endPosition2;
}

// linear motor movement from point to point at the end of sequence
void _updateSequenceLinearEnd(char endPosition1, char endPosition2) {
  // motor 1
  _cBuffer = _currentSequence[19].m.motor1;
  _currentSequence[0].m.motor1 = _cBuffer;
  _currentSequence[1].m.motor1 = _cBuffer;
  _currentSequence[2].m.motor1 = _cBuffer;
  _currentSequence[3].m.motor1 = _cBuffer;
  _currentSequence[4].m.motor1 = _cBuffer;
  _currentSequence[5].m.motor1 = _cBuffer;
  _currentSequence[6].m.motor1 = _cBuffer;
  _currentSequence[7].m.motor1 = _cBuffer;
  _currentSequence[8].m.motor1 = _cBuffer;
  _currentSequence[9].m.motor1 = _cBuffer;
  _currentSequence[10].m.motor1 = _cBuffer;
  _currentSequence[11].m.motor1 = _cBuffer;
  _currentSequence[12].m.motor1 = _cBuffer;
  _currentSequence[13].m.motor1 = _cBuffer;
  _currentSequence[14].m.motor1 = _cBuffer;
  _currentSequence[15].m.motor1 = _cBuffer;
  _currentSequence[16].m.motor1 = _cBuffer + (endPosition1 - _cBuffer) / 4;
  _currentSequence[17].m.motor1 = _cBuffer + ((endPosition1 - _cBuffer) * 2) / 4;
  _currentSequence[18].m.motor1 = _cBuffer + ((endPosition1 - _cBuffer) * 3) / 4;
  _currentSequence[19].m.motor1 = endPosition1;
  // motor 2
  _cBuffer = _currentSequence[19].m.motor2;
  _currentSequence[0].m.motor2 = _cBuffer;
  _currentSequence[1].m.motor2 = _cBuffer;
  _currentSequence[2].m.motor2 = _cBuffer;
  _currentSequence[3].m.motor2 = _cBuffer;
  _currentSequence[4].m.motor2 = _cBuffer;
  _currentSequence[5].m.motor2 = _cBuffer;
  _currentSequence[6].m.motor2 = _cBuffer;
  _currentSequence[7].m.motor2 = _cBuffer;
  _currentSequence[8].m.motor2 = _cBuffer;
  _currentSequence[9].m.motor2 = _cBuffer;
  _currentSequence[10].m.motor2 = _cBuffer;
  _currentSequence[11].m.motor2 = _cBuffer;
  _currentSequence[12].m.motor2 = _cBuffer;
  _currentSequence[13].m.motor2 = _cBuffer;
  _currentSequence[14].m.motor2 = _cBuffer;
  _currentSequence[15].m.motor2 = _cBuffer;
  _currentSequence[16].m.motor2 = _cBuffer + (endPosition2 - _cBuffer) / 4;
  _currentSequence[17].m.motor2 = _cBuffer + ((endPosition2 - _cBuffer) * 2) / 4;
  _currentSequence[18].m.motor2 = _cBuffer + ((endPosition2 - _cBuffer) * 3) / 4;
  _currentSequence[19].m.motor2 = endPosition2;
}

// linear motor movement from point to point at the start of sequence
void _updateSequenceLinearStart(char endPosition1, char endPosition2) {
  // motor 1
  _cBuffer = _currentSequence[19].m.motor1;
  _currentSequence[0].m.motor1 = _cBuffer;
  _currentSequence[1].m.motor1 = _cBuffer + (endPosition1 - _cBuffer) / 4;
  _currentSequence[2].m.motor1 = _cBuffer + ((endPosition1 - _cBuffer) * 2) / 4;
  _currentSequence[3].m.motor1 = _cBuffer + ((endPosition1 - _cBuffer) * 3) / 4;
  _currentSequence[4].m.motor1 = endPosition1;
  _currentSequence[5].m.motor1 = endPosition1;
  _currentSequence[6].m.motor1 = endPosition1;
  _currentSequence[7].m.motor1 = endPosition1;
  _currentSequence[8].m.motor1 = endPosition1;
  _currentSequence[9].m.motor1 = endPosition1;
  _currentSequence[10].m.motor1 = endPosition1;
  _currentSequence[11].m.motor1 = endPosition1;
  _currentSequence[12].m.motor1 = endPosition1;
  _currentSequence[13].m.motor1 = endPosition1;
  _currentSequence[14].m.motor1 = endPosition1;
  _currentSequence[15].m.motor1 = endPosition1;
  _currentSequence[16].m.motor1 = endPosition1;
  _currentSequence[17].m.motor1 = endPosition1;
  _currentSequence[18].m.motor1 = endPosition1;
  _currentSequence[19].m.motor1 = endPosition1;
  // motor 2
  _cBuffer = _currentSequence[19].m.motor2;
  _currentSequence[0].m.motor2 = _cBuffer;
  _currentSequence[1].m.motor2 = _cBuffer + (endPosition2 - _cBuffer) / 4;
  _currentSequence[2].m.motor2 = _cBuffer + ((endPosition2 - _cBuffer) * 2) / 4;
  _currentSequence[3].m.motor2 = _cBuffer + ((endPosition2 - _cBuffer) * 3) / 4;
  _currentSequence[4].m.motor2 = endPosition2;
  _currentSequence[5].m.motor2 = endPosition2;
  _currentSequence[6].m.motor2 = endPosition2;
  _currentSequence[7].m.motor2 = endPosition2;
  _currentSequence[8].m.motor2 = endPosition2;
  _currentSequence[9].m.motor2 = endPosition2;
  _currentSequence[10].m.motor2 = endPosition2;
  _currentSequence[11].m.motor2 = endPosition2;
  _currentSequence[12].m.motor2 = endPosition2;
  _currentSequence[13].m.motor2 = endPosition2;
  _currentSequence[14].m.motor2 = endPosition2;
  _currentSequence[15].m.motor2 = endPosition2;
  _currentSequence[16].m.motor2 = endPosition2;
  _currentSequence[17].m.motor2 = endPosition2;
  _currentSequence[18].m.motor2 = endPosition2;
  _currentSequence[19].m.motor2 = endPosition2;
}

// create stand walk sequence
void _updateSequenceStandWalk(char m1, char m2) {
  // motor 1
  _currentSequence[0].m.motor1 = m1 - _liftm1;
  _currentSequence[1].m.motor1 = m1 - _liftm1;
  _currentSequence[2].m.motor1 = m1 - _liftm1 / 2;
  _currentSequence[3].m.motor1 = m1 - _liftm1 / 4;
  _currentSequence[4].m.motor1 = m1;
  _currentSequence[5].m.motor1 = m1;
  _currentSequence[6].m.motor1 = m1;
  _currentSequence[7].m.motor1 = m1;
  _currentSequence[8].m.motor1 = m1;
  _currentSequence[9].m.motor1 = m1;
  _currentSequence[10].m.motor1 = m1;
  _currentSequence[11].m.motor1 = m1;
  _currentSequence[12].m.motor1 = m1;
  _currentSequence[13].m.motor1 = m1;
  _currentSequence[14].m.motor1 = m1;
  _currentSequence[15].m.motor1 = m1;
  _currentSequence[16].m.motor1 = m1;
  _currentSequence[17].m.motor1 = m1;
  _currentSequence[18].m.motor1 = m1;
  _currentSequence[19].m.motor1 = m1 - _liftm1 / 2;
  // motor 2
  _currentSequence[0].m.motor2 = m2 - _liftm2;
  _currentSequence[1].m.motor2 = m2 - _liftm2 / 2;
  _currentSequence[2].m.motor2 = m2 - _liftm2 / 4;
  _currentSequence[3].m.motor2 = m2;
  _currentSequence[4].m.motor2 = m2;
  _currentSequence[5].m.motor2 = m2;
  _currentSequence[6].m.motor2 = m2;
  _currentSequence[7].m.motor2 = m2;
  _currentSequence[8].m.motor2 = m2;
  _currentSequence[9].m.motor2 = m2;
  _currentSequence[10].m.motor2 = m2;
  _currentSequence[11].m.motor2 = m2;
  _currentSequence[12].m.motor2 = m2;
  _currentSequence[13].m.motor2 = m2;
  _currentSequence[14].m.motor2 = m2;
  _currentSequence[15].m.motor2 = m2;
  _currentSequence[16].m.motor2 = m2;
  _currentSequence[17].m.motor2 = m2;
  _currentSequence[18].m.motor2 = m2 - _liftm2 / 2;
  _currentSequence[19].m.motor2 = m2 - _liftm2;
}

// create walk sequence
void _updateSequenceWalk(char m1[], char m2[]) {
  // motor 1
  _currentSequence[0].m.motor1 = m1[_centerStatic] - _liftm1;
  _currentSequence[1].m.motor1 = m1[_centerStatic - 5] - _liftm1;
  _currentSequence[2].m.motor1 = m1[_centerStatic - 8] - _liftm1;
  _currentSequence[3].m.motor1 = m1[_centerStatic - 7] - _liftm1;
  _currentSequence[4].m.motor1 = m1[_centerStatic - 6];
  _currentSequence[5].m.motor1 = m1[_centerStatic - 5];
  _currentSequence[6].m.motor1 = m1[_centerStatic - 4];
  _currentSequence[7].m.motor1 = m1[_centerStatic - 3];
  _currentSequence[8].m.motor1 = m1[_centerStatic - 2];
  _currentSequence[9].m.motor1 = m1[_centerStatic - 1];
  _currentSequence[10].m.motor1 = m1[_centerStatic];
  _currentSequence[11].m.motor1 = m1[_centerStatic + 1];
  _currentSequence[12].m.motor1 = m1[_centerStatic + 2];
  _currentSequence[13].m.motor1 = m1[_centerStatic + 3];
  _currentSequence[14].m.motor1 = m1[_centerStatic + 4];
  _currentSequence[15].m.motor1 = m1[_centerStatic + 5];
  _currentSequence[16].m.motor1 = m1[_centerStatic + 6];
  _currentSequence[17].m.motor1 = m1[_centerStatic + 7];
  _currentSequence[18].m.motor1 = m1[_centerStatic + 8];
  _currentSequence[19].m.motor1 = m1[_centerStatic + 5] - _liftm1;
  // motor 2
  _currentSequence[0].m.motor2 = m2[_centerStatic] - _liftm2;
  _currentSequence[1].m.motor2 = m2[_centerStatic - 5] - _liftm2;
  _currentSequence[2].m.motor2 = m2[_centerStatic - 8] - _liftm2;
  _currentSequence[3].m.motor2 = m2[_centerStatic - 7];
  _currentSequence[4].m.motor2 = m2[_centerStatic - 6];
  _currentSequence[5].m.motor2 = m2[_centerStatic - 5];
  _currentSequence[6].m.motor2 = m2[_centerStatic - 4];
  _currentSequence[7].m.motor2 = m2[_centerStatic - 3];
  _currentSequence[8].m.motor2 = m2[_centerStatic - 2];
  _currentSequence[9].m.motor2 = m2[_centerStatic - 1];
  _currentSequence[10].m.motor2 = m2[_centerStatic];
  _currentSequence[11].m.motor2 = m2[_centerStatic + 1];
  _currentSequence[12].m.motor2 = m2[_centerStatic + 2];
  _currentSequence[13].m.motor2 = m2[_centerStatic + 3];
  _currentSequence[14].m.motor2 = m2[_centerStatic + 4];
  _currentSequence[15].m.motor2 = m2[_centerStatic + 5];
  _currentSequence[16].m.motor2 = m2[_centerStatic + 6];
  _currentSequence[17].m.motor2 = m2[_centerStatic + 7];
  _currentSequence[18].m.motor2 = m2[_centerStatic + 8] - _liftm2;
  _currentSequence[19].m.motor2 = m2[_centerStatic + 5] - _liftm2;
}

// create walk back sequence
void _updateSequenceWalkBack(char m1[], char m2[]) {
  // motor 1
  _currentSequence[0].m.motor1 = m1[_centerStatic] - _liftm1;
  _currentSequence[19].m.motor1 = m1[_centerStatic - 5] - _liftm1;
  _currentSequence[18].m.motor1 = m1[_centerStatic - 8];
  _currentSequence[17].m.motor1 = m1[_centerStatic - 7];
  _currentSequence[16].m.motor1 = m1[_centerStatic - 6];
  _currentSequence[15].m.motor1 = m1[_centerStatic - 5];
  _currentSequence[14].m.motor1 = m1[_centerStatic - 4];
  _currentSequence[13].m.motor1 = m1[_centerStatic - 3];
  _currentSequence[12].m.motor1 = m1[_centerStatic - 2];
  _currentSequence[11].m.motor1 = m1[_centerStatic - 1];
  _currentSequence[10].m.motor1 = m1[_centerStatic];
  _currentSequence[9].m.motor1 = m1[_centerStatic + 1];
  _currentSequence[8].m.motor1 = m1[_centerStatic + 2];
  _currentSequence[7].m.motor1 = m1[_centerStatic + 3];
  _currentSequence[6].m.motor1 = m1[_centerStatic + 4];
  _currentSequence[5].m.motor1 = m1[_centerStatic + 5];
  _currentSequence[4].m.motor1 = m1[_centerStatic + 6];
  _currentSequence[3].m.motor1 = m1[_centerStatic + 7] - _liftm1;
  _currentSequence[2].m.motor1 = m1[_centerStatic + 8] - _liftm1;
  _currentSequence[1].m.motor1 = m1[_centerStatic + 5] - _liftm1;
  // motor 2
  _currentSequence[0].m.motor2 = m2[_centerStatic] - _liftm2;
  _currentSequence[19].m.motor2 = m2[_centerStatic - 5] - _liftm2;
  _currentSequence[18].m.motor2 = m2[_centerStatic - 8] - _liftm2;
  _currentSequence[17].m.motor2 = m2[_centerStatic - 7];
  _currentSequence[16].m.motor2 = m2[_centerStatic - 6];
  _currentSequence[15].m.motor2 = m2[_centerStatic - 5];
  _currentSequence[14].m.motor2 = m2[_centerStatic - 4];
  _currentSequence[13].m.motor2 = m2[_centerStatic - 3];
  _currentSequence[12].m.motor2 = m2[_centerStatic - 2];
  _currentSequence[11].m.motor2 = m2[_centerStatic - 1];
  _currentSequence[10].m.motor2 = m2[_centerStatic];
  _currentSequence[9].m.motor2 = m2[_centerStatic + 1];
  _currentSequence[8].m.motor2 = m2[_centerStatic + 2];
  _currentSequence[7].m.motor2 = m2[_centerStatic + 3];
  _currentSequence[6].m.motor2 = m2[_centerStatic + 4];
  _currentSequence[5].m.motor2 = m2[_centerStatic + 5];
  _currentSequence[4].m.motor2 = m2[_centerStatic + 6];
  _currentSequence[3].m.motor2 = m2[_centerStatic + 7];
  _currentSequence[2].m.motor2 = m2[_centerStatic + 8] - _liftm2;
  _currentSequence[1].m.motor2 = m2[_centerStatic + 5] - _liftm2;
}


// get next sequence
unsigned char setPattern(unsigned char currentTaskItem) {
  // get new sequence array
  switch (currentTaskItem) {
    case DOSTAND:
    {
      _updateSequenceLinear(_motor1Walk[_centerStatic], _motor2Walk[_centerStatic]);
      _leftRightShift = 0;
      _updateTurn(0);
    }
    break;
    case STANDTOWALK:
    {
      _updateSequenceLinearEnd(_motor1Walk[_centerStatic] - _liftm1, _motor2Walk[_centerStatic] - _liftm2);
      _leftRightShift = 20;
      _updateTurn(0);
    }
    break;
    case WALKTOSTAND:
    {
      _updateSequenceLinearStart(_motor1Walk[_centerStatic], _motor2Walk[_centerStatic]);
      _leftRightShift = 20;
      _updateTurn(0);
    }
    break;
    case STANDWALK:
    {
      _updateSequenceStandWalk(_motor1Walk[_centerStatic], _motor2Walk[_centerStatic]);
      _leftRightShift = 10;
      return STANDWALK; // used to set direction
    }
    break;
    case STANDWALKLEFT:
    {
      _updateSequenceStandWalk(_motor1Walk[_centerStatic], _motor2Walk[_centerStatic]);
      _leftRightShift = 10;
      _updateTurn(-10);
    }
    break;
    case STANDWALKRIGHT:
    {
      _updateSequenceStandWalk(_motor1Walk[_centerStatic], _motor2Walk[_centerStatic]);
      _leftRightShift = 10;
      _updateTurn(10);
    }
    break;
    case STANDWALKSHIFTLEFT:
    {
      _updateSequenceStandWalk(_motor1Walk[_centerStatic], _motor2Walk[_centerStatic]);
      _leftRightShift = 10;
      _updateShift(-10);
    }
    break;
    case STANDWALKSHIFTRIGHT:
    {
      _updateSequenceStandWalk(_motor1Walk[_centerStatic], _motor2Walk[_centerStatic]);
      _leftRightShift = 10;
      _updateShift(10);
    }
    break;
    case WALKFORWARD:
    {
      _updateSequenceWalk(_motor1Walk, _motor2Walk);
      _leftRightShift = 10;
      return WALKFORWARD; // used to set direction
    }
    break;
    case WALKLEFT:
    {
      _updateSequenceWalk(_motor1Walk, _motor2Walk);
      _leftRightShift = 10;
      _updateTurn(-10);
    }
    break;
    case WALKRIGHT:
    {
      _updateSequenceWalk(_motor1Walk, _motor2Walk);
      _leftRightShift = 10;
      _updateTurn(10);
    }
    break;
    case WALKSHIFTLEFT:
    {
      _updateSequenceWalk(_motor1Walk, _motor2Walk);
      _leftRightShift = 10;
      _updateShift(-10);
    }
    break;
    case WALKSHIFTRIGHT:
    {
      _updateSequenceWalk(_motor1Walk, _motor2Walk);
      _leftRightShift = 10;
      _updateShift(10);
    }
    break;
    case WALKBACK:
    {
      _updateSequenceWalkBack(_motor1Walk, _motor2Walk);
      _leftRightShift = 10;
      _updateTurn(0);
    }
    break;
    case DODOWN:
    {
      _updateSequenceLinear(30, 30);
      _leftRightShift = 10;
      _updateTurn(0);
    }
    break;
    case DONE:
      return DONE;
    break;
    case DOCALIBRATION:
      _updateSequenceLinear(90, 90);
      _leftRightShift = 0;
      return DOCALIBRATION;
    break;
    case DOCALIBRATION_1:
      _updateSequenceLinear(90, 70);
      _leftRightShift = 0;
      return DOCALIBRATION;
    break;
    case DOCALIBRATION_2:
      _updateSequenceLinear(70, 90);
      _leftRightShift = 0;
      return DOCALIBRATION;
    break;
    case RESETDIRECTION:
      return RESETDIRECTION;
    break;
    default:
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

// update servo motors values
allMotors updateMotorsPatterns(allMotors calibration) {
  // set motors angle values
  _motorValue.front       = (_currentSequence[_sequenceCount.fl].front          + calibration.front);
  _motorValue.rear        = (_currentSequence[_sequenceCount.fl].rear           + calibration.rear);
  _motorValue.m.fl.motor1 = (_currentSequence[_sequenceCount.fl].m.motor1  - 30 + calibration.m.fl.motor1 - _sideUpLeft / 3);
  _motorValue.m.fl.motor2 = (_currentSequence[_sequenceCount.fl].m.motor2  - 30 + calibration.m.fl.motor2 - _sideUpLeft);
  _motorValue.m.fr.motor1 = (_currentSequence[_sequenceCount.fr].m.motor1  - 30 + calibration.m.fr.motor1 + _sideUpRight / 3);
  _motorValue.m.fr.motor2 = (_currentSequence[_sequenceCount.fr].m.motor2  - 30 + calibration.m.fr.motor2 + _sideUpRight);
  _motorValue.m.rl.motor1 = (_currentSequence[_sequenceCount.rl].m.motor1  - 30 + calibration.m.rl.motor1 - _sideUpLeft / 3);
  _motorValue.m.rl.motor2 = (_currentSequence[_sequenceCount.rl].m.motor2  - 30 + calibration.m.rl.motor2 - _sideUpLeft);
  _motorValue.m.rr.motor1 = (_currentSequence[_sequenceCount.rr].m.motor1  - 30 + calibration.m.rr.motor1 + _sideUpRight / 3);
  _motorValue.m.rr.motor2 = (_currentSequence[_sequenceCount.rr].m.motor2  - 30 + calibration.m.rr.motor2 + _sideUpRight);
  return _motorValue;
}

// update servo motors values
unsigned char updatePointPatterns(accRoll gyroState, unsigned char deviceMode) {
  // update sequence shift 
  if (_sequenceCount.fl < 19) {
    _sequenceCount.fl ++;
    _sequenceCount.rr = _sequenceCount.fl;
    if (_leftRightShift == 20) {
      // second pair of legs not moving
      // used for stand to walk and walk to stand transition
      _sequenceCount.fr = 0;
      _sequenceCount.rl = 0;
    } else if (_sequenceCount.fl >= _leftRightShift) {
      _sequenceCount.fr = _sequenceCount.fl - _leftRightShift;
      _sequenceCount.rl = _sequenceCount.fl - _leftRightShift;
    } else {
      _sequenceCount.fr = _sequenceCount.fl + _leftRightShift;
      _sequenceCount.rl = _sequenceCount.fl + _leftRightShift;
    }
  } else {
    // last sequence in the pattern. Get next pattern
    _sequenceCount.fl = 0;
    _sequenceCount.rr = 0;
    if (_leftRightShift == 20) {
      _sequenceCount.fr = 0;
      _sequenceCount.rl = 0;
    } else {
      _sequenceCount.fr = _leftRightShift;
      _sequenceCount.rl = _leftRightShift;
    }
    // check explore mode
    if (deviceMode == EXPLORE) {
      // explore mode
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
  }
  return _sequenceCount.fl;
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
  } else if (center > 15) {
    center = 15;
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


