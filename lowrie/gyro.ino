/*
Wlking Robot Lowrie
Licended GNU GPLv3 2023
Arduino nano
use MPU6050 to read gyroscope and accelerometer
*/

#include <Wire.h>

float AccErrorX = 0;
float AccErrorY = 0;
float GyroErrorX = 0;
float GyroErrorY = 0;
float GyroErrorZ = 0;
unsigned long currentTime = 0;
float gyroAngleX = 0;
float gyroAngleY = 0;
float yaw = 0;
int yawData = 0;
int pitchData = 0;
int rollMin = 0;
int rollMax = 0;
unsigned char rollMinTime = 0;
unsigned char rollMaxTime = 0;
unsigned long ulongBuffer;
float floatBuffer[3];
float floatBufferX;
float floatBufferY;
float floatBufferZ;
// variable for temporary use
int intBuffer;
// shift forward balance value
char forwardShift = 0;

// init gyroscope wire
void _initWire(void) {
  Wire.begin();                      
  Wire.beginTransmission(0x68);
  Wire.write(0x6B);
  Wire.write(0x00);
  Wire.endTransmission(true);
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
  delay(100);
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
void updateGyroData(unsigned char sequenceCount) {
  // accelerometer
  _readWire(floatBuffer, 0x3B);
  floatBuffer[0] /= 16384.0;
  floatBuffer[1] /= 16384.0;
  floatBuffer[2] /= 16384.0;
  floatBufferX = (atan(floatBuffer[1] / sqrt(pow(floatBuffer[0], 2) + pow(floatBuffer[2], 2))) * 180 / PI) - AccErrorX;
  floatBufferY = (atan(-1 * floatBuffer[0] / sqrt(pow(floatBuffer[1], 2) + pow(floatBuffer[2], 2))) * 180 / PI) - AccErrorY;
  // gyroscope
  ulongBuffer = currentTime;
  currentTime = millis();
  floatBufferZ = (currentTime - ulongBuffer) / 1000.0;
  _readWire(floatBuffer, 0x43);
  floatBuffer[0] = floatBuffer[0] / 131.0 - GyroErrorX;
  floatBuffer[1] = floatBuffer[1] / 131.0 - GyroErrorY;
  floatBuffer[2] = floatBuffer[2] / 131.0 - GyroErrorZ;
  gyroAngleX += floatBuffer[0] * floatBufferZ;
  gyroAngleY += floatBuffer[1] * floatBufferZ;
  yaw        += floatBuffer[2] * floatBufferZ;
  // if end of sycle
  if (sequenceCount == 15) {
    // robot walking direction value
    // positive - turned left
    // negative - turned right
    yawData = (int)yaw;
    // robot pitch value
    // positive - nose down
    // negative - nose up
    pitchData = (int)(0.96 * gyroAngleY + 0.04 * floatBufferY);
  }
  // robot side roll value
  // positive - rotated right
  // negative - rotated left
  if (sequenceCount == 0) {
    rollMin = 1000;
    rollMax = -1000;
    rollMinTime = 0;
    rollMaxTime = 0;
  }
  // find max and min roll
  intBuffer = (int)(0.96 * gyroAngleX + 0.04 * floatBufferX);
  if (intBuffer > rollMax) {
    rollMax = intBuffer;
    rollMaxTime = sequenceCount;
  }
  if (intBuffer < rollMin) {
    rollMin = intBuffer;
    rollMinTime = sequenceCount;
  }
}

// reset gyro data
void resetGyro(void) {
  gyroAngleX = 0;
  gyroAngleY = 0;
  yaw = 0;
  yawData = 0;
  pitchData = 0;
}

// get walking direction
int getWalkingDirectionGyro(void) {
  // positive - turned left
  // negative - turned right
  return yawData;
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
  intBuffer = yawData / 2;
  if (intBuffer > 10) {
    intBuffer = 10;
  }
  if (intBuffer < -10) {
    intBuffer = -10;
  }
  return (char)intBuffer;
}

// get nose dive value
int getNoseDiveGyro(void) {
  // positive - nose down
  // negative - nose up
  return pitchData;
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
  if (rollMax - rollMin > 90) {
    return false;
  }
  return true;
}

// fix balance using gyro
int fixBalanceGyro(int center) {
  //Serial.print(" Gyro ");
  //Serial.print((int)rollMin);
  //Serial.print(" ");
  //Serial.print((int)rollMax);
  //Serial.print(" ");
  //Serial.print((int)rollMinTime);
  //Serial.print(" ");
  //Serial.println((int)rollMaxTime);
  // balance
  if (rollMax - rollMin > 2) {
    // body rolls
    if ((rollMinTime < 8) && (rollMaxTime > 7)) {
      // front is too heavy
      // increase weight on rear
      if (forwardShift < 4) {
        forwardShift += 2;
        Serial.println(" increase weight on rear");
      } else {
        if(center > 9) {
          center -= 1;
          forwardShift = -4;
        } else {
          Serial.println(" limit weight on rear");
        }
      }
    }
    if ((rollMinTime > 7) && (rollMaxTime < 8)) {
      // rear is too heavy
      //increase wight on front
      if (forwardShift > -4) {
        forwardShift -= 2;
        Serial.println(" increase weight on front");
      } else {
        if(center < 13) {
          center += 1;
          forwardShift = 4;
        } else {
          Serial.println(" limit weight on front");
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
    Serial.println((int)center);
  } else {
    Serial.println(" balanced");
    // reset to fix drift
    gyroAngleX = 0;
  }
  return center;
}
