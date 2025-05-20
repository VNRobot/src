/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Robot legs steps
*/

// array size
char arraySize = 43;
// walking 43 points. center point is 22
//          shift      -110  -105  -100   -95   -90   -85   -80   -75   -70   -65   -60   -55   -50   -45   -40   -35   -30   -25   -20   -15   -10    -5     0     5    10    15    20    25    30    35    40    45    50    55    60    65    70    75    80    85    90    95   100
char m1WalkSt[43]   = {  22,   22,   22,   22,   22,   22,   22,   22,   23,   24,   25,   26,   27,   28,   30,   31,   33,   35,   37,   39,   41,   43,   45,   47,   50,   53,   56,   58,   61,   64,   68,   71,   74,   77,   81,   84,   88,   92,   97,  102,  108,  116,  126};
char m2WalkSt[43]   = { 118,  112,  107,  102,   98,   94,   91,   87,   84,   81,   78,   75,   72,   69,   66,   63,   60,   58,   55,   52,   50,   48,   45,   42,   40,   38,   36,   34,   33,   31,   29,   27,   26,   25,   24,   23,   23,   22,   22,   22,   22,   22,   22};
//  lifted  shift      -110  -105  -100   -95   -90   -85   -80   -75   -70   -65   -60   -55   -50   -45   -40   -35   -30   -25   -20   -15   -10    -5     0     5    10    15    20    25    30    35    40    45    50    55    60    65    70    75    80    85    90    95   100
char m1LiftSt[43]   = {   5,    5,    5,    5,    5,    5,    6,    6,    7,    8,   10,   11,   12,   13,   15,   17,   19,   21,   23,   25,   28,   30,   32,   34,   38,   40,   44,   46,   50,   52,   56,   60,   64,   68,   70,   74,   78,   82,   86,   90,   94,   98,  103};
char m2LiftSt[43]   = { 105,  100,   96,   92,   88,   84,   82,   78,   75,   72,   70,   66,   63,   60,   57,   54,   51,   48,   45,   43,   40,   37,   34,   31,   29,   26,   24,   22,   20,   18,   16,   14,   12,   10,    9,    8,    7,    6,    6,    5,    5,    5,    5};
// lifted high shift   -110  -105  -100   -95   -90   -85   -80   -75   -70   -65   -60   -55   -50   -45   -40   -35   -30   -25   -20   -15   -10    -5     0     5    10    15    20    25    30    35    40    45    50    55    60    65    70    75    80    85    90    95   100
char m1LiftH1[43]   = { -12,  -12,  -12,  -12,  -12,  -12,  -11,  -11,  -10,   -9,   -7,   -6,   -5,   -3,    0,    2,    4,    6,    8,   12,   14,   17,   20,   23,   26,   30,   33,   36,   40,   43,   47,   50,   54,   58,   62,   65,   69,   73,   77,   81,   86,   89,   94};
char m2LiftH1[43]   = {  98,   95,   92,   88,   85,   82,   78,   75,   72,   68,   65,   62,   58,   55,   51,   48,   44,   41,   38,   34,   31,   28,   24,   21,   18,   15,   12,    9,    6,    4,    2,   -1,   -3,   -5,   -6,   -7,   -8,  -10,  -11,  -12,  -12,  -12,  -12};
//  lift highest shift -110  -105  -100   -95   -90   -85   -80   -75   -70   -65   -60   -55   -50   -45   -40   -35   -30   -25   -20   -15   -10    -5     0     5    10    15    20    25    30    35    40    45    50    55    60    65    70    75    80    85    90    95   100
char m1LiftH2[43]   = { -30,  -30,  -30,  -30,  -30,  -30,  -29,  -29,  -28,  -27,  -26,  -25,  -24,  -22,  -19,  -17,  -14,  -11,   -9,   -5,   -1,    1,    6,   10,   14,   18,   23,   26,   31,   35,   38,   43,   48,   52,   56,   60,   64,   69,   73,   77,   81,   85,   89};
char m2LiftH2[43]   = {  97,   93,   90,   86,   83,   79,   76,   73,   70,   66,   62,   58,   55,   51,   47,   43,   38,   35,   31,   27,   23,   19,   15,   10,    7,    3,   -1,   -4,   -9,  -11,  -14,  -17,  -20,  -22,  -24,  -26,  -27,  -28,  -29,  -29,  -29,  -29,  -29};
// lift high flags. 2 - lift high, 1 - lift, 0 - walk
//char mLiftHFlag[36]   = { 3, 3,  3,  3,  2,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 3, 3};
char mLiftHFlag[36]     = { 3, 3,  3,  3,  2,  1,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 3, 3};
// walking pointers
char mPointWalk[36]     = { 0,-5,-10,-15,-14,-13,-12,-11,-10,-9,-8,-7,-6,-5,-4,-3,-2,-1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,10, 5};
char mPointWalkSlow[36] = { 0,-3, -6, -8, -7, -7, -6, -6, -5,-5,-4,-4,-3,-3,-2,-2,-1,-1, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 6, 3};
// center of array
// m_forwardCenterServo = 22; // bigger the number more weight on front
//
char forwardBallance = 0;
// walking mode
bool walkingMode = false;
// speed
char speed = 0;
char speedL = 0;
char speedR = 0;

// set forward ballance value
void setFowardBallanceSteps(char ballanceValue) {
  forwardBallance = ballanceValue;
}

// get walk point
char _getPoint(unsigned char counter, char speedValue) {
  char arrayPoint = m_forwardCenterServo + forwardBallance;
  switch (speedValue) {
    case -1:
    {
      arrayPoint = m_forwardCenterServo + forwardBallance - mPointWalkSlow[counter];
    }
    break;
    case 1:
    {
      arrayPoint = m_forwardCenterServo + forwardBallance + mPointWalkSlow[counter];
    }
    break;
    case 2:
    {
      arrayPoint = m_forwardCenterServo + forwardBallance + mPointWalk[counter];
    }
    break;
    default:
    break;
  }
  if (arrayPoint < 0) {
    arrayPoint = 0;
  } else if (arrayPoint >= arraySize) {
    arrayPoint = arraySize - 1;
  }
  return arrayPoint;
}

// set walk step value
motors _setWalkStep(char pointBuffer, unsigned char counter, char speedValue) {
  // motors values for 2 motors
  motors legStep = {0, 0};
  if (walkingMode) {
    switch (mLiftHFlag[counter]) {
      case 0:
      {
        legStep.motor1 = m1WalkSt[pointBuffer];
        legStep.motor2 = m2WalkSt[pointBuffer];
      }
      break;
      case 1:
      {
        legStep.motor1 = (m1LiftSt[pointBuffer] + m1WalkSt[pointBuffer]) / 2;
        legStep.motor2 = (m2LiftSt[pointBuffer] + m2WalkSt[pointBuffer]) / 2;
      }
      break;
      case 2:
      {
        legStep.motor1 = m1LiftSt[pointBuffer];
        legStep.motor2 = m2LiftSt[pointBuffer];
      }
      break;
      case 3:
      {
        if (speedValue == 2) {
          legStep.motor1 = m1LiftH2[pointBuffer];
          legStep.motor2 = m2LiftH2[pointBuffer];
        } else {
          legStep.motor1 = m1LiftH1[pointBuffer];
          legStep.motor2 = m2LiftH1[pointBuffer];
        }
      }
      break;
      default:
      break;
    }
  } else {
    // do not apply lifting
    legStep.motor1 = m1WalkSt[pointBuffer];
    legStep.motor2 = m2WalkSt[pointBuffer];
  }
  return legStep;
}

// get servo motor walk values
motors getWalkStep(unsigned char counter) {
  return _setWalkStep(_getPoint(counter, speed), counter, speed);
}
// get servo motor walk values left side
motors getWalkStepL(unsigned char counter) {
  return _setWalkStep(_getPoint(counter, speedL), counter, speedL);
}
// get servo motor walk values right side
motors getWalkStepR(unsigned char counter) {
  return _setWalkStep(_getPoint(counter, speedR), counter, speedR);
}

// set mode and speed
void setSteps(unsigned char currentPattern, char angleTurn) {
  // set mode
  switch (currentPattern) {
    case P_STANDGO:
    {
      speed = 0;
      speedL = speed;
      speedR = speed;
      if (angleTurn > 4) {
        speedL = 1;
        speedR = -1;
      } else if (angleTurn < -4) {
        speedL = -1;
        speedR = 1;
      }
      walkingMode = true;
    }
    break;
    case P_STANDGOLEFT:
    case P_STANDGOSHIFTLEFT:
    {
      speed = 0;
      speedL = -1;
      speedR = 1;
      walkingMode = true;
    }
    break;
    case P_STANDGORIGHT:
    case P_STANDGOSHIFTRIGHT:
    {
      speed = 0;
      speedL = 1;
      speedR = -1;
      walkingMode = true;
    }
    break;
    case P_GOFORWARD:
    {
      // increase speed
      if (speed < 2) {
        speed ++;
      }
      speedL = speed;
      speedR = speed;
      if (angleTurn > 4) {
        speedR = speedL - 1;
      } else if (angleTurn < -4) {
        speedL = speedR - 1;
      }
      walkingMode = true;
    }
    break;
    case P_GOLEFT:
    case P_GOSHIFTLEFT:
    {
      // increase speed
      if (speed < 2) {
        speed ++;
      }
      speedL = speed - 1;
      speedR = speed;
      walkingMode = true;
    }
    break;
    case P_GORIGHT:
    case P_GOSHIFTRIGHT:
    {
      // increase speed
      if (speed < 2) {
        speed ++;
      }
      speedL = speed;
      speedR = speed - 1;
      walkingMode = true;
    }
    break;
    case P_GOBACK:
    {
      speed = -1;
      speedL = speed;
      speedR = speed;
      walkingMode = true;
    }
    break;
    case P_GOBACKLEFT:
    {
      speed = -1;
      speedL = -1;
      speedR = 0;
      walkingMode = true;
    }
    break;
    case P_GOBACKRIGHT:
    {
      speed = -1;
      speedL = 0;
      speedR = -1;
      walkingMode = true;
    }
    break;
    default:
    {
      speed = 0;
      speedL = speed;
      speedR = speed;
      walkingMode = false;
    }
    break;
  }
}
