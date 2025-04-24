/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Robot legs motion patterns
*/

// points to currentSequence for every leg
unsigned char mainCounter = 0;
// lifting pattern value
char mLiftToGo[36]    = {  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-4,-8,-16};
char mLiftToStand[36] = {-16,-8,-4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0};
// recovering patterns
char mRecoverLiftDown[36] = {-15,-30,-45,-60,-75,-75,-75,-75,-75,-75,-75,-75,-75,-75,-75,-75,-75,-75,-75,-75,-75,-75,-75,-75,-75,-75,-75,-75,-75,-60,-45,-30,-15,  0,  0, 0};
char mRecoverLiftUp[36]   = { 15, 30, 45, 45, 45, 45, 45, 45, 45, 45,-45,-45,-45,-45,-45,-45,-45,-45,-45,-45,-45,-45,-45,-45,-45,-45,-45,-45,-45,-45,-45,-30,-15,  0,  0, 0};

// motors values for 12 motors
allMotors motorValue = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
allMotors motorLift = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
// leg timing phase
unsigned char counterFL = 0;
unsigned char counterFR = 0;
unsigned char counterRL = 0;
unsigned char counterRR = 0;
// task item
unsigned char taskItemBuffer;

// linear legs lift
void _addLinearLift(char * leftSide, char * rightSide, char FL, char FR, char RL, char RR) {
  motorLift.fl.motor1 = leftSide[mainCounter] * FL;
  motorLift.fl.motor2 = leftSide[mainCounter] * FL;
  motorLift.fr.motor1 = rightSide[mainCounter] * FR;
  motorLift.fr.motor2 = rightSide[mainCounter] * FR;
  motorLift.rl.motor1 = leftSide[mainCounter] * RL;
  motorLift.rl.motor2 = leftSide[mainCounter] * RL;
  motorLift.rr.motor1 = rightSide[mainCounter] * RR;
  motorLift.rr.motor2 = rightSide[mainCounter] * RR;
}

// update servo motors values
unsigned char updateCountPatterns(void) {
  // update sequence shift 
  mainCounter ++;
  if (mainCounter >= m_fullCycle) {
    mainCounter = 0;
  }
  // FL
  counterFL = mainCounter;
  // FR
  counterFR = mainCounter + m_halfCycle;
  if (counterFR >= m_fullCycle) {
    counterFR -= m_fullCycle;
  }
  // RL
  counterRL = mainCounter + m_halfCycle;
  if (counterRL >= m_fullCycle) {
    counterRL -= m_fullCycle;
  }
  // RR
  counterRR = mainCounter;
  return mainCounter;
}

// get next sequence
void setPattern(unsigned char currentTaskItem) {
  // remember current pattern
  taskItemBuffer = currentTaskItem;
}

// get servo motor steps
allMotors getWalkPatterns(void) {
  motors legSet;
  legSet = getWalkStep(counterFL);
  motorValue.fl.motor1 = legSet.motor1;
  motorValue.fl.motor2 = legSet.motor2;
  legSet = getWalkStep(counterFR);
  motorValue.fr.motor1 = legSet.motor1;
  motorValue.fr.motor2 = legSet.motor2;
  legSet = getWalkStep(counterRL);
  motorValue.rl.motor1 = legSet.motor1;
  motorValue.rl.motor2 = legSet.motor2;
  legSet = getWalkStep(counterRR);
  motorValue.rr.motor1 = legSet.motor1;
  motorValue.rr.motor2 = legSet.motor2;
  return motorValue;
}

// get servo motors values for lift
allMotors getLiftPatterns(void) {
  // add linear lift
  switch (taskItemBuffer) {
    case P_STANDTOGO:
    {
      _addLinearLift(mLiftToGo, mLiftToGo, 1, 0, 0, 1);
    }
    break;
    case P_GOTOSTAND:
    {
      _addLinearLift(mLiftToStand, mLiftToStand, 1, 0, 0, 1);
    }
    break;
    case P_RECOVERLEFT:
    {
      _addLinearLift(mRecoverLiftDown, mRecoverLiftUp, 1, 1, 1, 1);
    }
    break;
    case P_RECOVERRIGHT:
    {
      _addLinearLift(mRecoverLiftUp ,mRecoverLiftDown, 1, 1, 1, 1);
    }
    break;
    default:
    {
      // set motors angle values
      motorLift.fl.motor1 = 0;
      motorLift.fl.motor2 = 0;
      motorLift.fr.motor1 = 0;
      motorLift.fr.motor2 = 0;
      motorLift.rl.motor1 = 0;
      motorLift.rl.motor2 = 0;
      motorLift.rr.motor1 = 0;
      motorLift.rr.motor2 = 0;
    }
    break;
  }
  return motorLift;
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
