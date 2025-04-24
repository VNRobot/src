/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Robot legs steps
*/

// walking 93 points. center point is 48
//          shift    -110      -110      -110      -105      -100       -95       -90       -85       -80       -75       -70       -65       -60       -55       -50       -45       -40       -35       -30       -25       -20       -15       -10        -5         0         5        10        15        20        25        30        35        40        45        50        55        60        65        70        75        80        85        90        95       100       100       100
char m1WalkSt[93] = {  22,  22,  22,  22,  22,  22,  22,  22,  22,  22,  22,  22,  22,  22,  22,  22,  22,  22,  22,  23,  23,  23,  24,  24,  25,  25,  26,  26,  27,  27,  28,  29,  30,  30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,  48,  50,  51,  53,  54,  56,  57,  58,  59,  61,  62,  64,  66,  68,  69,  71,  72,  74,  75,  77,  79,  81,  82,  84,  86,  88,  90,  92,  94,  97,  99, 102, 105, 108, 112, 116, 121, 126, 126, 126, 126, 126};
char m2WalkSt[93] = { 118, 118, 118, 118, 118, 115, 112, 109, 107, 104, 102, 100,  98,  96,  94,  92,  91,  89,  87,  85,  84,  82,  81,  79,  78,  76,  75,  73,  72,  70,  69,  67,  66,  64,  63,  61,  60,  59,  58,  56,  55,  53,  52,  51,  50,  49,  48,  46,  45,  43,  42,  41,  40,  39,  38,  37,  36,  35,  34,  33,  33,  32,  31,  30,  29,  28,  27,  26,  26,  25,  25,  24,  24,  23,  23,  23,  23,  22,  22,  22,  22,  22,  22,  22,  22,  22,  22,  22,  22,  22,  22,  22,  22};
//  lifted  shift    -110      -110      -110      -105      -100       -95       -90       -85       -80       -75       -70       -65       -60       -55       -50       -45       -40       -35       -30       -25       -20       -15       -10        -5         0         5        10        15        20        25        30        35        40        45        50        55        60        65        70        75        80        85        90        95       100       100       100
char m1LiftSt[93] = {   5,   5,   5,   5,   5,   5,   5,   5,   5,   5,   5,   5,   5,   5,   5,   5,   6,   6,   6,   7,   7,   7,   8,   9,  10,  10,  11,  11,  12,  12,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  28,  29,  30,  31,  32,  33,  34,  36,  38,  39,  40,  42,  44,  45,  46,  48,  50,  51,  52,  54,  56,  58,  60,  62,  64,  66,  68,  69,  70,  72,  74,  76,  78,  80,  82,  84,  86,  88,  90,  92,  94,  96,  98, 100, 103, 103, 103, 103, 103};
char m2LiftSt[93] = { 105, 105, 105, 105, 105, 102, 100,  98,  96,  94,  92,  90,  88,  86,  84,  83,  82,  80,  78,  76,  75,  73,  72,  71,  70,  68,  66,  64,  63,  61,  60,  58,  57,  55,  54,  52,  51,  49,  48,  47,  45,  44,  43,  41,  40,  38,  37,  36,  34,  32,  31,  30,  29,  27,  26,  25,  24,  23,  22,  21,  20,  19,  18,  17,  16,  15,  14,  13,  12,  11,  10,  10,   9,   8,   8,   7,   7,   7,   6,   6,   6,   5,   5,   5,   5,   5,   5,   5,   5,   5,   5,   5,   5};
// lifted high shift -110      -110      -110      -105      -100       -95       -90       -85       -80       -75       -70       -65       -60       -55       -50       -45       -40       -35       -30       -25       -20       -15       -10        -5         0         5        10        15        20        25        30        35        40        45        50        55        60        65        70        75        80        85        90        95       100       100       100
char m1LiftH[93]  = { -12, -12, -12, -12, -12, -12, -12, -12, -12, -12, -12, -12, -12, -12, -12, -12, -11, -11, -11, -11, -10, -10,  -9,  -8,  -7,  -7,  -6,  -6,  -5,  -4,  -3,  -2,   0,   1,   2,   3,   4,   5,   6,   7,   8,  10,  12,  13,  14,  15,  17,  18,  20,  21,  23,  24,  26,  28,  30,  31,  33,  34,  36,  38,  40,  41,  43,  45,  47,  48,  50,  52,  54,  56,  58,  60,  62,  63,  65,  67,  69,  71,  73,  75,  77,  79,  81,  84,  86,  87,  89,  91,  94,  94,  94,  94,  94};
char m2LiftH[93]  = {  98,  98,  98,  98,  98,  97,  95,  94,  92,  90,  88,  87,  85,  84,  82,  80,  78,  77,  75,  74,  72,  70,  68,  67,  65,  64,  62,  60,  58,  57,  55,  53,  51,  50,  48,  46,  44,  43,  41,  40,  38,  36,  34,  33,  31,  30,  28,  26,  24,  23,  21,  20,  18,  17,  15,  14,  12,  11,   9,   8,   6,   5,   4,   3,   2,   1,  -1,  -2,  -3,  -4,  -5,  -5,  -6,  -6,  -7,  -7,  -8,  -9, -10, -10, -11, -11, -12, -12, -12, -12, -12, -12, -12, -12, -12, -12, -12};
// lift high flags. 2 - lift high, 1 - lift, 0 - walk
char mLiftHFlag[36] = { 2, 2,  2,  2,  1,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 2, 2};
// walking pointers
char mPointWalk[36] = { 0,-5,-10,-15,-14,-13,-12,-11,-10,-9,-8,-7,-6,-5,-4,-3,-2,-1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,10, 5};
// center of array
// m_forwardCenterServo = 48; // bigger the number more weight on front
//
char forwardBallance = 0;
// motors values for 2 motors
motors legStep = {0, 0};
// walking mode
bool walkingMode = false;
// speed
char speed = 0;
// array pointer buffer
unsigned char pointBuffer = 0;

// set forward ballance value
void setFowardBallanceSteps(char ballanceValue) {
  forwardBallance = ballanceValue;
}

// get servo motor walk values
motors getWalkStep(unsigned char counter) {
  pointBuffer = m_forwardCenterServo + forwardBallance + mPointWalk[counter] * speed;
  if (walkingMode) {
    if (mLiftHFlag[counter] == 0) {
      legStep.motor1 = m1WalkSt[pointBuffer];
      legStep.motor2 = m2WalkSt[pointBuffer];
    } else if ((mLiftHFlag[counter] == 2) && (speed == 2)) {
      legStep.motor1 = m1LiftH[pointBuffer];
      legStep.motor2 = m2LiftH[pointBuffer];
    } else {
      legStep.motor1 = m1LiftSt[pointBuffer];
      legStep.motor2 = m2LiftSt[pointBuffer];
    }
  } else {
    // do not apply lifting
    legStep.motor1 = m1WalkSt[pointBuffer];
    legStep.motor2 = m2WalkSt[pointBuffer];
  }
  return legStep;
}

// set mode and speed
void setSteps(unsigned char currentPattern) {
  // set mode
  switch (currentPattern) {
    case P_STANDGO:
    case P_STANDGOLEFT:
    case P_STANDGORIGHT:
    case P_STANDGOSHIFTLEFT:
    case P_STANDGOSHIFTRIGHT:
    {
      speed = 0;
      walkingMode = true;
    }
    break;
    case P_GOFORWARD:
    case P_GOLEFT:
    case P_GORIGHT:
    case P_GOSHIFTLEFT:
    case P_GOSHIFTRIGHT:
    {
      // increase speed
      if (speed < 2) {
        speed ++;
      }
      walkingMode = true;
    }
    break;
    case P_GOBACK:
    case P_GOBACKLEFT:
    case P_GOBACKRIGHT:
    {
      speed = -1;
      walkingMode = true;
    }
    break;
    default:
    {
      speed = 0;
      walkingMode = false;
    }
    break;
  }
}
