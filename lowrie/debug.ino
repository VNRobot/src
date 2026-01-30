/*
Walking Robot TurtleV1
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Debug file
*/
/*
// print task  name
void printTaskNameDebug(unsigned char taskNow) {
  Serial.println(F(" "));
  switch (taskNow) {
    case BEGIN_TASK:
      Serial.println(F(" BEGIN_TASK "));
    break;
    case DOWN_TASK:
      Serial.println(F(" DOWN_TASK "));
    break;
    case GOBACKLEFT_TASK:
      Serial.println(F(" GOBACKLEFT_TASK "));
    break;
    case GOBACKRIGHT_TASK:
      Serial.println(F(" GOBACKRIGHT_TASK "));
    break;
    case GOTURNRIGHT_TASK:
      Serial.println(F(" GOTURNRIGHT_TASK "));
    break;
    case GOTURNLEFT_TASK:
      Serial.println(F(" GOTURNLEFT_TASK "));
    break;
    case STANDTURNRIGHT_TASK:
      Serial.println(F(" STANDTURNRIGHT_TASK "));
    break;
    case STANDTURNLEFT_TASK:
      Serial.println(F(" STANDTURNLEFT_TASK "));
    break;
    case STANDTURNRIGHT2_TASK:
      Serial.println(F(" STANDTURNRIGHT2_TASK "));
    break;
    case STANDTURNLEFT2_TASK:
      Serial.println(F(" STANDTURNLEFT2_TASK "));
    break;
    case GO_TASK:
      Serial.println(F(" GO_TASK "));
    break;
    case STANDGO_TASK:
      Serial.println(F(" STANDGO_TASK "));
    break;
    case STAND_TASK:
      Serial.println(F(" STAND_TASK "));
    break;
    case RECOVER_LEFT_TASK:
      Serial.println(F(" RECOVER_LEFT_TASK "));
    break;
    case RECOVER_RIGHT_TASK:
      Serial.println(F(" RECOVER_RIGHT_TASK "));
    break;
    case FLIP_TASK:
      Serial.println(F(" FLIP_TASK "));
    break;
    case DEFAULT_TASK:
      Serial.println(F(" DEFAULT_TASK "));
    break;
    default:
      Serial.print(F(" unknown task "));
      Serial.print((int)taskNow);
      Serial.println(F(" "));
    break;
  }
}

// print task  name
void printPatternNameDebug(unsigned char patternNow) {
  switch (patternNow) {
    case P_DOSTAND:
      Serial.print(F(" P_DOSTAND "));
    break;
    case P_STANDTOGO:
      Serial.print(F(" P_STANDTOGO "));
    break;
    case P_GOTOSTAND:
      Serial.print(F(" P_GOTOSTAND "));
    break;
    case P_STANDGO:
      Serial.print(F(" P_STANDGO "));
    break;
    case P_STANDGOLEFT:
      Serial.print(F(" P_STANDGOLEFT "));
    break;
    case P_STANDGORIGHT:
      Serial.print(F(" P_STANDGORIGHT "));
    break;
    case P_GOFORWARD:
      Serial.print(F(" P_GOFORWARD "));
    break;
    case P_GOLEFT:
      Serial.print(F(" P_GOLEFT "));
    break;
    case P_GORIGHT:
      Serial.print(F(" P_GORIGHT "));
    break;
    case P_GOBACK:
      Serial.print(F(" P_GOBACK "));
    break;
    case P_GOBACKLEFT:
      Serial.print(F(" P_GOBACKLEFT "));
    break;
    case P_GOBACKRIGHT:
      Serial.print(F(" P_GOBACKRIGHT "));
    break;
    case P_DOLOW:
      Serial.print(F(" P_DOLOW "));
    break;
    case P_DODOWN:
      Serial.print(F(" P_DODOWN "));
    break;
    case P_RECOVERLEFT:
      Serial.print(F("P_RECOVERLEFT  "));
    break;
    case P_RECOVERRIGHT:
      Serial.print(F(" P_RECOVERRIGHT "));
    break;
    case P_DOFLIP:
      Serial.print(F(" P_DOFLIP "));
    break;
    case P_DONE:
      Serial.print(F(" P_DONE "));
    break;
    case P_RESETDIRECTION:
      Serial.print(F(" P_RESETDIRECTION "));
    break;
    case P_RESTOREDIRECTION:
      Serial.print(F(" P_RESTOREDIRECTION "));
    break;
    case P_RESETGIRO:
      Serial.print(F(" P_RESETGIRO "));
    break;
    case P_BENDSTART:
      Serial.print(F(" P_BENDSTART "));
    break;
    case P_CRAWLSTART:
      Serial.print(F(" P_CRAWLSTART "));
    break;
    case P_SWIMSTART:
      Serial.print(F(" P_SWIMSTART "));
    break;
    case P_INOSTART:
      Serial.print(F(" P_INOSTART "));
    break;
    case P_NORMALSTART:
      Serial.print(F(" P_NORMALSTART "));
    break;
    case P_REPEAT:
      Serial.print(F(" P_REPEAT "));
    break;
    case P_GETCURRENT:
      Serial.print(F(" P_GETCURRENT "));
    break;
    case P_SETPRIORITY_HIGH:
      Serial.print(F(" P_SETPRIORITY_HIGH "));
    break;
    case P_SETPRIORITY_NORM:
      Serial.print(F(" P_SETPRIORITY_NORM "));
    break;
    case P_SETPRIORITY_LOW:
      Serial.print(F(" P_SETPRIORITY_LOW "));
    break;
    case P_END:
      Serial.print(F(" P_END "));
    break;
    default:
      Serial.print(F(" unknown pattern "));
    break;
  }
}

// print sensor inputs state
void printInputsDebug(void) {
  // print input state
  switch (m_robotState.inputStateNow) {
    case IN_WALL_FRONTLEFT:
      Serial.println(F(" IN_WALL_FRONTLEFT "));
    break;
    case IN_WALL_FRONTRIGHT:
      Serial.println(F(" IN_WALL_FRONTRIGHT "));
    break;
    case IN_WALL_LEFT:
      Serial.println(F(" IN_WALL_LEFT "));
    break;
    case IN_WALL_RIGHT:
      Serial.println(F(" IN_WALL_RIGHT "));
    break;
    case IN_OBSTACLE_FRONTLEFT:
      Serial.println(F(" IN_OBSTACLE_FRONTLEFT "));
    break;
    case IN_OBSTACLE_FRONTRIGHT:
      Serial.println(F(" IN_OBSTACLE_FRONTRIGHT "));
    break;
    case IN_OBSTACLE_LEFT:
      Serial.println(F(" IN_OBSTACLE_LEFT "));
    break;
    case IN_OBSTACLE_RIGHT:
      Serial.println(F(" IN_OBSTACLE_RIGHT "));
    break;
    case IN_NORMAL:
      Serial.println(F(" IN_NORMAL "));
    break;
    default:
      Serial.println(F(" Wrong inputs state "));
  }
}

// print current state
void printCurrentStateDebug(void) {
  // print input state
  switch (m_robotState.currentStateNow) {
    case C_LOW_BATTERY:
      Serial.print(F(" C_LOW_BATTERY "));
    break;
    case C_HIGH_CURRENT:
      Serial.print(F(" C_HIGH_CURRENT "));
    break;
    case C_LOW_CURRENT:
      Serial.print(F(" C_LOW_CURRENT "));
    break;
    case C_NORMAL:
      Serial.print(F(" C_NORMAL "));
    break;
    default:
      Serial.println(F(" Wrong inputs state "));
  }
}

// print gyro values
void printLineGyroDebug(void) {
  Serial.print(" aRoll ");
  Serial.print(m_gyroState.accRollX);
  Serial.print(" aPitch ");
  Serial.print(m_gyroState.accPitchY);
  Serial.print(" gRoll ");
  Serial.print(m_gyroState.gyroRollX);
  Serial.print(" gPitch ");
  Serial.print(m_gyroState.gyroPitchY);
  Serial.print(" direction ");
  Serial.println(m_gyroState.direction);
}

// print gyro values
void printRollGyroDebug(void) {
  Serial.print(" rollMin ");
  Serial.print(m_gyroState.rollMin);
  Serial.print(" rollMax ");
  Serial.print(m_gyroState.rollMax);
  Serial.print(" rollMinTime ");
  Serial.print(m_gyroState.rollMinTime);
  Serial.print(" rollMaxTime ");
  Serial.println(m_gyroState.rollMaxTime);
}
*/