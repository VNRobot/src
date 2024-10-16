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
char motor1Walk[24] =  { 46,  47,  48,  50,  52,  54,  56,  58,  60,  62,  64, 66, 69, 72, 75, 78, 81, 84, 87, 90, 94, 98,102,106};
char motor2Walk[24] =  { 96,  90,  85,  81,  77,  73,  69,  65,  61,  57,  54, 51, 48, 45, 42, 40, 38, 36, 34, 33, 32, 31, 30, 29};
// center position in the pattern array
int _centerAbsolute = 13; // (range 8 to 15) bigger the number more weight on front
// static forward ballance
int _centerStatic = _centerAbsolute;
// char buffer for temporary use
char cBuffer;
// leg lift angles
unsigned char _liftm1 = 8;
unsigned char _liftm2 = 24;
// center motor default position
char _centerF = 0; // positive more distance between legs
char _centerR = 0; // positive more distance between legs
// points to currentSequence for every leg
legs sequenceCount = { 0, 0, 0, 0};
// dynamic side ballance
char _sideBallance = 0;
char _sideUpLeft = 0;
char _sideUpRight = 0;
// 20 positions per sequence
// all legs using the same sequence with different pozition shift
sequence currentSequence[20] = {
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
// motors values for 10 motors
allMotors motorValue = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
// interruption flag
bool iterrupted = false;

// set new values of center motors
void _updateCenterMotor(char cAngleF, char cAngleR) {
  // front motor
  currentSequence[0].front = _centerF - cAngleF/2;
  currentSequence[1].front = _centerF - cAngleF;
  currentSequence[2].front = _centerF - cAngleF;
  currentSequence[3].front = _centerF - cAngleF;
  currentSequence[4].front = _centerF - cAngleF;
  currentSequence[5].front = _centerF - cAngleF;
  currentSequence[6].front = _centerF - cAngleF;
  currentSequence[7].front = _centerF - cAngleF;
  currentSequence[8].front = _centerF - cAngleF;
  currentSequence[9].front = _centerF - cAngleF/2;
  currentSequence[10].front = _centerF + cAngleF/2;
  currentSequence[11].front = _centerF + cAngleF;
  currentSequence[12].front = _centerF + cAngleF;
  currentSequence[13].front = _centerF + cAngleF;
  currentSequence[14].front = _centerF + cAngleF;
  currentSequence[15].front = _centerF + cAngleF;
  currentSequence[16].front = _centerF + cAngleF;
  currentSequence[17].front = _centerF + cAngleF;
  currentSequence[18].front = _centerF + cAngleF;
  currentSequence[19].front = _centerF + cAngleF/2;
  // rear motor
  currentSequence[0].rear = _centerR - cAngleR/2;
  currentSequence[1].rear = _centerR - cAngleR;
  currentSequence[2].rear = _centerR - cAngleR;
  currentSequence[3].rear = _centerR - cAngleR;
  currentSequence[4].rear = _centerR - cAngleR;
  currentSequence[5].rear = _centerR - cAngleR;
  currentSequence[6].rear = _centerR - cAngleR;
  currentSequence[7].rear = _centerR - cAngleR;
  currentSequence[8].rear = _centerR - cAngleR;
  currentSequence[9].rear = _centerR - cAngleR/2;
  currentSequence[10].rear = _centerR + cAngleR/2;
  currentSequence[11].rear = _centerR + cAngleR;
  currentSequence[12].rear = _centerR + cAngleR;
  currentSequence[13].rear = _centerR + cAngleR;
  currentSequence[14].rear = _centerR + cAngleR;
  currentSequence[15].rear = _centerR + cAngleR;
  currentSequence[16].rear = _centerR + cAngleR;
  currentSequence[17].rear = _centerR + cAngleR;
  currentSequence[18].rear = _centerR + cAngleR;
  currentSequence[19].rear = _centerR + cAngleR/2;
}

// set default values of center motors
// use 0 for now
void _defaultCenterMotor(void) {
  // front motor
  currentSequence[0].front = _centerF;
  currentSequence[1].front = _centerF;
  currentSequence[2].front = _centerF;
  currentSequence[3].front = _centerF;
  currentSequence[4].front = _centerF;
  currentSequence[5].front = _centerF;
  currentSequence[6].front = _centerF;
  currentSequence[7].front = _centerF;
  currentSequence[8].front = _centerF;
  currentSequence[9].front = _centerF;
  currentSequence[10].front = _centerF;
  currentSequence[11].front = _centerF;
  currentSequence[12].front = _centerF;
  currentSequence[13].front = _centerF;
  currentSequence[14].front = _centerF;
  currentSequence[15].front = _centerF;
  currentSequence[16].front = _centerF;
  currentSequence[17].front = _centerF;
  currentSequence[18].front = _centerF;
  currentSequence[19].front = _centerF;
  // rear motor
  currentSequence[0].rear = _centerR;
  currentSequence[1].rear = _centerR;
  currentSequence[2].rear = _centerR;
  currentSequence[3].rear = _centerR;
  currentSequence[4].rear = _centerR;
  currentSequence[5].rear = _centerR;
  currentSequence[6].rear = _centerR;
  currentSequence[7].rear = _centerR;
  currentSequence[8].rear = _centerR;
  currentSequence[9].rear = _centerR;
  currentSequence[10].rear = _centerR;
  currentSequence[11].rear = _centerR;
  currentSequence[12].rear = _centerR;
  currentSequence[13].rear = _centerR;
  currentSequence[14].rear = _centerR;
  currentSequence[15].rear = _centerR;
  currentSequence[16].rear = _centerR;
  currentSequence[17].rear = _centerR;
  currentSequence[18].rear = _centerR;
  currentSequence[19].rear = _centerR;
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
  cBuffer = currentSequence[19].m.motor1;
  currentSequence[0].m.motor1 = cBuffer;
  currentSequence[1].m.motor1 = cBuffer + (endPosition1 - cBuffer) / 14;
  currentSequence[2].m.motor1 = cBuffer + ((endPosition1 - cBuffer) * 2) / 18;
  currentSequence[3].m.motor1 = cBuffer + ((endPosition1 - cBuffer) * 3) / 18;
  currentSequence[4].m.motor1 = cBuffer + ((endPosition1 - cBuffer) * 4) / 18;
  currentSequence[5].m.motor1 = cBuffer + ((endPosition1 - cBuffer) * 5) / 18;
  currentSequence[6].m.motor1 = cBuffer + ((endPosition1 - cBuffer) * 6) / 18;
  currentSequence[7].m.motor1 = cBuffer + ((endPosition1 - cBuffer) * 7) / 18;
  currentSequence[8].m.motor1 = cBuffer + ((endPosition1 - cBuffer) * 8) / 18;
  currentSequence[9].m.motor1 = cBuffer + ((endPosition1 - cBuffer) * 9) / 18;
  currentSequence[10].m.motor1 = cBuffer + ((endPosition1 - cBuffer) * 10) / 18;
  currentSequence[11].m.motor1 = cBuffer + ((endPosition1 - cBuffer) * 11) / 18;
  currentSequence[12].m.motor1 = cBuffer + ((endPosition1 - cBuffer) * 12) / 18;
  currentSequence[13].m.motor1 = cBuffer + ((endPosition1 - cBuffer) * 13) / 18;
  currentSequence[14].m.motor1 = cBuffer + ((endPosition1 - cBuffer) * 14) / 18;
  currentSequence[15].m.motor1 = cBuffer + ((endPosition1 - cBuffer) * 15) / 18;
  currentSequence[16].m.motor1 = cBuffer + ((endPosition1 - cBuffer) * 16) / 18;
  currentSequence[17].m.motor1 = cBuffer + ((endPosition1 - cBuffer) * 17) / 18;
  currentSequence[18].m.motor1 = endPosition1;
  currentSequence[19].m.motor1 = endPosition1;
  // motor 2
  cBuffer = currentSequence[19].m.motor2;
  currentSequence[0].m.motor2 = cBuffer;
  currentSequence[1].m.motor2 = cBuffer + (endPosition2 - cBuffer) / 14;
  currentSequence[2].m.motor2 = cBuffer + ((endPosition2 - cBuffer) * 2) / 18;
  currentSequence[3].m.motor2 = cBuffer + ((endPosition2 - cBuffer) * 3) / 18;
  currentSequence[4].m.motor2 = cBuffer + ((endPosition2 - cBuffer) * 4) / 18;
  currentSequence[5].m.motor2 = cBuffer + ((endPosition2 - cBuffer) * 5) / 18;
  currentSequence[6].m.motor2 = cBuffer + ((endPosition2 - cBuffer) * 6) / 18;
  currentSequence[7].m.motor2 = cBuffer + ((endPosition2 - cBuffer) * 7) / 18;
  currentSequence[8].m.motor2 = cBuffer + ((endPosition2 - cBuffer) * 8) / 18;
  currentSequence[9].m.motor2 = cBuffer + ((endPosition2 - cBuffer) * 9) / 18;
  currentSequence[10].m.motor2 = cBuffer + ((endPosition2 - cBuffer) * 10) / 18;
  currentSequence[11].m.motor2 = cBuffer + ((endPosition2 - cBuffer) * 11) / 18;
  currentSequence[12].m.motor2 = cBuffer + ((endPosition2 - cBuffer) * 12) / 18;
  currentSequence[13].m.motor2 = cBuffer + ((endPosition2 - cBuffer) * 13) / 18;
  currentSequence[14].m.motor2 = cBuffer + ((endPosition2 - cBuffer) * 14) / 18;
  currentSequence[15].m.motor2 = cBuffer + ((endPosition2 - cBuffer) * 15) / 18;
  currentSequence[16].m.motor2 = cBuffer + ((endPosition2 - cBuffer) * 16) / 18;
  currentSequence[17].m.motor2 = cBuffer + ((endPosition2 - cBuffer) * 17) / 18;
  currentSequence[18].m.motor2 = endPosition2;
  currentSequence[19].m.motor2 = endPosition2;
}

// linear motor movement from point to point at the end of sequence
void _updateSequenceLinearEnd(char endPosition1, char endPosition2) {
  // motor 1
  cBuffer = currentSequence[19].m.motor1;
  currentSequence[0].m.motor1 = cBuffer;
  currentSequence[1].m.motor1 = cBuffer;
  currentSequence[2].m.motor1 = cBuffer;
  currentSequence[3].m.motor1 = cBuffer;
  currentSequence[4].m.motor1 = cBuffer;
  currentSequence[5].m.motor1 = cBuffer;
  currentSequence[6].m.motor1 = cBuffer;
  currentSequence[7].m.motor1 = cBuffer;
  currentSequence[8].m.motor1 = cBuffer;
  currentSequence[9].m.motor1 = cBuffer;
  currentSequence[10].m.motor1 = cBuffer;
  currentSequence[11].m.motor1 = cBuffer;
  currentSequence[12].m.motor1 = cBuffer;
  currentSequence[13].m.motor1 = cBuffer;
  currentSequence[14].m.motor1 = cBuffer;
  currentSequence[15].m.motor1 = cBuffer;
  currentSequence[16].m.motor1 = cBuffer + (endPosition1 - cBuffer) / 4;
  currentSequence[17].m.motor1 = cBuffer + ((endPosition1 - cBuffer) * 2) / 4;
  currentSequence[18].m.motor1 = cBuffer + ((endPosition1 - cBuffer) * 3) / 4;
  currentSequence[19].m.motor1 = endPosition1;
  // motor 2
  cBuffer = currentSequence[19].m.motor2;
  currentSequence[0].m.motor2 = cBuffer;
  currentSequence[1].m.motor2 = cBuffer;
  currentSequence[2].m.motor2 = cBuffer;
  currentSequence[3].m.motor2 = cBuffer;
  currentSequence[4].m.motor2 = cBuffer;
  currentSequence[5].m.motor2 = cBuffer;
  currentSequence[6].m.motor2 = cBuffer;
  currentSequence[7].m.motor2 = cBuffer;
  currentSequence[8].m.motor2 = cBuffer;
  currentSequence[9].m.motor2 = cBuffer;
  currentSequence[10].m.motor2 = cBuffer;
  currentSequence[11].m.motor2 = cBuffer;
  currentSequence[12].m.motor2 = cBuffer;
  currentSequence[13].m.motor2 = cBuffer;
  currentSequence[14].m.motor2 = cBuffer;
  currentSequence[15].m.motor2 = cBuffer;
  currentSequence[16].m.motor2 = cBuffer + (endPosition2 - cBuffer) / 4;
  currentSequence[17].m.motor2 = cBuffer + ((endPosition2 - cBuffer) * 2) / 4;
  currentSequence[18].m.motor2 = cBuffer + ((endPosition2 - cBuffer) * 3) / 4;
  currentSequence[19].m.motor2 = endPosition2;
}

// linear motor movement from point to point at the start of sequence
void _updateSequenceLinearStart(char endPosition1, char endPosition2) {
  // motor 1
  cBuffer = currentSequence[19].m.motor1;
  currentSequence[0].m.motor1 = cBuffer;
  currentSequence[1].m.motor1 = cBuffer + (endPosition1 - cBuffer) / 4;
  currentSequence[2].m.motor1 = cBuffer + ((endPosition1 - cBuffer) * 2) / 4;
  currentSequence[3].m.motor1 = cBuffer + ((endPosition1 - cBuffer) * 3) / 4;
  currentSequence[4].m.motor1 = endPosition1;
  currentSequence[5].m.motor1 = endPosition1;
  currentSequence[6].m.motor1 = endPosition1;
  currentSequence[7].m.motor1 = endPosition1;
  currentSequence[8].m.motor1 = endPosition1;
  currentSequence[9].m.motor1 = endPosition1;
  currentSequence[10].m.motor1 = endPosition1;
  currentSequence[11].m.motor1 = endPosition1;
  currentSequence[12].m.motor1 = endPosition1;
  currentSequence[13].m.motor1 = endPosition1;
  currentSequence[14].m.motor1 = endPosition1;
  currentSequence[15].m.motor1 = endPosition1;
  currentSequence[16].m.motor1 = endPosition1;
  currentSequence[17].m.motor1 = endPosition1;
  currentSequence[18].m.motor1 = endPosition1;
  currentSequence[19].m.motor1 = endPosition1;
  // motor 2
  cBuffer = currentSequence[19].m.motor2;
  currentSequence[0].m.motor2 = cBuffer;
  currentSequence[1].m.motor2 = cBuffer + (endPosition2 - cBuffer) / 4;
  currentSequence[2].m.motor2 = cBuffer + ((endPosition2 - cBuffer) * 2) / 4;
  currentSequence[3].m.motor2 = cBuffer + ((endPosition2 - cBuffer) * 3) / 4;
  currentSequence[4].m.motor2 = endPosition2;
  currentSequence[5].m.motor2 = endPosition2;
  currentSequence[6].m.motor2 = endPosition2;
  currentSequence[7].m.motor2 = endPosition2;
  currentSequence[8].m.motor2 = endPosition2;
  currentSequence[9].m.motor2 = endPosition2;
  currentSequence[10].m.motor2 = endPosition2;
  currentSequence[11].m.motor2 = endPosition2;
  currentSequence[12].m.motor2 = endPosition2;
  currentSequence[13].m.motor2 = endPosition2;
  currentSequence[14].m.motor2 = endPosition2;
  currentSequence[15].m.motor2 = endPosition2;
  currentSequence[16].m.motor2 = endPosition2;
  currentSequence[17].m.motor2 = endPosition2;
  currentSequence[18].m.motor2 = endPosition2;
  currentSequence[19].m.motor2 = endPosition2;
}

// create stand walk sequence
void _updateSequenceStandWalk(char m1, char m2) {
  // motor 1
  currentSequence[0].m.motor1 = m1 - _liftm1;
  currentSequence[1].m.motor1 = m1 - _liftm1;
  currentSequence[2].m.motor1 = m1 - _liftm1 / 2;
  currentSequence[3].m.motor1 = m1 - _liftm1 / 4;
  currentSequence[4].m.motor1 = m1;
  currentSequence[5].m.motor1 = m1;
  currentSequence[6].m.motor1 = m1;
  currentSequence[7].m.motor1 = m1;
  currentSequence[8].m.motor1 = m1;
  currentSequence[9].m.motor1 = m1;
  currentSequence[10].m.motor1 = m1;
  currentSequence[11].m.motor1 = m1;
  currentSequence[12].m.motor1 = m1;
  currentSequence[13].m.motor1 = m1;
  currentSequence[14].m.motor1 = m1;
  currentSequence[15].m.motor1 = m1;
  currentSequence[16].m.motor1 = m1;
  currentSequence[17].m.motor1 = m1;
  currentSequence[18].m.motor1 = m1;
  currentSequence[19].m.motor1 = m1 - _liftm1 / 2;
  // motor 2
  currentSequence[0].m.motor2 = m2 - _liftm2;
  currentSequence[1].m.motor2 = m2 - _liftm2 / 2;
  currentSequence[2].m.motor2 = m2 - _liftm2 / 4;
  currentSequence[3].m.motor2 = m2;
  currentSequence[4].m.motor2 = m2;
  currentSequence[5].m.motor2 = m2;
  currentSequence[6].m.motor2 = m2;
  currentSequence[7].m.motor2 = m2;
  currentSequence[8].m.motor2 = m2;
  currentSequence[9].m.motor2 = m2;
  currentSequence[10].m.motor2 = m2;
  currentSequence[11].m.motor2 = m2;
  currentSequence[12].m.motor2 = m2;
  currentSequence[13].m.motor2 = m2;
  currentSequence[14].m.motor2 = m2;
  currentSequence[15].m.motor2 = m2;
  currentSequence[16].m.motor2 = m2;
  currentSequence[17].m.motor2 = m2;
  currentSequence[18].m.motor2 = m2 - _liftm2 / 2;
  currentSequence[19].m.motor2 = m2 - _liftm2;
}

// create walk sequence
void _updateSequenceWalk(char m1[], char m2[]) {
  // motor 1
  currentSequence[0].m.motor1 = m1[_centerStatic] - _liftm1;
  currentSequence[1].m.motor1 = m1[_centerStatic - 5] - _liftm1;
  currentSequence[2].m.motor1 = m1[_centerStatic - 8] - _liftm1;
  currentSequence[3].m.motor1 = m1[_centerStatic - 7] - _liftm1;
  currentSequence[4].m.motor1 = m1[_centerStatic - 6];
  currentSequence[5].m.motor1 = m1[_centerStatic - 5];
  currentSequence[6].m.motor1 = m1[_centerStatic - 4];
  currentSequence[7].m.motor1 = m1[_centerStatic - 3];
  currentSequence[8].m.motor1 = m1[_centerStatic - 2];
  currentSequence[9].m.motor1 = m1[_centerStatic - 1];
  currentSequence[10].m.motor1 = m1[_centerStatic];
  currentSequence[11].m.motor1 = m1[_centerStatic + 1];
  currentSequence[12].m.motor1 = m1[_centerStatic + 2];
  currentSequence[13].m.motor1 = m1[_centerStatic + 3];
  currentSequence[14].m.motor1 = m1[_centerStatic + 4];
  currentSequence[15].m.motor1 = m1[_centerStatic + 5];
  currentSequence[16].m.motor1 = m1[_centerStatic + 6];
  currentSequence[17].m.motor1 = m1[_centerStatic + 7];
  currentSequence[18].m.motor1 = m1[_centerStatic + 8];
  currentSequence[19].m.motor1 = m1[_centerStatic + 5] - _liftm1;
  // motor 2
  currentSequence[0].m.motor2 = m2[_centerStatic] - _liftm2;
  currentSequence[1].m.motor2 = m2[_centerStatic - 5] - _liftm2;
  currentSequence[2].m.motor2 = m2[_centerStatic - 8] - _liftm2;
  currentSequence[3].m.motor2 = m2[_centerStatic - 7];
  currentSequence[4].m.motor2 = m2[_centerStatic - 6];
  currentSequence[5].m.motor2 = m2[_centerStatic - 5];
  currentSequence[6].m.motor2 = m2[_centerStatic - 4];
  currentSequence[7].m.motor2 = m2[_centerStatic - 3];
  currentSequence[8].m.motor2 = m2[_centerStatic - 2];
  currentSequence[9].m.motor2 = m2[_centerStatic - 1];
  currentSequence[10].m.motor2 = m2[_centerStatic];
  currentSequence[11].m.motor2 = m2[_centerStatic + 1];
  currentSequence[12].m.motor2 = m2[_centerStatic + 2];
  currentSequence[13].m.motor2 = m2[_centerStatic + 3];
  currentSequence[14].m.motor2 = m2[_centerStatic + 4];
  currentSequence[15].m.motor2 = m2[_centerStatic + 5];
  currentSequence[16].m.motor2 = m2[_centerStatic + 6];
  currentSequence[17].m.motor2 = m2[_centerStatic + 7];
  currentSequence[18].m.motor2 = m2[_centerStatic + 8] - _liftm2;
  currentSequence[19].m.motor2 = m2[_centerStatic + 5] - _liftm2;
}

// create walk back sequence
void _updateSequenceWalkBack(char m1[], char m2[]) {
  // motor 1
  currentSequence[0].m.motor1 = m1[_centerStatic] - _liftm1;
  currentSequence[19].m.motor1 = m1[_centerStatic - 5] - _liftm1;
  currentSequence[18].m.motor1 = m1[_centerStatic - 8];
  currentSequence[17].m.motor1 = m1[_centerStatic - 7];
  currentSequence[16].m.motor1 = m1[_centerStatic - 6];
  currentSequence[15].m.motor1 = m1[_centerStatic - 5];
  currentSequence[14].m.motor1 = m1[_centerStatic - 4];
  currentSequence[13].m.motor1 = m1[_centerStatic - 3];
  currentSequence[12].m.motor1 = m1[_centerStatic - 2];
  currentSequence[11].m.motor1 = m1[_centerStatic - 1];
  currentSequence[10].m.motor1 = m1[_centerStatic];
  currentSequence[9].m.motor1 = m1[_centerStatic + 1];
  currentSequence[8].m.motor1 = m1[_centerStatic + 2];
  currentSequence[7].m.motor1 = m1[_centerStatic + 3];
  currentSequence[6].m.motor1 = m1[_centerStatic + 4];
  currentSequence[5].m.motor1 = m1[_centerStatic + 5];
  currentSequence[4].m.motor1 = m1[_centerStatic + 6];
  currentSequence[3].m.motor1 = m1[_centerStatic + 7] - _liftm1;
  currentSequence[2].m.motor1 = m1[_centerStatic + 8] - _liftm1;
  currentSequence[1].m.motor1 = m1[_centerStatic + 5] - _liftm1;
  // motor 2
  currentSequence[0].m.motor2 = m2[_centerStatic] - _liftm2;
  currentSequence[19].m.motor2 = m2[_centerStatic - 5] - _liftm2;
  currentSequence[18].m.motor2 = m2[_centerStatic - 8] - _liftm2;
  currentSequence[17].m.motor2 = m2[_centerStatic - 7];
  currentSequence[16].m.motor2 = m2[_centerStatic - 6];
  currentSequence[15].m.motor2 = m2[_centerStatic - 5];
  currentSequence[14].m.motor2 = m2[_centerStatic - 4];
  currentSequence[13].m.motor2 = m2[_centerStatic - 3];
  currentSequence[12].m.motor2 = m2[_centerStatic - 2];
  currentSequence[11].m.motor2 = m2[_centerStatic - 1];
  currentSequence[10].m.motor2 = m2[_centerStatic];
  currentSequence[9].m.motor2 = m2[_centerStatic + 1];
  currentSequence[8].m.motor2 = m2[_centerStatic + 2];
  currentSequence[7].m.motor2 = m2[_centerStatic + 3];
  currentSequence[6].m.motor2 = m2[_centerStatic + 4];
  currentSequence[5].m.motor2 = m2[_centerStatic + 5];
  currentSequence[4].m.motor2 = m2[_centerStatic + 6];
  currentSequence[3].m.motor2 = m2[_centerStatic + 7];
  currentSequence[2].m.motor2 = m2[_centerStatic + 8] - _liftm2;
  currentSequence[1].m.motor2 = m2[_centerStatic + 5] - _liftm2;
} 

// get next sequence
void setPattern(unsigned char currentTaskItem) {
  // reset interruption
  iterrupted = false;
  // get new sequence array
  switch (currentTaskItem) {
    case P_DOSTAND:
    {
      _updateSequenceLinear(motor1Walk[_centerStatic], motor2Walk[_centerStatic]);
      _updateTurn(0);
    }
    break;
    case P_STANDTOGO:
    {
      _updateSequenceLinearEnd(motor1Walk[_centerStatic] - _liftm1, motor2Walk[_centerStatic] - _liftm2);
      _updateTurn(0);
    }
    break;
    case P_GOTOSTAND:
    {
      _updateSequenceLinearStart(motor1Walk[_centerStatic], motor2Walk[_centerStatic]);
      _updateTurn(0);
    }
    break;
    case P_STANDGO:
    {
      _updateSequenceStandWalk(motor1Walk[_centerStatic], motor2Walk[_centerStatic]);
    }
    break;
    case P_STANDGOLEFT:
    {
      _updateSequenceStandWalk(motor1Walk[_centerStatic], motor2Walk[_centerStatic]);
      _updateTurn(-10);
    }
    break;
    case P_STANDGORIGHT:
    {
      _updateSequenceStandWalk(motor1Walk[_centerStatic], motor2Walk[_centerStatic]);
      _updateTurn(10);
    }
    break;
    case P_STANDGOSHIFTLEFT:
    {
      _updateSequenceStandWalk(motor1Walk[_centerStatic], motor2Walk[_centerStatic]);
      _updateShift(-10);
    }
    break;
    case P_STANDGOSHIFTRIGHT:
    {
      _updateSequenceStandWalk(motor1Walk[_centerStatic], motor2Walk[_centerStatic]);
      _updateShift(10);
    }
    break;
    case P_GOFORWARD:
    {
      _updateSequenceWalk(motor1Walk, motor2Walk);
    }
    break;
    case P_GOLEFT:
    {
      _updateSequenceWalk(motor1Walk, motor2Walk);
      _updateTurn(-10);
    }
    break;
    case P_GORIGHT:
    {
      _updateSequenceWalk(motor1Walk, motor2Walk);
      _updateTurn(10);
    }
    break;
    case P_GOSHIFTLEFT:
    {
      _updateSequenceWalk(motor1Walk, motor2Walk);
      _updateShift(-10);
    }
    break;
    case P_GOSHIFTRIGHT:
    {
      _updateSequenceWalk(motor1Walk, motor2Walk);
      _updateShift(10);
    }
    break;
    case P_GOBACK:
    {
      _updateSequenceWalkBack(motor1Walk, motor2Walk);
      _updateTurn(0);
    }
    break;
    case P_GOBACKLEFT:
    {
      _updateSequenceWalkBack(motor1Walk, motor2Walk);
      _updateTurn(-10);
    }
    break;
    case P_GOBACKRIGHT:
    {
      _updateSequenceWalkBack(motor1Walk, motor2Walk);
      _updateTurn(10);
    }
    break;
    case P_DODOWN:
    {
      _updateSequenceLinear(30, 30);
      _updateTurn(0);
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
  /*
  if (iterrupted) {
    // TODO
    motorValue.front       = (currentSequence[sequenceCount.fl].front          + calibration.front);
    motorValue.rear        = (currentSequence[sequenceCount.fl].rear           + calibration.rear);
    motorValue.m.fl.motor1 = (currentSequence[sequenceCount.fl].m.motor1  - 30 + calibration.m.fl.motor1 - _sideUpLeft / 3);
    motorValue.m.fl.motor2 = (currentSequence[sequenceCount.fl].m.motor2  - 30 + calibration.m.fl.motor2 - _sideUpLeft);
    motorValue.m.fr.motor1 = (currentSequence[sequenceCount.fr].m.motor1  - 30 + calibration.m.fr.motor1 + _sideUpRight / 3);
    motorValue.m.fr.motor2 = (currentSequence[sequenceCount.fr].m.motor2  - 30 + calibration.m.fr.motor2 + _sideUpRight);
    motorValue.m.rl.motor1 = (currentSequence[sequenceCount.rl].m.motor1  - 30 + calibration.m.rl.motor1 - _sideUpLeft / 3);
    motorValue.m.rl.motor2 = (currentSequence[sequenceCount.rl].m.motor2  - 30 + calibration.m.rl.motor2 - _sideUpLeft);
    motorValue.m.rr.motor1 = (currentSequence[sequenceCount.rr].m.motor1  - 30 + calibration.m.rr.motor1 + _sideUpRight / 3);
    motorValue.m.rr.motor2 = (currentSequence[sequenceCount.rr].m.motor2  - 30 + calibration.m.rr.motor2 + _sideUpRight);
  } else {
  */
  // set motors angle values
  motorValue.front       = (currentSequence[sequenceCount.fl].front          + calibration.front);
  motorValue.rear        = (currentSequence[sequenceCount.fl].rear           + calibration.rear);
  motorValue.m.fl.motor1 = (currentSequence[sequenceCount.fl].m.motor1  - 30 + calibration.m.fl.motor1 - _sideUpLeft / 3);
  motorValue.m.fl.motor2 = (currentSequence[sequenceCount.fl].m.motor2  - 30 + calibration.m.fl.motor2 - _sideUpLeft);
  motorValue.m.fr.motor1 = (currentSequence[sequenceCount.fr].m.motor1  - 30 + calibration.m.fr.motor1 + _sideUpRight / 3);
  motorValue.m.fr.motor2 = (currentSequence[sequenceCount.fr].m.motor2  - 30 + calibration.m.fr.motor2 + _sideUpRight);
  motorValue.m.rl.motor1 = (currentSequence[sequenceCount.rl].m.motor1  - 30 + calibration.m.rl.motor1 - _sideUpLeft / 3);
  motorValue.m.rl.motor2 = (currentSequence[sequenceCount.rl].m.motor2  - 30 + calibration.m.rl.motor2 - _sideUpLeft);
  motorValue.m.rr.motor1 = (currentSequence[sequenceCount.rr].m.motor1  - 30 + calibration.m.rr.motor1 + _sideUpRight / 3);
  motorValue.m.rr.motor2 = (currentSequence[sequenceCount.rr].m.motor2  - 30 + calibration.m.rr.motor2 + _sideUpRight);
  // }
  return motorValue;
}

// update servo motors values
unsigned char updateCountPatterns(unsigned char currentTaskItem) {
  // update sequence shift 
  sequenceCount.fl ++;
  if (sequenceCount.fl > 19) {
    sequenceCount.fl = 0;
  }
    sequenceCount.rr = sequenceCount.fl;
    // normal leg pattern shift
    if (sequenceCount.fl >= 10) {
      sequenceCount.fr = sequenceCount.fl - 10;
    } else {
      sequenceCount.fr = sequenceCount.fl + 10;
    }
    // special cases
    switch (currentTaskItem) {
      case P_DOSTAND:
      case P_DODOWN:
      {
        sequenceCount.fr = sequenceCount.fl;
      }
      break;
      case P_STANDTOGO:
      {
        // fr rl pair not moving
        sequenceCount.fr = 0;
      }
      break;
      case P_GOTOSTAND:
      {
        // fr rl pair not moving
        sequenceCount.fr = 19;
      }
      break;
      default:
      break;
    }
    sequenceCount.rl = sequenceCount.fr;
  return sequenceCount.fl;
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

// set interruped flag
void setInterruptedPattern(void) {
  iterrupted = true;
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
    default:
      Serial.println(" unknown pattern ");
    break;
  }
}
*/