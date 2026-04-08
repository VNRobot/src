/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Robot walking path
*/

// speed relative value from 0 to 2
char speedAbsolute = 0;
// walking direction
bool walkFrward = true;
// distance to the target mm
short distanceToTarget = 0;

/*
uses
  m_robotState
output
  m_robotState.speedLNow
  m_robotState.speedRNow
  m_robotState.forwardNow
*/

// set distance to target in cm
void setDistancePathZero(short distance) {
  distanceToTarget = distance * 10;
}

// update distance to target in cm
void updateDistancePathZero(short distance) {
  distanceToTarget += distance * 10;
}

// get distance to target in cm
short getDistancePathZero(void) {
  return distanceToTarget / 10;
}

// step rotation
char _turnStep(short direction, char speed) {
  if (walkFrward) {
    // forward
    if (direction > 4) {
      if (speed < 2) {
        speed ++;
      }
    }
    if (direction < -4) {
      if (speed > 0) {
        speed --;
      }
    }
  } else {
    // backward
    if (direction < -4) {
      if (speed < 2) {
        speed ++;
      }
    }
    if (direction > 4) {
      if (speed > 0) {
        speed --;
      }
    }
  }
  return speed;
}

// get speed
void updatePathZero(short direction) {
  // walking mode
  if (distanceToTarget > 0) {
    // plan to go
    if ((direction < 30) && (direction > -30)) {
      // go forward
      if (speedAbsolute < 1) {
        speedAbsolute = 1;
      } else {
        speedAbsolute = 2;
      }
      walkFrward = true;
    } else if ((direction < 150) && (direction > -150)) {
      // stand and turn
      speedAbsolute = 0;
      walkFrward = true;
    } else {
      // go back
      if (speedAbsolute < 1) {
        speedAbsolute = 1;
      } else {
        speedAbsolute = 2;
      }
      walkFrward = false;
    }
  } else {
    // arrived to the destnation
    speedAbsolute = 0;
    walkFrward = true;
  }
  // uncomment for step turning
  m_robotState.speedLNow = speedAbsolute; //_turnStep(-direction, speedAbsolute);
  m_robotState.speedRNow = speedAbsolute; //_turnStep(direction, speedAbsolute);
  m_robotState.forwardNow = walkFrward;
  // step size
  short stepSize = (SERVO_HALF_CYCLE - (2 + speedAbsolute))* speedAbsolute * m_robotState.speedMuliplierNow;
  // update distance to target
  if (distanceToTarget > stepSize) {
    distanceToTarget -= stepSize;
  } else {
    distanceToTarget = 0;
  }
}
