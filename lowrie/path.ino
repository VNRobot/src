/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Robot walking path
*/

// robot size devider
#define ROBOT_SIZE_DEVIDER       2

// enable steps distance count
bool stepsDistanceCountEnable = false;
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
void updatePath(short direction) {
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
  // step turning
  m_robotState.speedLNow = _turnStep(-direction, speedAbsolute);
  m_robotState.speedRNow = _turnStep(direction, speedAbsolute);
  m_robotState.forwardNow = walkFrward;
  if (stepsDistanceCountEnable) {
    // step size
    short stepSize = ((SERVO_HALF_CYCLE - (2 + speedAbsolute))* speedAbsolute * m_robotState.speedMuliplierNow) / ROBOT_SIZE_DEVIDER;
    // update distance to target
    if (distanceToTarget > stepSize) {
      distanceToTarget -= stepSize;
    } else {
      distanceToTarget = 0;
    }
  }
}

// set distance to target in cm
void setDistancePath(short distance) {
  distanceToTarget = distance * 10;
}

// update distance to target in cm
void updateDistancePath(short distance) {
  distanceToTarget += distance * 10;
}

// get distance to target in cm
short getDistancePath(void) {
  return distanceToTarget / 10;
}
