/*
Walking Hexapod Robot
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Robot legs motion patterns
*/

// points to currentSequence for every leg
unsigned char mainCounter = 0;
// lifting pattern
char m1Lift[32]       = {-4,-4, -4, -4, -4,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-4,-4};
char m2Lift[32]       = {-4,-4, -4, -4,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-4,-4,-4};
char mLiftToGo[32]    = { 0, 0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-1,-2,-4};
char mLiftToStand[32] = {-4,-2, -1,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
// walking pointers
char mWalk[32]        = { 0,-5,-10,-13,-12,-11,-10,-9,-8,-7,-6,-5,-4,-3,-2,-1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,10, 5};
// recovering patterns
char mRecoverLiftDown[32] = {-15,-45,-75,-75,-75,-75,-75,-75,-75,-75,-75,-75,-75,-75,-75,-75,-75,-75,-75,-75,-75,-75,-75,-75,-75,-75,-75,-75,-75,-75,-15, 0};
char mRecoverLiftUp[32]   = { 15, 35, 45, 45, 45, 45,-45,-45,-45,-45,-45,-45,-45,-45,-45,-45,-45,-45,-45,-45,-45,-45,-45,-45,-45,-45,-45,-45,-45,-45,-15, 0};

// motors values for 10 motors
allMotors motorValue = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
allMotors motorLift = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
// leg timing phase
unsigned char counterFL = 0;
unsigned char counterFR = 0;
unsigned char counterSL = 0;
unsigned char counterSR = 0;
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
void _addLinearLift(char * seftSide, char * rightSide, char FL, char FR, char SL, char SR, char RL, char RR) {
  motorLift.m.fl.motor1 += seftSide[mainCounter] * FL;
  motorLift.m.fl.motor2 += seftSide[mainCounter] * FL;
  motorLift.m.fr.motor1 += rightSide[mainCounter] * FR;
  motorLift.m.fr.motor2 += rightSide[mainCounter] * FR;
  motorLift.m.sl.motor1 += seftSide[mainCounter] * SL;
  motorLift.m.sl.motor2 += seftSide[mainCounter] * SL;
  motorLift.m.sr.motor1 += rightSide[mainCounter] * SR;
  motorLift.m.sr.motor2 += rightSide[mainCounter] * SR;
  motorLift.m.rl.motor1 += seftSide[mainCounter] * RL;
  motorLift.m.rl.motor2 += seftSide[mainCounter] * RL;
  motorLift.m.rr.motor1 += rightSide[mainCounter] * RR;
  motorLift.m.rr.motor2 += rightSide[mainCounter] * RR;
}

// update servo motors values
unsigned char updateCountPatterns(void) {
  // update sequence shift 
  mainCounter ++;
  if (mainCounter >= m_fullCycle) {
    mainCounter = 0;
  }
  // FL
  counterFL = mainCounter + m_halfCycle + m_halfCycle / 2;
  if (counterFL >= m_fullCycle) {
    counterFL -= m_fullCycle;
  }
  // FR
  counterFR = mainCounter + m_halfCycle / 2;
  if (counterFR >= m_fullCycle) {
    counterFR -= m_fullCycle;
  }
  // SL
  counterSL = mainCounter + m_halfCycle / 4;
  if (counterSL >= m_fullCycle) {
    counterSL -= m_fullCycle;
  }
  // SR
  counterSR = mainCounter + m_halfCycle + m_halfCycle / 4;
  if (counterSR >= m_fullCycle) {
    counterSR -= m_fullCycle;
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
    {
      speedL = m_speedPatternValue / 2;
      speedR = m_speedPatternValue;
    }
    break;
    case P_GORIGHT:
    {
      speedL = m_speedPatternValue;
      speedR = m_speedPatternValue / 2;
    }
    break;
    case P_GOFORWARD:
    {
      if (angleTurn > 2) {
        speedL = m_speedPatternValue;
        speedR = m_speedPatternValue / 2;
      } else if (angleTurn < -2) {
        speedL = m_speedPatternValue / 2;
        speedR = m_speedPatternValue;
      } else {
        speedL = m_speedPatternValue;
        speedR = m_speedPatternValue;
      }
    }
    break;
    case P_GOFORWARDSLOW:
    {
      if (angleTurn > 2) {
        speedL = 1;
        speedR = 0;
      } else if (angleTurn < -2) {
        speedL = 0;
        speedR = 1;
      } else {
        speedL = 1;
        speedR = 1;
      }
    }
    break;
    case P_STANDGO:
    {
      if (angleTurn > 2) {
        speedL = 1;
        speedR = -1;
      } else if (angleTurn < -2) {
        speedL = -1;
        speedR = 1;
      } else {
        speedL = 0;
        speedR = 0;
      }
    }
    case P_GOBACK:
    {
      speedL = -1;
      speedR = -1;
    }
    case P_GOBACKLEFT:
    {
      speedL = -1;
      speedR = 0;
    }
    case P_GOBACKRIGHT:
    {
      speedL = 0;
      speedR = -1;
    }
    case P_STANDGOLEFT:
    {
      speedL = -1;
      speedR = 1;
    }
    break;
    case P_STANDGORIGHT:
    {
      speedL = 1;
      speedR = -1;
    }
    break;
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
  motorValue.m.fl.motor1 = mWalk[counterFL] * speedL;
  motorValue.m.fl.motor2 = mWalk[counterFL] * speedL;
  motorValue.m.fr.motor1 = mWalk[counterFR] * speedR;
  motorValue.m.fr.motor2 = mWalk[counterFR] * speedR;
  motorValue.m.sl.motor1 = mWalk[counterSL] * speedL;
  motorValue.m.sl.motor2 = mWalk[counterSL] * speedL;
  motorValue.m.sr.motor1 = mWalk[counterSR] * speedR;
  motorValue.m.sr.motor2 = mWalk[counterSR] * speedR;
  motorValue.m.rl.motor1 = mWalk[counterRL] * speedL;
  motorValue.m.rl.motor2 = mWalk[counterRL] * speedL;
  motorValue.m.rr.motor1 = mWalk[counterRR] * speedR;
  motorValue.m.rr.motor2 = mWalk[counterRR] * speedR;
  return motorValue;
}

// get servo motors values for lift
allMotors getLiftPatterns(void) {
  // set motors angle values
  motorLift.m.fl.motor1 = m1Lift[counterFL] * walkLiftHight;
  motorLift.m.fl.motor2 = m2Lift[counterFL] * walkLiftHight;
  motorLift.m.fr.motor1 = m1Lift[counterFR] * walkLiftHight;
  motorLift.m.fr.motor2 = m2Lift[counterFR] * walkLiftHight;
  motorLift.m.sl.motor1 = m1Lift[counterSL] * walkLiftHight;
  motorLift.m.sl.motor2 = m2Lift[counterSL] * walkLiftHight;
  motorLift.m.sr.motor1 = m1Lift[counterSR] * walkLiftHight;
  motorLift.m.sr.motor2 = m2Lift[counterSR] * walkLiftHight;
  motorLift.m.rl.motor1 = m1Lift[counterRL] * walkLiftHight;
  motorLift.m.rl.motor2 = m2Lift[counterRL] * walkLiftHight;
  motorLift.m.rr.motor1 = m1Lift[counterRR] * walkLiftHight;
  motorLift.m.rr.motor2 = m2Lift[counterRR] * walkLiftHight;
  // add linear lift
  switch (taskItemBuffer) {
    case P_STANDTOGO:
    {
      _addLinearLift(mLiftToGo, mLiftToGo, 0, 0, 0, 0, 0, 4);
    }
    break;
    case P_GOTOSTAND:
    {
      _addLinearLift(mLiftToStand, mLiftToStand, 0, 0, 0, 0, 0, 4);
    }
    break;
    case P_RECOVERLEFT:
    {
      _addLinearLift(mRecoverLiftDown, mRecoverLiftUp, 1, 1, 1, 1, 1, 1);
    }
    break;
    case P_RECOVERRIGHT:
    {
      _addLinearLift(mRecoverLiftUp ,mRecoverLiftDown, 1, 1, 1, 1, 1, 1);
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
