/*
Wlking Robot Lowrie
Licended GNU GPLv3 2023
Arduino nano
use MPU6050 to read gyroscope and accelerometer
*/

#include <Wire.h>
// acceleration
float accAngleX;
float accAngleY;
float accAngleYold = 0;
// acceleration errors
float AccErrorX = 0;
float AccErrorY = 0;
// gyro
float GyroErrorX = 0;
float GyroErrorY = 0;
float GyroErrorZ = 0;
// time parameters
float timeInterval;
unsigned long currentTime = 0;
unsigned long oldTime;
// Rotation angles
float gyroAngleX = 0;
float gyroAngleY = 0;
float yaw = 0;
int rollAngle;
int directionAngle;
// gyro data at the end of walk cycle
int yawLast = 0;
int pitchLast = 0;
// roll data inside walk cycle
int rollMin = 0;
int rollMax = 0;
unsigned char rollMinTime = 0;
unsigned char rollMaxTime = 0;
// buffers to read register into
float floatBuffer[3];
// shift forward walking balance value
char forwardShift = 0;
// deceleration value
float deceleration = 0;

// init gyroscope wire
void _initWire(void) {
  Wire.begin();                      
  delay(100);
  Wire.beginTransmission(0x68);
  Wire.write(0x6B);
  Wire.write(0b10001000); // DEVICE_RESET, TEMP_DIS.
  Wire.endTransmission();
  delay(100);
  Wire.beginTransmission(0x68);
  Wire.write(0x68);
  Wire.write(0b00000111); // GYRO_RESET, ACCEL_RESET, TEMP_RESET.
  Wire.endTransmission();
  delay(100);
  Wire.beginTransmission(0x68);
  Wire.write(0x6B);
  Wire.write(0b00001000); // SLEEP = 0, TEMP_DIS = 1.
  Wire.endTransmission();
  delay(100);
  Wire.beginTransmission(0x68);
  Wire.write(0x1B);
  Wire.write(0b00001000); // +/-500 deg/s.
  Wire.endTransmission();
  delay(100);
}

// read gyroscope wire
void _readWire(float * regData, unsigned char code) {
  Wire.beginTransmission(0x68);
  Wire.write(code);
  Wire.endTransmission(false);
  Wire.requestFrom(0x68, 6, true);
  regData[0] = Wire.read() << 8 | Wire.read();
  regData[1] = Wire.read() << 8 | Wire.read();
  regData[2] = Wire.read() << 8 | Wire.read();
}

// init gyroscope and accelerometer MPU6050 using I2C
void initGyro() {
  _initWire();
  // calculate gyro errors
  for (i = 0; i < 200; i++) {
    _readWire(floatBuffer, 0x3B);
    floatBuffer[0] /= 16384.0;
    floatBuffer[1] /= 16384.0;
    floatBuffer[2] /= 16384.0;
    AccErrorX += atan((floatBuffer[1]) / sqrt(pow((floatBuffer[0]), 2) + pow((floatBuffer[2]), 2))) * 180 / PI;
    AccErrorY += atan(-1 * (floatBuffer[0]) / sqrt(pow((floatBuffer[1]), 2) + pow((floatBuffer[2]), 2))) * 180 / PI;
  }
  AccErrorX /= 200;
  AccErrorY /= 200;
  for (i = 0; i < 200; i++) {
    _readWire(floatBuffer, 0x43);
    floatBuffer[0] /= 131.0;
    floatBuffer[1] /= 131.0;
    floatBuffer[2] /= 131.0;
    GyroErrorX += floatBuffer[0];
    GyroErrorY += floatBuffer[1];
    GyroErrorZ += floatBuffer[2];
  }
  GyroErrorX /= 200;
  GyroErrorY /= 200;
  GyroErrorZ /= 200;
}

// read gyroscope and accelerometer data
void updateGyro(unsigned char sequenceCount) {
  // accelerometer
  _readWire(floatBuffer, 0x3B);
  floatBuffer[0] /= 16384.0;
  floatBuffer[1] /= 16384.0;
  floatBuffer[2] /= 16384.0;
  accAngleX = (atan(floatBuffer[1] / sqrt(pow(floatBuffer[0], 2) + pow(floatBuffer[2], 2))) * 180 / PI) - AccErrorX;
  accAngleY = (atan(-1 * floatBuffer[0] / sqrt(pow(floatBuffer[1], 2) + pow(floatBuffer[2], 2))) * 180 / PI) - AccErrorY;
  // gyroscope
  oldTime = currentTime;
  currentTime = millis();
  timeInterval = (currentTime - oldTime) / 1000.0;
  _readWire(floatBuffer, 0x43);
  floatBuffer[0] = floatBuffer[0] / 131.0 - GyroErrorX;
  floatBuffer[1] = floatBuffer[1] / 131.0 - GyroErrorY;
  floatBuffer[2] = floatBuffer[2] / 131.0 - GyroErrorZ;
  gyroAngleX += floatBuffer[0] * timeInterval;
  gyroAngleY += floatBuffer[1] * timeInterval;
  yaw        += floatBuffer[2] * timeInterval;
  rollAngle = (int)(0.96 * gyroAngleX + 0.04 * accAngleX);
  // walk cycle related operations
  if (sequenceCount == 0) {
    // start of walk cycle
    rollMin = rollAngle;
    rollMax = rollAngle;
    rollMinTime = 0;
    rollMaxTime = 0;
    deceleration = 0;
  } else {
    // find deceleration
    accAngleYold -= accAngleY;
    if (accAngleYold > 0) {
      deceleration += accAngleYold;
    } else if (accAngleYold < 0) {
      deceleration -= accAngleYold;
    }
    // find max and min roll
    if (rollAngle > rollMax) {
      rollMax = rollAngle;
      rollMaxTime = sequenceCount;
    }
    if (rollAngle < rollMin) {
      rollMin = rollAngle;
      rollMinTime = sequenceCount;
    }
  }
  accAngleYold = accAngleY;
  // if end of sycle
  if (sequenceCount == 15) {
    yawLast = (int)yaw;
    pitchLast = (int)(0.96 * gyroAngleY + 0.04 * accAngleY);
    // fix slow drift
    if (gyroAngleX > 0) {
      gyroAngleX --;
    } else if (gyroAngleX < 0) {
      gyroAngleX ++;
    }
    if (gyroAngleY > 0) {
      gyroAngleY --;
    } else if (gyroAngleY < 0) {
      gyroAngleY ++;
    }
  }
}

// reset gyro data
void resetGyro(void) {
  gyroAngleX = 0;
  gyroAngleY = 0;
  yaw = 0;
  yawLast = 0;
  pitchLast = 0;
}

// get deceleration event
bool getDecelerationGyro(void) {
  // make border value 100 
  if ((int)deceleration > 100) {
    return true;
  }
  return false;
}

// get walking direction
int getWalkingDirectionGyro(void) {
  // positive - turned left
  // negative - turned right
  return yawLast;
}

// correct walking direction
void updateWalkingDirectionGyro(int directionData) {
  // positive - turned left
  // negative - turned right
  yaw += directionData;
}

// get walking direction correction from gyroscope
char getDirectionCorrectionGyro(void) {
  // maximal direction correction is 10
  directionAngle = yawLast / 2;
  if (directionAngle > 10) {
    directionAngle = 10;
  }
  if (directionAngle < -10) {
    directionAngle = -10;
  }
  return (char)directionAngle;
}

// get nose dive value
int getNoseDiveGyro(void) {
  // positive - nose down
  // negative - nose up
  return pitchLast;
}

// get roll left
int getRollLeftGyro(void) {
  // negative - rotated left
  return rollMin;
}

// get roll right
int getRollRightGyro(void) {
  // positive - rotated right
  return rollMax;
}

// get roll left time
unsigned char getRollLeftTimeGyro(void) {
  // time 0 - 7 front is too heavy
  return rollMinTime;
}

// get roll right time
unsigned char getRollRightTimeGyro(void) {
  // time 8 - 15 front is too heavy
  return rollMaxTime;
}

// check if robot is in vertical position
bool checkVerticalPositionGyro(void) {
  // set angle as 60
  if ((rollMax > 45) || (rollMin < -45)) {
    return false;
  }
  return true;
}

// fix balance using gyro
int fixBalanceGyro(int center) {
  // balance
  if (rollMax - rollMin > 2) {
    // body rolls
    if ((rollMinTime < 8) && (rollMaxTime > 7)) {
      // front is too heavy
      // increase weight on rear
      if (forwardShift < 4) {
        forwardShift += 2;
      } else {
        if(center > 9) {
          center -= 1;
          forwardShift = -4;
        }
      }
    }
    if ((rollMinTime > 7) && (rollMaxTime < 8)) {
      // rear is too heavy
      //increase wight on front
      if (forwardShift > -4) {
        forwardShift -= 2;
      } else {
        if(center < 13) {
          center += 1;
          forwardShift = 4;
        }
      }
    }
    // update body balance
    m_bodyBalance.fl.motor1 = -forwardShift;
    m_bodyBalance.fl.motor2 = forwardShift;
    m_bodyBalance.fr.motor1 = -forwardShift;
    m_bodyBalance.fr.motor2 = forwardShift;
    m_bodyBalance.rl.motor1 = -forwardShift;
    m_bodyBalance.rl.motor2 = forwardShift;
    m_bodyBalance.rr.motor1 = -forwardShift;
    m_bodyBalance.rr.motor2 = forwardShift;
  }
  return center;
}
