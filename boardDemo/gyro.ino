/*
Board Lowrie 1.1 demo code
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Uses MPU6050 gyroscope and accelerometer
 - Gets yaw value
 - Gets pitch value
 - Gets roll value
*/

#include <Wire.h>

// acceleration
float accAngleX;
float accAngleY;
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
// gyro data yaw and pitch
int yawLast = 0;
int pitchLast = 0;
// buffers to read register into
float floatBuffer[3];
// loop variable
unsigned short i;

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
void readGyro(void) {
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

// reset gyro data
void resetGyro(void) {
  gyroAngleX = 0;
  gyroAngleY = 0;
  yaw = 0;
  yawLast = 0;
  pitchLast = 0;
}

// get horizontal direction
int getDirectionGyro(void) {
  // positive - turned left
  // negative - turned right
  return yawLast;
}

// correct horizontal direction
void updateDirectionGyro(int directionData) {
  // positive - turned left
  // negative - turned right
  yaw += directionData;
}

// get nose dive value
int getPitchGyro(void) {
  // positive - nose down
  // negative - nose up
  return pitchLast;
}

// get roll value
int getRollGyro(void) {
  // positive - roll right
  // negative - roll left
  return rollAngle;
}
