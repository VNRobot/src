/*
Ladybug Robot
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Robot legs motion patterns
*/

// servos speed
char motorLWalk[5] =  {-25, -15, 0, 15, 25};
char motorRWalk[5] =  {-25, -15, 0, 15, 25};
// static motor point
int _centerStatic = 2;
// char buffer for temporary use
char cBuffer1;
char cBuffer2;
// points to currentSequence for every leg
unsigned char mainCounter = 0;
// 20 positions per sequence
motors sequenceWing[20] = {50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50};
motors sequenceWheel[20] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
// motors values for 4 motors
allMotors motorValue = {0, 0, 0, 0};

// linear motor movement from point to point at the start of sequence
void _updateSequencePart1(char endPosition1, char endPosition2, motors * sequence) {
  cBuffer1 = sequence[19].motorL;
  cBuffer2 = sequence[19].motorR;
  sequence[0].motorL = cBuffer1;
  sequence[1].motorL = cBuffer1 + (endPosition1 - cBuffer1) / 4;
  sequence[2].motorL = cBuffer1 + ((endPosition1 - cBuffer1) * 2) / 4;
  sequence[3].motorL = cBuffer1 + ((endPosition1 - cBuffer1) * 3) / 4;
  sequence[4].motorL = endPosition1;
  sequence[0].motorR = cBuffer2;
  sequence[1].motorR = cBuffer2 + (endPosition2 - cBuffer2) / 4;
  sequence[2].motorR = cBuffer2 + ((endPosition2 - cBuffer2) * 2) / 4;
  sequence[3].motorR = cBuffer2 + ((endPosition2 - cBuffer2) * 3) / 4;
  sequence[4].motorR = endPosition2;
}

// linear motor movement from point to point at the start of sequence
void _updateSequencePart2(char endPosition1, char endPosition2, motors * sequence) {
  cBuffer1 = sequence[4].motorL;
  cBuffer2 = sequence[4].motorR;
  sequence[5].motorL = cBuffer1;
  sequence[6].motorL = cBuffer1 + (endPosition1 - cBuffer1) / 4;
  sequence[7].motorL = cBuffer1 + ((endPosition1 - cBuffer1) * 2) / 4;
  sequence[8].motorL = cBuffer1 + ((endPosition1 - cBuffer1) * 3) / 4;
  sequence[9].motorL = endPosition1;
  sequence[5].motorR = cBuffer2;
  sequence[6].motorR = cBuffer2 + (endPosition2 - cBuffer2) / 4;
  sequence[7].motorR = cBuffer2 + ((endPosition2 - cBuffer2) * 2) / 4;
  sequence[8].motorR = cBuffer2 + ((endPosition2 - cBuffer2) * 3) / 4;
  sequence[9].motorR = endPosition2;
}

// linear motor movement from point to point at the start of sequence
void _updateSequencePart3(char endPosition1, char endPosition2, motors * sequence) {
  cBuffer1 = sequence[9].motorL;
  cBuffer2 = sequence[9].motorR;
  sequence[10].motorL = cBuffer1;
  sequence[11].motorL = cBuffer1 + (endPosition1 - cBuffer1) / 4;
  sequence[12].motorL = cBuffer1 + ((endPosition1 - cBuffer1) * 2) / 4;
  sequence[13].motorL = cBuffer1 + ((endPosition1 - cBuffer1) * 3) / 4;
  sequence[14].motorL = endPosition1;
  sequence[10].motorR = cBuffer2;
  sequence[11].motorR = cBuffer2 + (endPosition2 - cBuffer2) / 4;
  sequence[12].motorR = cBuffer2 + ((endPosition2 - cBuffer2) * 2) / 4;
  sequence[13].motorR = cBuffer2 + ((endPosition2 - cBuffer2) * 3) / 4;
  sequence[14].motorR = endPosition2;
}

// linear motor movement from point to point at the start of sequence
void _updateSequencePart4(char endPosition1, char endPosition2, motors * sequence) {
  cBuffer1 = sequence[14].motorL;
  cBuffer2 = sequence[14].motorR;
  sequence[15].motorL = cBuffer1;
  sequence[16].motorL = cBuffer1 + (endPosition1 - cBuffer1) / 4;
  sequence[17].motorL = cBuffer1 + ((endPosition1 - cBuffer1) * 2) / 4;
  sequence[18].motorL = cBuffer1 + ((endPosition1 - cBuffer1) * 3) / 4;
  sequence[19].motorL = endPosition1;
  sequence[15].motorR = cBuffer2;
  sequence[16].motorR = cBuffer2 + (endPosition2 - cBuffer2) / 4;
  sequence[17].motorR = cBuffer2 + ((endPosition2 - cBuffer2) * 2) / 4;
  sequence[18].motorR = cBuffer2 + ((endPosition2 - cBuffer2) * 3) / 4;
  sequence[19].motorR = endPosition2;
}

// create recover sequence
void _setSequenceRecoverCenterL(void) {
    for (i = 0; i < 16; i ++) {
      sequenceWing[i].motorL = -70;
    }
  sequenceWing[17].motorL = 0;
  sequenceWing[18].motorL = 30;
  sequenceWing[19].motorL = 50;
}

// create recover sequence
void _setSequenceRecoverCenterR(void) {
    for (i = 0; i < 16; i ++) {
      sequenceWing[i].motorR = -70;
    }
  sequenceWing[17].motorR = 0;
  sequenceWing[18].motorR = 30;
  sequenceWing[19].motorR = 50;
}

// create recover sequence
void _setSequenceWingsDown(void) {
    for (i = 0; i < 19; i ++) {
      sequenceWing[i].motorL = 50;
      sequenceWing[i].motorR = 50;
    }
}

// get next sequence
void setPattern(unsigned char currentTaskItem) {
  // get new sequence array
  switch (currentTaskItem) {
    case P_DOSTAND:

    {
      _updateSequencePart1(motorLWalk[_centerStatic], motorRWalk[_centerStatic], & sequenceWheel[0]);
      _updateSequencePart2(motorLWalk[_centerStatic], motorRWalk[_centerStatic], & sequenceWheel[0]);
      _updateSequencePart3(motorLWalk[_centerStatic], motorRWalk[_centerStatic], & sequenceWheel[0]);
      _updateSequencePart4(motorLWalk[_centerStatic], motorRWalk[_centerStatic], & sequenceWheel[0]);
      _setSequenceWingsDown();
    }
    break;
    case P_STANDGOLEFT:
    {
      _updateSequencePart1(motorLWalk[_centerStatic - 1], motorRWalk[_centerStatic + 1], & sequenceWheel[0]);
      _updateSequencePart2(motorLWalk[_centerStatic - 1], motorRWalk[_centerStatic + 1], & sequenceWheel[0]);
      _updateSequencePart3(motorLWalk[_centerStatic - 1], motorRWalk[_centerStatic + 1], & sequenceWheel[0]);
      _updateSequencePart4(motorLWalk[_centerStatic], motorRWalk[_centerStatic], & sequenceWheel[0]);
      _setSequenceWingsDown();
    }
    break;
    case P_STANDGORIGHT:
    {
      _updateSequencePart1(motorLWalk[_centerStatic + 1], motorRWalk[_centerStatic - 1], & sequenceWheel[0]);
      _updateSequencePart2(motorLWalk[_centerStatic + 1], motorRWalk[_centerStatic - 1], & sequenceWheel[0]);
      _updateSequencePart3(motorLWalk[_centerStatic + 1], motorRWalk[_centerStatic - 1], & sequenceWheel[0]);
      _updateSequencePart4(motorLWalk[_centerStatic], motorRWalk[_centerStatic], & sequenceWheel[0]);
      _setSequenceWingsDown();
    }
    break;
    case P_GOFORWARD:
    {
      _updateSequencePart1(motorLWalk[_centerStatic + 2], motorRWalk[_centerStatic + 2], & sequenceWheel[0]);
      _updateSequencePart2(motorLWalk[_centerStatic + 2], motorRWalk[_centerStatic + 2], & sequenceWheel[0]);
      _updateSequencePart3(motorLWalk[_centerStatic + 2], motorRWalk[_centerStatic + 2], & sequenceWheel[0]);
      _updateSequencePart4(motorLWalk[_centerStatic + 2], motorRWalk[_centerStatic + 2], & sequenceWheel[0]);
      _setSequenceWingsDown();
    }
    break;
    case P_GOLEFT:
    {
      _updateSequencePart1(motorLWalk[_centerStatic + 1], motorRWalk[_centerStatic + 2], & sequenceWheel[0]);
      _updateSequencePart2(motorLWalk[_centerStatic + 1], motorRWalk[_centerStatic + 2], & sequenceWheel[0]);
      _updateSequencePart3(motorLWalk[_centerStatic + 1], motorRWalk[_centerStatic + 2], & sequenceWheel[0]);
      _updateSequencePart4(motorLWalk[_centerStatic + 1], motorRWalk[_centerStatic + 2], & sequenceWheel[0]);
      _setSequenceWingsDown();
    }
    break;
    case P_GORIGHT:
    {
      _updateSequencePart1(motorLWalk[_centerStatic + 2], motorRWalk[_centerStatic + 1], & sequenceWheel[0]);
      _updateSequencePart2(motorLWalk[_centerStatic + 2], motorRWalk[_centerStatic + 1], & sequenceWheel[0]);
      _updateSequencePart3(motorLWalk[_centerStatic + 2], motorRWalk[_centerStatic + 1], & sequenceWheel[0]);
      _updateSequencePart4(motorLWalk[_centerStatic + 2], motorRWalk[_centerStatic + 1], & sequenceWheel[0]);
      _setSequenceWingsDown();
    }
    break;
    case P_GOBACK:
    {
      _updateSequencePart1(motorLWalk[_centerStatic - 1], motorRWalk[_centerStatic - 1], & sequenceWheel[0]);
      _updateSequencePart2(motorLWalk[_centerStatic - 1], motorRWalk[_centerStatic - 1], & sequenceWheel[0]);
      _updateSequencePart3(motorLWalk[_centerStatic - 1], motorRWalk[_centerStatic - 1], & sequenceWheel[0]);
      _updateSequencePart4(motorLWalk[_centerStatic - 1], motorRWalk[_centerStatic - 1], & sequenceWheel[0]);
      _setSequenceWingsDown();
    }
    break;
    case P_GOBACKLEFT:
    {
      _updateSequencePart1(motorLWalk[_centerStatic - 1], motorRWalk[_centerStatic], & sequenceWheel[0]);
      _updateSequencePart2(motorLWalk[_centerStatic - 1], motorRWalk[_centerStatic], & sequenceWheel[0]);
      _updateSequencePart3(motorLWalk[_centerStatic - 1], motorRWalk[_centerStatic], & sequenceWheel[0]);
      _updateSequencePart4(motorLWalk[_centerStatic - 1], motorRWalk[_centerStatic], & sequenceWheel[0]);
      _setSequenceWingsDown();
    }
    break;
    case P_GOBACKRIGHT:
    {
      _updateSequencePart1(motorLWalk[_centerStatic], motorRWalk[_centerStatic - 1], & sequenceWheel[0]);
      _updateSequencePart2(motorLWalk[_centerStatic], motorRWalk[_centerStatic - 1], & sequenceWheel[0]);
      _updateSequencePart3(motorLWalk[_centerStatic], motorRWalk[_centerStatic - 1], & sequenceWheel[0]);
      _updateSequencePart4(motorLWalk[_centerStatic], motorRWalk[_centerStatic - 1], & sequenceWheel[0]);
      _setSequenceWingsDown();
    }
    break;
    case P_RECOVERLEFT:
    {
      _setSequenceWingsDown();
      _setSequenceRecoverCenterL();
    }
    break;
    case P_RECOVERRIGHT:
    {
      _setSequenceWingsDown();
      _setSequenceRecoverCenterR();
    }
    break;
    case P_RECOVER:
    {
      _setSequenceWingsDown();
      _setSequenceRecoverCenterL();
      _setSequenceRecoverCenterR();
    }
    break;
    default:
    break;
  }
}

// update turning by gyro
void updateTurnPattern(char cAngle) {
  if (cAngle != 0) {
    for (i = 1; i < 18; i ++) {
      sequenceWheel[i].motorL += cAngle;
      sequenceWheel[i].motorR -= cAngle;
    }
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

// update servo motors values
allMotors updateMotorsPatterns(allMotors calibration) {
  // set motors angle values
  motorValue.left = limitValue(sequenceWing[mainCounter].motorL + calibration.left);
  motorValue.right = limitValue(sequenceWing[mainCounter].motorR + calibration.right);
  motorValue.m.wheel.motorL = limitValue(sequenceWheel[mainCounter].motorL + calibration.m.wheel.motorL);
  motorValue.m.wheel.motorR = limitValue(sequenceWheel[mainCounter].motorR + calibration.m.wheel.motorR);
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
    case P_ENABLEINPUTLEFT:
    {
      Serial.println(" P_ENABLEINPUTLEFT ");
    }
    break;
    case P_ENABLEINPUTRIGHT:
    {
      Serial.println(" P_ENABLEINPUTRIGHT ");
    }
    break;
    case P_DISABLEINPUTLEFT:
    {
      Serial.println(" P_DISABLEINPUTLEFT ");
    }
    break;
    case P_DISABLEINPUTRIGHT:
    {
      Serial.println(" P_DISABLEINPUTRIGHT ");
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
    case P_RECOVER:
    {
      Serial.println(" P_RECOVER ");
    }
    break;
    default:
      Serial.println(" unknown pattern ");
    break;
  }
}
*/
