/*
Simple Robot
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
use MPU6050 to read gyroscope and accelerometer
*/

#include <Wire.h>

// gyro data structure
typedef struct gyro {
  float accAngleX;
  float accAngleY;
  float gyroAngleX;
  float gyroAngleY;
  float yaw;
  int pitch;
  int roll;
} gyro;

// accelerometer data structure
typedef struct acc {
  int accAngleX;
  int accAngleY;
} acc;

// gyro data
gyro gyroData = {0, 0, 0, 0, 0, 0, 0};
acc accDataOld1 = {0, 0};
acc accDataOld2 = {0, 0};
acc accError = {0, 0};
acc accErrorAverage = {0, 0};
accRoll accAverageValue = {0, 0, 0, 0, 0, 0, GYRO_NORM};

// acceleration errors
float AccErrorX = 0;
float AccErrorY = 0;
// gyro errors
float GyroErrorX = 0;
float GyroErrorY = 0;
float GyroErrorZ = 0;
// time parameters
float timeInterval;
unsigned long currentTime = 0;
unsigned long oldTime;
// buffers to read register into
float floatBuffer[3];
float directionOld = 0;
unsigned char stateGyroOld = GYRO_NORM;

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
  Wire.beginTransmission(0x68);
  Wire.write(0x1A);
  Wire.write(0b00000010); // LPF
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
  int i;
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
accRoll updateGyro(unsigned char sequenceCount) {
  // accelerometer
  _readWire(floatBuffer, 0x3B);
  floatBuffer[0] /= 16384.0;
  floatBuffer[1] /= 16384.0;
  floatBuffer[2] /= 16384.0;
  gyroData.accAngleX = (atan(floatBuffer[1] / sqrt(pow(floatBuffer[0], 2) + pow(floatBuffer[2], 2))) * 180 / PI) - AccErrorX;
  gyroData.accAngleY = (atan(-1 * floatBuffer[0] / sqrt(pow(floatBuffer[1], 2) + pow(floatBuffer[2], 2))) * 180 / PI) - AccErrorY;
  // gyroscope
  oldTime = currentTime;
  currentTime = millis();
  timeInterval = (currentTime - oldTime) / 1000.0;
  _readWire(floatBuffer, 0x43);
  floatBuffer[0] = floatBuffer[0] / 131.0 - GyroErrorX;
  floatBuffer[1] = floatBuffer[1] / 131.0 - GyroErrorY;
  floatBuffer[2] = floatBuffer[2] / 131.0 - GyroErrorZ;
  gyroData.gyroAngleX += floatBuffer[0] * timeInterval;
  gyroData.gyroAngleY += floatBuffer[1] * timeInterval;
  gyroData.yaw        += floatBuffer[2] * timeInterval;
  gyroData.roll = (int)(0.96 * gyroData.gyroAngleX + 0.04 * gyroData.accAngleX);
  gyroData.pitch = (int)(0.96 * gyroData.gyroAngleY + 0.04 * gyroData.accAngleY);
  // fix slow drift
  if (sequenceCount == 0) {
    if (gyroData.gyroAngleX > 0) {
      gyroData.gyroAngleX --;
    } else if (gyroData.gyroAngleX < 0) {
      gyroData.gyroAngleX ++;
    }
    if (gyroData.gyroAngleY > 0) {
      gyroData.gyroAngleY --;
    } else if (gyroData.gyroAngleY < 0) {
      gyroData.gyroAngleY ++;
    }
  }
  // end fix slow drift
  if (accError.accAngleX > 0) {
    accErrorAverage.accAngleX += accError.accAngleX;
  } else {
    accErrorAverage.accAngleX -= accError.accAngleX;
  }
  accErrorAverage.accAngleX -= accErrorAverage.accAngleX / 4;
  if (accError.accAngleY > 0) {
    accErrorAverage.accAngleY += accError.accAngleY;
  } else {
    accErrorAverage.accAngleY -= accError.accAngleY;
  }
  accErrorAverage.accAngleY -= accErrorAverage.accAngleY / 4;
  //
  accError.accAngleX = (int)gyroData.accAngleX - accAverageValue.accAngleX;
  accError.accAngleY = (int)gyroData.accAngleY - accAverageValue.accAngleY;
  // 
  accAverageValue.accAngleX = ((int)gyroData.accAngleX + accDataOld1.accAngleX + accDataOld2.accAngleX + accAverageValue.accAngleX + 2) / 4;
  accAverageValue.accAngleY = ((int)gyroData.accAngleY + accDataOld1.accAngleY + accDataOld2.accAngleY + accAverageValue.accAngleY + 2) / 4;
  //
  accDataOld2.accAngleX = accDataOld1.accAngleX;
  accDataOld2.accAngleY = accDataOld1.accAngleY;
  //
  accDataOld1.accAngleX = (int)gyroData.accAngleX;
  accDataOld1.accAngleY = (int)gyroData.accAngleY;
  //
  // walk cycle related operations
  if (sequenceCount == 0) {
    // start of walk cycle
    accAverageValue.rollMin = gyroData.roll;
    accAverageValue.rollMax = gyroData.roll;
    accAverageValue.rollMinTime = 0;
    accAverageValue.rollMaxTime = 0;
  } else {
    // find max and min roll
    if (gyroData.roll > accAverageValue.rollMax) {
      accAverageValue.rollMax = gyroData.roll;
      accAverageValue.rollMaxTime = sequenceCount;
    }
    if (gyroData.roll < accAverageValue.rollMin) {
      accAverageValue.rollMin = gyroData.roll;
      accAverageValue.rollMinTime = sequenceCount;
    }
  }
  // end walk cycle related operations
  accAverageValue.stateGyro = _statusGyro(accErrorAverage);
  if (stateGyroOld != accAverageValue.stateGyro) {
    stateGyroOld = accAverageValue.stateGyro;
    // debug print
    _printGyro(accAverageValue.stateGyro);
  }
  // debug print
  //if (sequenceCount == 0) {
  //  _printLineGyro(accErrorAverage);
  //}
  return accAverageValue;
}

// reset gyro data
void resetGyro(void) {
  gyroData.gyroAngleX = 0;
  gyroData.gyroAngleY = 0;
  gyroData.yaw = 0;
  gyroData.pitch = 0;
  //
  accDataOld1.accAngleX = (int)gyroData.accAngleX;
  accDataOld1.accAngleY = (int)gyroData.accAngleY;
  //
  accDataOld2.accAngleX = accDataOld1.accAngleX;
  accDataOld2.accAngleY = accDataOld1.accAngleY;
  //
  accAverageValue.accAngleX = accDataOld2.accAngleX;
  accAverageValue.accAngleY = accDataOld2.accAngleY;
  accAverageValue.rollMin = 0;
  accAverageValue.rollMax = 0;
  accAverageValue.rollMinTime = 0;
  accAverageValue.rollMaxTime = 0;
  accAverageValue.stateGyro = GYRO_NORM;
}

// correct horizontal direction
void resetDirectionGyro(void) {
  // positive - turned left
  // negative - turned right
  directionOld = gyroData.yaw;
  gyroData.yaw = 0;
}

// return to old horizontal direction
void restoreDirectionGyro(void) {
  gyroData.yaw += directionOld;
}

// get walking direction correction from gyroscope
char getDirectionCorrectionGyro(void) {
  // maximal direction correction is 5
  if (gyroData.yaw > 10) {
    return 5;
  }
  if (gyroData.yaw < -10) {
    return -5;
  }
  return (char)(gyroData.yaw / 2);
}
/*
// print gyro values
void _printLineGyro(struct acc data) {
  Serial.print(" aX ");
  Serial.print((int)data.accAngleX);
  Serial.print(" aY ");
  Serial.print((int)data.accAngleY);
  Serial.print(" gX ");
  Serial.print((int)gyroData.gyroAngleX);
  Serial.print(" gY ");
  Serial.print((int)gyroData.gyroAngleY);
  Serial.print(" d ");
  Serial.print((int)gyroData.yaw);
  Serial.print(" p ");
  Serial.print((int)gyroData.pitch);
  Serial.print(" r ");
  Serial.println((int)gyroData.roll);
}
*/
// print gyro values
void _printGyro(unsigned char state) {
  // print gyro status
  switch (state) {
    case GYRO_NORM:
      Serial.print(F(" GYRO_NORM "));
    break;
    case GYRO_SHAKEN:
      Serial.print(F(" GYRO_SHAKEN "));
    break;
    case GYRO_UPSIDEDOWN:
      Serial.print(F(" GYRO_UPSIDEDOWN "));
    break;
    case GYRO_HIT_SIDE:
      Serial.print(F(" GYRO_HIT_SIDE "));
    break;
    case GYRO_HIT_FRONT:
      Serial.print(F(" GYRO_HIT_FRONT "));
    break;
    case GYRO_FELL_LEFT:
      Serial.print(F(" GYRO_FELL_LEFT "));
    break;
    case GYRO_FELL_RIGHT:
      Serial.print(F(" GYRO_FELL_RIGHT "));
    break;
    case GYRO_FELL_FRONT:
      Serial.print(F(" GYRO_FELL_FRONT "));
    break;
    case GYRO_FELL_BACK:
      Serial.print(F(" GYRO_FELL_BACK "));
    break;
    case GYRO_DOWN_HILL:
      Serial.print(F(" GYRO_DOWN_HILL "));
    break;
    case GYRO_UP_HILL:
      Serial.print(F(" GYRO_UP_HILL "));
    break;
    case GYRO_FOLLING_LEFT:
      Serial.print(F(" GYRO_FOLLING_LEFT "));
    break;
    case GYRO_FOLLING_RIGHT:
      Serial.print(F(" GYRO_FOLLING_RIGHT "));
    break;
    case GYRO_FOLLING_FRONT:
      Serial.print(F(" GYRO_FOLLING_FRONT "));
    break;
    case GYRO_FOLLING_BACK:
      Serial.print(F(" GYRO_FOLLING_BACK "));
    break;
    default:
      Serial.println(F(" Wrong gyro state "));
  }
}

// status of gyro
unsigned char _statusGyro(struct acc data) {
  // upside down r > 70 or r < -70
  if ((gyroData.roll > 70) || (gyroData.roll < -70)) {
    return GYRO_UPSIDEDOWN;
  }
  // shaken aX > 200 or aY > 200
  if ((data.accAngleX > 200) || (data.accAngleY > 200)) {
    return GYRO_SHAKEN;
  }
  // hit side aX > 100
  if ((data.accAngleX > 100) && (data.accAngleX > data.accAngleY)) {
    return GYRO_HIT_SIDE;
  }
  // hit front aY > 100
  if ((data.accAngleY > 100) && (data.accAngleY > data.accAngleX)) {
    return GYRO_HIT_FRONT;
  }
  // fell left r < -30
  if (gyroData.roll < -40) {
    return GYRO_FELL_LEFT;
  }
  // fell right r > 30
  if (gyroData.roll > 40) {
    return GYRO_FELL_RIGHT;
  }
  // fell front p > 20
  if (gyroData.pitch > 40) {
    return GYRO_FELL_FRONT;
  }
  // fell back p < -20
  if (gyroData.pitch < -40) {
    return GYRO_FELL_BACK;
  }
  // walk down p > 5
  if (gyroData.pitch > 5) {
    return GYRO_DOWN_HILL;
  }
  // walk up p < -5
  if (gyroData.pitch < -5) {
    return GYRO_UP_HILL;
  }
  // folling left r negative grows
  if (gyroData.roll < -30) {
    return GYRO_FOLLING_LEFT;
  }
  // folling right r grows
  if (gyroData.roll > 30) {
    return GYRO_FOLLING_RIGHT;
  }
  // folling front p grows
  if (gyroData.pitch > 20) {
    return GYRO_FOLLING_FRONT;
  }
  // folling back p negative grows
  if (gyroData.pitch < -20) {
    return GYRO_FOLLING_BACK;
  }
  // walking aX > 5 or aY > 5
  return GYRO_NORM;
}
