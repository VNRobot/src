/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Robot legs motion patterns
*/

// leg timing phase
struct phase {
  unsigned char FL;
  unsigned char FR;
  unsigned char RL;
  unsigned char RR;
};

// main counter
unsigned char mainCounter = 0;
// leg values for 4 legs
allLegs legsValue = {125, 0, 125, 0, 125, 0, 125, 0};
// leg phase
phase legPhase = {0, 0, 0, 0};
// walking mode
bool walkingMode = false;
// speed
char speed = 0;
char speedL = 0;
char speedR = 0;
// touch map
//char mTouchFlag[36]     = { 0, 0,  0,  0,  0,  2,  2,  1,  1, 1, 1, 1, 1, 1, 1, 2, 0, 0, 0, 0, 0, 0, 0, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 0, 0};
// walk and lift
char mLiftHFlag[36]     = { 3, 3,  3,  3,  2,  1,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 3, 3};
char mPointWalk[36]     = { 0,-5,-10,-15,-14,-13,-12,-11,-10,-9,-8,-7,-6,-5,-4,-3,-2,-1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,10, 5};
short mRecoverDown[36] = {100, 70, 50, 40, 30, 30,  30,  30,  30,  30,  30,  30,  30,  30,  30,  30,  30,  30,  30,  30, 30, 30, 30, 30, 30, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 125};
short mRecoverUp[36]   = {100, 80, 80, 80, 80, 80, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 90, 100, 110, 120, 125};
// leg speed const
char speedConst = 2;
// pattern item buffer
unsigned char taskItemBuffer = P_DOSTAND;

// get next sequence, mode and speed
void setPattern(unsigned char currentPattern, char angleTurn) {
  taskItemBuffer = currentPattern;
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

// update servo motors values
unsigned char updateCountPatterns(void) {
  // update sequence shift 
  mainCounter ++;
  if (mainCounter >= m_fullCycle) {
    mainCounter = 0;
  }
  legPhase.FL = mainCounter;
  legPhase.FR = mainCounter + m_halfCycle;
  if (legPhase.FR >= m_fullCycle) {
    legPhase.FR -= m_fullCycle;
  }
  legPhase.RL = legPhase.FR;
  legPhase.RR = legPhase.FL;
  return mainCounter;
}

// get servo motor steps
allLegs getWalkPatterns(void) {
  leg legSetL;
  leg legSetR;
  switch (taskItemBuffer) {
    case P_STANDTOGO:
    case P_GOTOSTAND:
    {
      legSetL.hight = m_defaultHight;
      legSetL.shift = 0;
      legSetR.hight = m_defaultHight;
      legSetR.shift = 0;
      legsValue.fl.hight = legSetL.hight;
      legsValue.fl.shift = legSetL.shift;
      legsValue.fr.hight = legSetR.hight;
      legsValue.fr.shift = legSetR.shift;
      legsValue.rl.hight = legSetL.hight;
      legsValue.rl.shift = legSetL.shift;
      legsValue.rr.hight = legSetR.hight;
      legsValue.rr.shift = legSetR.shift;
    }
    break;
    case P_RECOVERLEFT:
    {
      legSetL.hight = mRecoverDown[mainCounter];
      legSetL.shift = 0;
      legSetR.hight = mRecoverUp[mainCounter];
      legSetR.shift = 0;
      legsValue.fl.hight = legSetL.hight;
      legsValue.fl.shift = legSetL.shift;
      legsValue.fr.hight = legSetR.hight;
      legsValue.fr.shift = legSetR.shift;
      legsValue.rl.hight = legSetL.hight;
      legsValue.rl.shift = legSetL.shift;
      legsValue.rr.hight = legSetR.hight;
      legsValue.rr.shift = legSetR.shift;
    }
    break;
    case P_RECOVERRIGHT:
    {
      legSetL.hight = mRecoverUp[mainCounter];
      legSetL.shift = 0;
      legSetR.hight = mRecoverDown[mainCounter];
      legSetR.shift = 0;
      legsValue.fl.hight = legSetL.hight;
      legsValue.fl.shift = legSetL.shift;
      legsValue.fr.hight = legSetR.hight;
      legsValue.fr.shift = legSetR.shift;
      legsValue.rl.hight = legSetL.hight;
      legsValue.rl.shift = legSetL.shift;
      legsValue.rr.hight = legSetR.hight;
      legsValue.rr.shift = legSetR.shift;
    }
    break;
    default:
    {
      legSetL = _getWalkStep(legPhase.FL, speedL);
      legsValue.fl.hight = legSetL.hight;
      legsValue.fl.shift = legSetL.shift;
      legSetR = _getWalkStep(legPhase.FR, speedR);
      legsValue.fr.hight = legSetR.hight;
      legsValue.fr.shift = legSetR.shift;
      legSetL = _getWalkStep(legPhase.RL, speedL);
      legsValue.rl.hight = legSetL.hight;
      legsValue.rl.shift = legSetL.shift;
      legSetR = _getWalkStep(legPhase.RR, speedR);
      legsValue.rr.hight = legSetR.hight;
      legsValue.rr.shift = legSetR.shift;
    }
    break;
  }
  return legsValue;
}

// set walk step value
leg _getWalkStep(unsigned char counter, char speedValue) {
  short lift = 0;
  short shift = 0;
  // motors values for 2 motors
  leg legStep = {0, 0};
  if (walkingMode) {
    switch (mLiftHFlag[counter]) {
      case 0:
      {
        lift = 0;
      }
      break;
      case 1:
      {
        lift = 5;
      }
      break;
      case 2:
      {
        lift = 15;
      }
      break;
      case 3:
      {
        if (speedValue == 2) {
        lift = 35;
        } else {
        lift = 25;
        }
      }
      break;
      default:
      break;
    }
    switch (speedValue) {
      case -1:
      {
        shift =  - mPointWalk[counter];
      }
      break;
      case 1:
      {
        shift = mPointWalk[counter];
      }
      break;
      case 2:
      {
        shift = mPointWalk[counter] * 2;
      }
      break;
      default:
      break;
    }
    legStep.hight = m_defaultHight - lift;
    legStep.shift = shift * speedConst;
  } else {
    legStep.hight = m_defaultHight;
    legStep.shift = 0;
  }
  return legStep;
}
