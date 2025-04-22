/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Robot legs motion patterns
*/

// points to currentSequence for every leg
unsigned char mainCounter = 0;
// lifting pattern value * 4
char m1Lift[36]       = {-4,-4,-4,-4,-4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-4,-4,-4};
char m2Lift[36]       = {-4,-4,-4,-4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-4,-4,-4,-4};
char mLiftToGo[36]    = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-1,-2,-4};
char mLiftToStand[36] = {-4,-2,-1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
// walking pointers
char mWalk[36]        = { 0,-5,-10,-15,-14,-13,-12,-11,-10,-9,-8,-7,-6,-5,-4,-3,-2,-1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,10, 5};
// recovering patterns
char mRecoverLiftDown[36] = {-15,-30,-45,-60,-75,-75,-75,-75,-75,-75,-75,-75,-75,-75,-75,-75,-75,-75,-75,-75,-75,-75,-75,-75,-75,-75,-75,-75,-75,-60,-45,-30,-15,  0,  0, 0};
char mRecoverLiftUp[36]   = { 15, 30, 45, 45, 45, 45, 45, 45, 45, 45,-45,-45,-45,-45,-45,-45,-45,-45,-45,-45,-45,-45,-45,-45,-45,-45,-45,-45,-45,-45,-45,-30,-15,  0,  0, 0};

// motors values for 10 motors
allMotors motorValue = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
allMotors motorLift = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
// leg timing phase
unsigned char counterFL = 0;
unsigned char counterFR = 0;
unsigned char counterRL = 0;
unsigned char counterRR = 0;
// task item
unsigned char taskItemBuffer;
// walk speed can be: -1, 0, 1, 2
char speedL = 0;
char speedR = 0;
// walk lift value
char walkLiftHight = 0;

// linear legs lift
void _addLinearLift(char * seftSide, char * rightSide, char FL, char FR, char RL, char RR) {
  motorLift.fl.motor1 += seftSide[mainCounter] * FL;
  motorLift.fl.motor2 += seftSide[mainCounter] * FL;
  motorLift.fr.motor1 += rightSide[mainCounter] * FR;
  motorLift.fr.motor2 += rightSide[mainCounter] * FR;
  motorLift.rl.motor1 += seftSide[mainCounter] * RL;
  motorLift.rl.motor2 += seftSide[mainCounter] * RL;
  motorLift.rr.motor1 += rightSide[mainCounter] * RR;
  motorLift.rr.motor2 += rightSide[mainCounter] * RR;
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
void setPattern(unsigned char currentTaskItem, char angleTurn) {
  // remember current pattern
  taskItemBuffer = currentTaskItem;
  // set walk speed
  switch (taskItemBuffer) {
    case P_GOLEFT:
    case P_GORIGHT:
    case P_GOSHIFTLEFT:
    case P_GOSHIFTRIGHT:
    case P_GOFORWARD:
    {
      speedL = m_speedPatternValue;
      speedR = m_speedPatternValue;
    }
    break;
    case P_GOFORWARDSLOW:
    {
      speedL = 1;
      speedR = 1;
    }
    break;
    case P_GOBACK:
    case P_GOBACKLEFT:
    case P_GOBACKRIGHT:
    {
      speedL = -1;
      speedR = -1;
    }
    break;
    default:
    {
      speedL = 0;
      speedR = 0;
    }
    break;
  }
  // set walking leg lift
  switch (taskItemBuffer) {
    case P_STANDGO:
    case P_STANDGOLEFT:
    case P_STANDGORIGHT:
    case P_STANDGOSHIFTLEFT:
    case P_STANDGOSHIFTRIGHT:
    case P_GOFORWARDSLOW:
    case P_GOBACK:
    case P_GOBACKLEFT:
    case P_GOBACKRIGHT:
    {
      walkLiftHight = m_legPatternLift;
    }
    break;
    case P_GOFORWARD:
    case P_GOLEFT:
    case P_GORIGHT:
    case P_GOSHIFTLEFT:
    case P_GOSHIFTRIGHT:
    {
      walkLiftHight = m_legPatternLift * m_liftHighPatternMultiplier;
    }
    break;
    default:
    {
      walkLiftHight = 0;
    }
    break;
  }
}

// get servo motor points
allMotors getWalkPatterns(void) {
  motorValue.fl.motor1 = mWalk[counterFL] * speedL;
  motorValue.fl.motor2 = mWalk[counterFL] * speedL;
  motorValue.fr.motor1 = mWalk[counterFR] * speedR;
  motorValue.fr.motor2 = mWalk[counterFR] * speedR;
  motorValue.rl.motor1 = mWalk[counterRL] * speedL;
  motorValue.rl.motor2 = mWalk[counterRL] * speedL;
  motorValue.rr.motor1 = mWalk[counterRR] * speedR;
  motorValue.rr.motor2 = mWalk[counterRR] * speedR;
  return motorValue;
}

// get servo motors values for lift
allMotors getLiftPatterns(void) {
  // set motors angle values
  motorLift.fl.motor1 = m1Lift[counterFL] * walkLiftHight;
  motorLift.fl.motor2 = m2Lift[counterFL] * walkLiftHight;
  motorLift.fr.motor1 = m1Lift[counterFR] * walkLiftHight;
  motorLift.fr.motor2 = m2Lift[counterFR] * walkLiftHight;
  motorLift.rl.motor1 = m1Lift[counterRL] * walkLiftHight;
  motorLift.rl.motor2 = m2Lift[counterRL] * walkLiftHight;
  motorLift.rr.motor1 = m1Lift[counterRR] * walkLiftHight;
  motorLift.rr.motor2 = m2Lift[counterRR] * walkLiftHight;
  // add linear lift
  switch (taskItemBuffer) {
    case P_STANDTOGO:
    {
      _addLinearLift(mLiftToGo, mLiftToGo, 4, 0, 0, 4);
    }
    break;
    case P_GOTOSTAND:
    {
      _addLinearLift(mLiftToStand, mLiftToStand, 4, 0, 0, 4);
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
    case P_GOFORWARDSLOW:
    {
      Serial.println(" P_GOFORWARDSLOW ");
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
