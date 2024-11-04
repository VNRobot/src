/*
Simple Robot
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Robot legs motion patterns
*/

// left motor, right motor
struct sequence {
  char left;
  char right;
};
// points to currentSequence 
unsigned char sequenceCount = 0;
// 20 positions per sequence
sequence currentSequence[20] = {
  0,  0,
  0,  0,
  0,  0,
  0,  0,

  0,  0,
  0,  0,
  0,  0,
  0,  0,

  0,  0,
  0,  0,
  0,  0,
  0,  0,

  0,  0,
  0,  0,
  0,  0,
  0,  0,

  0,  0,
  0,  0,
  0,  0,
  0,  0
};

// char buffer for temporary use
char cBuffer;
// motors values for 10 motors
allMotors motorValue = {0, 0};
// interruption flag
bool iterrupted = false;

// linear motor movement from point to point at the start of sequence
void _updateSequenceLinearStart(char endPosition1, char endPosition2) {
  // motor 1
  cBuffer = currentSequence[19].left;
  currentSequence[0].left = cBuffer;
  currentSequence[1].left = cBuffer + (endPosition1 - cBuffer) / 4;
  currentSequence[2].left = cBuffer + ((endPosition1 - cBuffer) * 2) / 4;
  currentSequence[3].left = cBuffer + ((endPosition1 - cBuffer) * 3) / 4;
  currentSequence[4].left = endPosition1;
  currentSequence[5].left = endPosition1;
  currentSequence[6].left = endPosition1;
  currentSequence[7].left = endPosition1;
  currentSequence[8].left = endPosition1;
  currentSequence[9].left = endPosition1;
  currentSequence[10].left = endPosition1;
  currentSequence[11].left = endPosition1;
  currentSequence[12].left = endPosition1;
  currentSequence[13].left = endPosition1;
  currentSequence[14].left = endPosition1;
  currentSequence[15].left = endPosition1;
  currentSequence[16].left = endPosition1;
  currentSequence[17].left = endPosition1;
  currentSequence[18].left = endPosition1;
  currentSequence[19].left = endPosition1;
  // motor 2
  cBuffer = currentSequence[19].right;
  currentSequence[0].right = cBuffer;
  currentSequence[1].right = cBuffer + (endPosition2 - cBuffer) / 4;
  currentSequence[2].right = cBuffer + ((endPosition2 - cBuffer) * 2) / 4;
  currentSequence[3].right = cBuffer + ((endPosition2 - cBuffer) * 3) / 4;
  currentSequence[4].right = endPosition2;
  currentSequence[5].right = endPosition2;
  currentSequence[6].right = endPosition2;
  currentSequence[7].right = endPosition2;
  currentSequence[8].right = endPosition2;
  currentSequence[9].right = endPosition2;
  currentSequence[10].right = endPosition2;
  currentSequence[11].right = endPosition2;
  currentSequence[12].right = endPosition2;
  currentSequence[13].right = endPosition2;
  currentSequence[14].right = endPosition2;
  currentSequence[15].right = endPosition2;
  currentSequence[16].right = endPosition2;
  currentSequence[17].right = endPosition2;
  currentSequence[18].right = endPosition2;
  currentSequence[19].right = endPosition2;
}

// create stand walk sequence
void _updateSequenceRelative(char m1, char m2) {
  // motor 1
  currentSequence[0].left += m1 / 4;
  currentSequence[1].left += m1 / 2;
  currentSequence[2].left += m1;
  currentSequence[3].left += m1;
  currentSequence[4].left += m1;
  currentSequence[5].left += m1;
  currentSequence[6].left += m1;
  currentSequence[7].left += m1;
  currentSequence[8].left += m1;
  currentSequence[9].left += m1;
  currentSequence[10].left += m1;
  currentSequence[11].left += m1;
  currentSequence[12].left += m1;
  currentSequence[13].left += m1;
  currentSequence[14].left += m1;
  currentSequence[15].left += m1;
  currentSequence[16].left += m1;
  currentSequence[17].left += m1;
  currentSequence[18].left += m1;
  currentSequence[19].left += m1;
  // motor 2
  currentSequence[0].right += m2 / 4;
  currentSequence[1].right += m2 / 2;
  currentSequence[2].right += m2;
  currentSequence[3].right += m2;
  currentSequence[4].right += m2;
  currentSequence[5].right += m2;
  currentSequence[6].right += m2;
  currentSequence[7].right += m2;
  currentSequence[8].right += m2;
  currentSequence[9].right += m2;
  currentSequence[10].right += m2;
  currentSequence[11].right += m2;
  currentSequence[12].right += m2;
  currentSequence[13].right += m2;
  currentSequence[14].right += m2;
  currentSequence[15].right += m2;
  currentSequence[16].right += m2;
  currentSequence[17].right += m2;
  currentSequence[18].right += m2;
  currentSequence[19].right += m2;
}

// get next sequence
void setPattern(unsigned char currentTaskItem) {
  // reset interruption
  iterrupted = false;
  // get new sequence array
  switch (currentTaskItem) {
    case P_DOSTAND:
    {
      _updateSequenceLinearStart(0, 0);
    }
    break;
    case P_STANDTOGO:
    {
      _updateSequenceLinearStart(0, 0);
    }
    break;
    case P_GOTOSTAND:
    {
      _updateSequenceLinearStart(0, 0);
    }
    break;
    case P_STANDGO:
    {
      _updateSequenceLinearStart(0, 0);
    }
    break;
    case P_STANDGOLEFT:
    {
      _updateSequenceLinearStart(-8, 8);
    }
    break;
    case P_STANDGORIGHT:
    {
      _updateSequenceLinearStart(8, -8);
    }
    break;
    case P_GOFORWARD:
    {
      _updateSequenceLinearStart(14, 14);
    }
    break;
    case P_GOLEFT:
    {
      _updateSequenceLinearStart(8, 14);
    }
    break;
    case P_GORIGHT:
    {
      _updateSequenceLinearStart(14, 8);
    }
    break;
    case P_GOBACK:
    {
      _updateSequenceLinearStart(-8, -8);
    }
    break;
    case P_GOBACKLEFT:
    {
      _updateSequenceLinearStart(-10, -8);
    }
    break;
    case P_GOBACKRIGHT:
    {
      _updateSequenceLinearStart(-8, -10);
    }
    break;
    default:
    break;
  }
}

// update turning
void updateTurnPattern(char cAngle) {
  _updateSequenceRelative(cAngle, -cAngle);
}

// update servo motors values
allMotors updateMotorsPatterns(void) {
  // set motors speed values
  if (iterrupted) {
    // just slowdown if interrupted
    motorValue.left  = (currentSequence[sequenceCount].left) / 3;
    motorValue.right = (currentSequence[sequenceCount].right) / 3;
  } else {
    // normal operation
    motorValue.left  = (currentSequence[sequenceCount].left);
    motorValue.right = (currentSequence[sequenceCount].right);
  }
  return motorValue;
}

// update servo motors values
unsigned char updateCountPatterns(void) {
  // update sequence shift 
  if (sequenceCount < 19) {
    sequenceCount ++;
  } else {
    // last sequence in the pattern. Get next pattern
    sequenceCount = 0;
  }
  return sequenceCount;
}

// set interruped flag
void setInterruptedPattern(void) {
  iterrupted = true;
}

// print sequence name
void printPatternName(unsigned char currentTaskItem) {
  // get new sequence array
  switch (currentTaskItem) {
    case P_DOSTAND:
    {
      Serial.println(F(" P_DOSTAND "));
    }
    break;
    case P_STANDTOGO:
    {
      Serial.println(F(" P_STANDTOGO "));
    }
    break;
    case P_GOTOSTAND:
    {
      Serial.println(F(" P_GOTOSTAND "));
    }
    break;
    case P_STANDGO:
    {
      Serial.println(F(" P_STANDGO "));
    }
    break;
    case P_STANDGOLEFT:
    {
      Serial.println(F(" P_STANDGOLEFT "));
    }
    break;
    case P_STANDGORIGHT:
    {
      Serial.println(F(" P_STANDGORIGHT "));
    }
    break;
    case P_GOFORWARD:
    {
      Serial.println(F(" P_GOFORWARD "));
    }
    break;
    case P_GOLEFT:
    {
      Serial.println(F(" P_GOLEFT "));
    }
    break;
    case P_GORIGHT:
    {
      Serial.println(F(" P_GORIGHT "));
    }
    break;
    case P_GOBACK:
    {
      Serial.println(F(" P_GOBACK "));
    }
    break;
    case P_GOBACKLEFT:
    {
      Serial.println(F(" P_GOBACKLEFT "));
    }
    break;
    case P_GOBACKRIGHT:
    {
      Serial.println(F(" P_GOBACKRIGHT "));
    }
    break;
    case P_DONE:
    {
      Serial.println(F(" P_DONE "));
    }
    break;
    case P_RESETDIRECTION:
    {
      Serial.println(F(" P_RESETDIRECTION "));
    }
    break;
    case P_RESTOREDIRECTION:
    {
      Serial.println(F(" P_RESTOREDIRECTION "));
    }
    break;
    case P_ENABLEINPUTS:
    {
      Serial.println(F(" P_ENABLEINPUTS "));
    }
    break;
    case P_DISABLEINPUTS:
    {
      Serial.println(F(" P_DISABLEINPUTS "));
    }
    break;
    default:
      Serial.println(F(" unknown pattern "));
    break;
  }
}
  

