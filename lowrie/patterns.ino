/*
Wlking Robot Lowrie
Licended GNU GPLv3 2023
Arduino nano
Robot legs motion patterns
*/

// forward shift        {-45, -45, -45, -40, -35, -30, -25, -20, -15,-10, -5,  0,  5, 10, 15, 20, 25, 30, 35, 35, 35};
// servos angles for forward shift 
char _motor1Walk[21] =  { 45,  45,  45,  47,  49,  52,  55,  58,  61, 65, 69, 73, 77, 81, 85, 90, 96,102,108,108,108};
char _motor2Walk[21] =  {100, 100, 100,  90,  83,  77,  71,  65,  59, 54, 49, 45, 41, 38, 35, 33, 31, 30, 29, 29, 29};
// char buffer for temporary use
char _cBuffer;
// leg lift angles
unsigned char liftm1 = 12;
unsigned char liftm2 = 12;

// set new values of center motors
void _updateCenterMotor(char cAngleF, char cAngleR) {
  // front motor
  m_currentSequence[0].front = -cAngleF/2;
  m_currentSequence[1].front = -cAngleF;
  m_currentSequence[2].front = -cAngleF;
  m_currentSequence[3].front = -cAngleF;
  m_currentSequence[4].front = -cAngleF;
  m_currentSequence[5].front = -cAngleF;
  m_currentSequence[6].front = -cAngleF;
  m_currentSequence[7].front = -cAngleF/2;
  m_currentSequence[8].front = cAngleF/2;
  m_currentSequence[9].front = cAngleF;
  m_currentSequence[10].front = cAngleF;
  m_currentSequence[11].front = cAngleF;
  m_currentSequence[12].front = cAngleF;
  m_currentSequence[13].front = cAngleF;
  m_currentSequence[14].front = cAngleF;
  m_currentSequence[15].front = cAngleF/2;
  // rear motor
  m_currentSequence[0].rear = -cAngleR/2;
  m_currentSequence[1].rear = -cAngleR;
  m_currentSequence[2].rear = -cAngleR;
  m_currentSequence[3].rear = -cAngleR;
  m_currentSequence[4].rear = -cAngleR;
  m_currentSequence[5].rear = -cAngleR;
  m_currentSequence[6].rear = -cAngleR;
  m_currentSequence[7].rear = -cAngleR/2;
  m_currentSequence[8].rear = cAngleR/2;
  m_currentSequence[9].rear = cAngleR;
  m_currentSequence[10].rear = cAngleR;
  m_currentSequence[11].rear = cAngleR;
  m_currentSequence[12].rear = cAngleR;
  m_currentSequence[13].rear = cAngleR;
  m_currentSequence[14].rear = cAngleR;
  m_currentSequence[15].rear = cAngleR/2;
}

// set default values of center motors
// use 0 for now
void _defaultCenterMotor(void) {
  // front motor
  m_currentSequence[0].front = 0;
  m_currentSequence[1].front = 0;
  m_currentSequence[2].front = 0;
  m_currentSequence[3].front = 0;
  m_currentSequence[4].front = 0;
  m_currentSequence[5].front = 0;
  m_currentSequence[6].front = 0;
  m_currentSequence[7].front = 0;
  m_currentSequence[8].front = 0;
  m_currentSequence[9].front = 0;
  m_currentSequence[10].front = 0;
  m_currentSequence[11].front = 0;
  m_currentSequence[12].front = 0;
  m_currentSequence[13].front = 0;
  m_currentSequence[14].front = 0;
  m_currentSequence[15].front = 0;
  // rear motor
  m_currentSequence[0].rear = 0;
  m_currentSequence[1].rear = 0;
  m_currentSequence[2].rear = 0;
  m_currentSequence[3].rear = 0;
  m_currentSequence[4].rear = 0;
  m_currentSequence[5].rear = 0;
  m_currentSequence[6].rear = 0;
  m_currentSequence[7].rear = 0;
  m_currentSequence[8].rear = 0;
  m_currentSequence[9].rear = 0;
  m_currentSequence[10].rear = 0;
  m_currentSequence[11].rear = 0;
  m_currentSequence[12].rear = 0;
  m_currentSequence[13].rear = 0;
  m_currentSequence[14].rear = 0;
  m_currentSequence[15].rear = 0;
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
  _cBuffer = m_currentSequence[15].m.motor1;
  m_currentSequence[0].m.motor1 = _cBuffer;
  m_currentSequence[1].m.motor1 = _cBuffer + (endPosition1 - _cBuffer) / 14;
  m_currentSequence[2].m.motor1 = _cBuffer + ((endPosition1 - _cBuffer) * 2) / 14;
  m_currentSequence[3].m.motor1 = _cBuffer + ((endPosition1 - _cBuffer) * 3) / 14;
  m_currentSequence[4].m.motor1 = _cBuffer + ((endPosition1 - _cBuffer) * 4) / 14;
  m_currentSequence[5].m.motor1 = _cBuffer + ((endPosition1 - _cBuffer) * 5) / 14;
  m_currentSequence[6].m.motor1 = _cBuffer + ((endPosition1 - _cBuffer) * 6) / 14;
  m_currentSequence[7].m.motor1 = _cBuffer + ((endPosition1 - _cBuffer) * 7) / 14;
  m_currentSequence[8].m.motor1 = _cBuffer + ((endPosition1 - _cBuffer) * 8) / 14;
  m_currentSequence[9].m.motor1 = _cBuffer + ((endPosition1 - _cBuffer) * 9) / 14;
  m_currentSequence[10].m.motor1 = _cBuffer + ((endPosition1 - _cBuffer) * 10) / 14;
  m_currentSequence[11].m.motor1 = _cBuffer + ((endPosition1 - _cBuffer) * 11) / 14;
  m_currentSequence[12].m.motor1 = _cBuffer + ((endPosition1 - _cBuffer) * 12) / 14;
  m_currentSequence[13].m.motor1 = _cBuffer + ((endPosition1 - _cBuffer) * 13) / 14;
  m_currentSequence[14].m.motor1 = endPosition1;
  m_currentSequence[15].m.motor1 = endPosition1;
  // motor 2
  _cBuffer = m_currentSequence[15].m.motor2;
  m_currentSequence[0].m.motor2 = _cBuffer;
  m_currentSequence[1].m.motor2 = _cBuffer + (endPosition2 - _cBuffer) / 14;
  m_currentSequence[2].m.motor2 = _cBuffer + ((endPosition2 - _cBuffer) * 2) / 14;
  m_currentSequence[3].m.motor2 = _cBuffer + ((endPosition2 - _cBuffer) * 3) / 14;
  m_currentSequence[4].m.motor2 = _cBuffer + ((endPosition2 - _cBuffer) * 4) / 14;
  m_currentSequence[5].m.motor2 = _cBuffer + ((endPosition2 - _cBuffer) * 5) / 14;
  m_currentSequence[6].m.motor2 = _cBuffer + ((endPosition2 - _cBuffer) * 6) / 14;
  m_currentSequence[7].m.motor2 = _cBuffer + ((endPosition2 - _cBuffer) * 7) / 14;
  m_currentSequence[8].m.motor2 = _cBuffer + ((endPosition2 - _cBuffer) * 8) / 14;
  m_currentSequence[9].m.motor2 = _cBuffer + ((endPosition2 - _cBuffer) * 9) / 14;
  m_currentSequence[10].m.motor2 = _cBuffer + ((endPosition2 - _cBuffer) * 10) / 14;
  m_currentSequence[11].m.motor2 = _cBuffer + ((endPosition2 - _cBuffer) * 11) / 14;
  m_currentSequence[12].m.motor2 = _cBuffer + ((endPosition2 - _cBuffer) * 12) / 14;
  m_currentSequence[13].m.motor2 = _cBuffer + ((endPosition2 - _cBuffer) * 13) / 14;
  m_currentSequence[14].m.motor2 = endPosition2;
  m_currentSequence[15].m.motor2 = endPosition2;
}

// linear motor movement from point to point at the end of sequence
void _updateSequenceLinearEnd(char endPosition1, char endPosition2) {
  // motor 1
  _cBuffer = m_currentSequence[15].m.motor1;
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
  m_currentSequence[12].m.motor1 = _cBuffer + (endPosition1 - _cBuffer) / 4;
  m_currentSequence[13].m.motor1 = _cBuffer + ((endPosition1 - _cBuffer) * 2) / 4;
  m_currentSequence[14].m.motor1 = _cBuffer + ((endPosition1 - _cBuffer) * 3) / 4;
  m_currentSequence[15].m.motor1 = endPosition1;
  // motor 2
  _cBuffer = m_currentSequence[15].m.motor2;
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
  m_currentSequence[12].m.motor2 = _cBuffer + (endPosition2 - _cBuffer) / 4;
  m_currentSequence[13].m.motor2 = _cBuffer + ((endPosition2 - _cBuffer) * 2) / 4;
  m_currentSequence[14].m.motor2 = _cBuffer + ((endPosition2 - _cBuffer) * 3) / 4;
  m_currentSequence[15].m.motor2 = endPosition2;
}

// linear motor movement from point to point at the start of sequence
void _updateSequenceLinearStart(char endPosition1, char endPosition2) {
  // motor 1
  _cBuffer = m_currentSequence[15].m.motor1;
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
  // motor 2
  _cBuffer = m_currentSequence[15].m.motor2;
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
}

// create stand walk sequence
void _updateSequenceStandWalk(char m1, char m2) {
  // motor 1
  m_currentSequence[0].m.motor1 = m1 - liftm1;
  m_currentSequence[1].m.motor1 = m1 - liftm1 / 2;
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
  m_currentSequence[14].m.motor1 = m1 - liftm1 / 4;
  m_currentSequence[15].m.motor1 = m1 - liftm1 / 2;
  // motor 2
  m_currentSequence[0].m.motor2 = m2 - liftm2;
  m_currentSequence[1].m.motor2 = m2 - liftm2 / 2;
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
  m_currentSequence[14].m.motor2 = m2 - liftm2 / 4;
  m_currentSequence[15].m.motor2 = m2 - liftm2 / 2;
}

// create walk sequence
void _updateSequenceWalk(char m1[], char m2[]) {
  // motor 1
  m_currentSequence[0].m.motor1 = m1[m_center] - liftm1;
  m_currentSequence[1].m.motor1 = m1[m_center - 3] - liftm1;
  m_currentSequence[2].m.motor1 = m1[m_center - 6] - liftm1 / 4;
  m_currentSequence[3].m.motor1 = m1[m_center - 5];
  m_currentSequence[4].m.motor1 = m1[m_center - 4];
  m_currentSequence[5].m.motor1 = m1[m_center - 3];
  m_currentSequence[6].m.motor1 = m1[m_center - 2];
  m_currentSequence[7].m.motor1 = m1[m_center - 1];
  m_currentSequence[8].m.motor1 = m1[m_center];
  m_currentSequence[9].m.motor1 = m1[m_center + 1];
  m_currentSequence[10].m.motor1 = m1[m_center + 2];
  m_currentSequence[11].m.motor1 = m1[m_center + 3];
  m_currentSequence[12].m.motor1 = m1[m_center + 4];
  m_currentSequence[13].m.motor1 = m1[m_center + 5];
  m_currentSequence[14].m.motor1 = m1[m_center + 6] - liftm1 / 4;
  m_currentSequence[15].m.motor1 = m1[m_center + 3] - liftm1;
  // motor 2
  m_currentSequence[0].m.motor2 = m2[m_center] - liftm2;
  m_currentSequence[1].m.motor2 = m2[m_center - 3] - liftm2;
  m_currentSequence[2].m.motor2 = m2[m_center - 6] - liftm2 / 4;
  m_currentSequence[3].m.motor2 = m2[m_center - 5];
  m_currentSequence[4].m.motor2 = m2[m_center - 4];
  m_currentSequence[5].m.motor2 = m2[m_center - 3];
  m_currentSequence[6].m.motor2 = m2[m_center - 2];
  m_currentSequence[7].m.motor2 = m2[m_center - 1];
  m_currentSequence[8].m.motor2 = m2[m_center];
  m_currentSequence[9].m.motor2 = m2[m_center + 1];
  m_currentSequence[10].m.motor2 = m2[m_center + 2];
  m_currentSequence[11].m.motor2 = m2[m_center + 3];
  m_currentSequence[12].m.motor2 = m2[m_center + 4];
  m_currentSequence[13].m.motor2 = m2[m_center + 5];
  m_currentSequence[14].m.motor2 = m2[m_center + 6] - liftm2 / 4;
  m_currentSequence[15].m.motor2 = m2[m_center + 3] - liftm2;
}

// create walk back sequence
void _updateSequenceWalkBack(char m1[], char m2[]) {
  // motor 1
  m_currentSequence[0].m.motor1 = m1[m_center] - liftm1;
  m_currentSequence[15].m.motor1 = m1[m_center - 3] - liftm1;
  m_currentSequence[14].m.motor1 = m1[m_center - 6] - liftm1 / 4;
  m_currentSequence[13].m.motor1 = m1[m_center - 5];
  m_currentSequence[12].m.motor1 = m1[m_center - 4];
  m_currentSequence[11].m.motor1 = m1[m_center - 3];
  m_currentSequence[10].m.motor1 = m1[m_center - 2];
  m_currentSequence[9].m.motor1 = m1[m_center - 1];
  m_currentSequence[8].m.motor1 = m1[m_center];
  m_currentSequence[7].m.motor1 = m1[m_center + 1];
  m_currentSequence[6].m.motor1 = m1[m_center + 2];
  m_currentSequence[5].m.motor1 = m1[m_center + 3];
  m_currentSequence[4].m.motor1 = m1[m_center + 4];
  m_currentSequence[3].m.motor1 = m1[m_center + 5];
  m_currentSequence[2].m.motor1 = m1[m_center + 6] - liftm1 / 4;
  m_currentSequence[1].m.motor1 = m1[m_center + 3] - liftm1;
  // motor 2
  m_currentSequence[0].m.motor2 = m2[m_center] - liftm2;
  m_currentSequence[15].m.motor2 = m2[m_center - 3] - liftm2;
  m_currentSequence[14].m.motor2 = m2[m_center - 6] - liftm2 / 4;
  m_currentSequence[13].m.motor2 = m2[m_center - 5];
  m_currentSequence[12].m.motor2 = m2[m_center - 4];
  m_currentSequence[11].m.motor2 = m2[m_center - 3];
  m_currentSequence[10].m.motor2 = m2[m_center - 2];
  m_currentSequence[9].m.motor2 = m2[m_center - 1];
  m_currentSequence[8].m.motor2 = m2[m_center];
  m_currentSequence[7].m.motor2 = m2[m_center + 1];
  m_currentSequence[6].m.motor2 = m2[m_center + 2];
  m_currentSequence[5].m.motor2 = m2[m_center + 3];
  m_currentSequence[4].m.motor2 = m2[m_center + 4];
  m_currentSequence[3].m.motor2 = m2[m_center + 5];
  m_currentSequence[2].m.motor2 = m2[m_center + 6] - liftm2 / 4;
  m_currentSequence[1].m.motor2 = m2[m_center + 3] - liftm2;
}


// get next sequence
unsigned char setNextPattern(unsigned char currentTaskItem) {
  // get new sequence array
  switch (currentTaskItem) {
    case DOSTAND:
    {
      _updateSequenceLinear(_motor1Walk[m_center], _motor2Walk[m_center]);
      m_leftRightShift = 0;
      _updateTurn(0);
      m_patternCounter ++;
    }
    break;
    case STANDTOWALK:
    {
      _updateSequenceLinearEnd(_motor1Walk[m_center] - liftm1, _motor2Walk[m_center] - liftm2);
      m_leftRightShift = 16;
      _updateTurn(0);
      m_patternCounter ++;
    }
    break;
    case WALKTOSTAND:
    {
      _updateSequenceLinearStart(_motor1Walk[m_center], _motor2Walk[m_center]);
      m_leftRightShift = 16;
      _updateTurn(0);
      m_patternCounter ++;
    }
    break;
    case STANDWALK:
    {
      _updateSequenceStandWalk(_motor1Walk[m_center], _motor2Walk[m_center]);
      m_leftRightShift = 8;
      m_patternCounter ++;
      return STANDWALK; // used to set direction
    }
    break;
    case STANDWALKLEFT:
    {
      _updateSequenceStandWalk(_motor1Walk[m_center], _motor2Walk[m_center]);
      m_leftRightShift = 8;
      _updateTurn(-10);
      m_patternCounter ++;
    }
    break;
    case STANDWALKRIGHT:
    {
      _updateSequenceStandWalk(_motor1Walk[m_center], _motor2Walk[m_center]);
      m_leftRightShift = 8;
      _updateTurn(10);
      m_patternCounter ++;
    }
    break;
    case STANDWALKSHIFTLEFT:
    {
      _updateSequenceStandWalk(_motor1Walk[m_center], _motor2Walk[m_center]);
      m_leftRightShift = 8;
      _updateShift(-10);
      m_patternCounter ++;
    }
    break;
    case STANDWALKSHIFTRIGHT:
    {
      _updateSequenceStandWalk(_motor1Walk[m_center], _motor2Walk[m_center]);
      m_leftRightShift = 8;
      _updateShift(10);
      m_patternCounter ++;
    }
    break;
    case WALKFORWARD:
    {
      _updateSequenceWalk(_motor1Walk, _motor2Walk);
      m_leftRightShift = 8;
      m_patternCounter ++;
      return WALKFORWARD; // used to set direction
    }
    break;
    case WALKLEFT:
    {
      _updateSequenceWalk(_motor1Walk, _motor2Walk);
      m_leftRightShift = 8;
      _updateTurn(-10);
      m_patternCounter ++;
    }
    break;
    case WALKRIGHT:
    {
      _updateSequenceWalk(_motor1Walk, _motor2Walk);
      m_leftRightShift = 8;
      _updateTurn(10);
      m_patternCounter ++;
    }
    break;
    case WALKSHIFTLEFT:
    {
      _updateSequenceWalk(_motor1Walk, _motor2Walk);
      m_leftRightShift = 8;
      _updateShift(-10);
      m_patternCounter ++;
    }
    break;
    case WALKSHIFTRIGHT:
    {
      _updateSequenceWalk(_motor1Walk, _motor2Walk);
      m_leftRightShift = 8;
      _updateShift(10);
      m_patternCounter ++;
    }
    break;
    case WALKBACK:
    {
      _updateSequenceWalkBack(_motor1Walk, _motor2Walk);
      m_leftRightShift = 8;
      _updateTurn(0);
      m_patternCounter ++;
    }
    break;
    case DODOWN:
    {
      _updateSequenceLinear(30, 30);
      m_leftRightShift = 0;
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
      _updateSequenceLinear(30, 30);
      m_leftRightShift = 0;
      return DOCALIBRATION;
    break;
    case DOCALIBRATION_1:
      _updateSequenceLinear(90, -45);
      m_leftRightShift = 0;
      return DOCALIBRATION;
    break;
    case DOCALIBRATION_2:
      _updateSequenceLinear(-45, 90);
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
