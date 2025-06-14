/*
Walking Robot Lowrie
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
int rollMinBuffer;
int rollMaxBuffer;
unsigned char rollMinTimeBuffer;
unsigned char rollMaxTimeBuffer;

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
  AccErrorX /= 200; // deg, positive-turn right, negative-turn left, 0-90. upsige down is the same
  AccErrorY /= 200; // deg, positive-fase down, negative-face up, 0-90. upsige down is the same
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
  if ((sequenceCount == 0) || (sequenceCount == m_init.halfCycle)) {
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
    accAverageValue.rollMin = rollMinBuffer;
    accAverageValue.rollMax = rollMaxBuffer;
    accAverageValue.rollMinTime = rollMinTimeBuffer;
    accAverageValue.rollMaxTime = rollMaxTimeBuffer;
    rollMinBuffer = gyroData.roll;
    rollMaxBuffer = gyroData.roll;
    rollMinTimeBuffer = 0;
    rollMaxTimeBuffer = 0;
  } else {
    // find max and min roll
    if (gyroData.roll > rollMaxBuffer) {
      rollMaxBuffer = gyroData.roll;
      rollMaxTimeBuffer = sequenceCount;
    }
    if (gyroData.roll < rollMinBuffer) {
      rollMinBuffer = gyroData.roll;
      rollMinTimeBuffer = sequenceCount;
    }
  }
  // end walk cycle related operations
  if (sequenceCount == 0) {
    accAverageValue.stateGyro = _statusGyro();
    if (stateGyroOld != accAverageValue.stateGyro) {
      stateGyroOld = accAverageValue.stateGyro;
      // debug print
      //_printGyro(accAverageValue.stateGyro);
    }
    // debug print
    //  _printLineGyro();
  }
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

// get direction from gyroscope
char getDirectionGyro(void) {
  return (char)(gyroData.yaw);
}

// get walking direction correction from gyroscope
char getDirectionCorrectionGyro(void) {
  // maximal direction correction is 5
  if (gyroData.yaw > 10) {
    return 10;
  }
  if (gyroData.yaw < -10) {
    return -10;
  }
  return (char)(gyroData.yaw);
}

// get roll
int getRollGiro(void) {
  // positive - rolled right
  // negative - rolled left
  //return gyroData.roll;
  return (int)(gyroData.accAngleX / 2);
}

// get pitch
int getPitchGiro(void) {
  // positive - nose down
  // negative - nose up  
  //return gyroData.pitch;
  return (int)(gyroData.accAngleY / 2);
}

// status of gyro
unsigned char _statusGyro(void) {
  // upside down r > 70 or r < -70
  if ((gyroData.roll > 70) || (gyroData.roll < -70)) {
    return GYRO_UPSIDEDOWN;
  }
  // fell left r < -30
  if ((gyroData.roll + ((int)AccErrorX)) < -40) {
    return GYRO_FELL_LEFT;
  }
  // fell right r > 30
  if ((gyroData.roll + ((int)AccErrorX)) > 40) {
    return GYRO_FELL_RIGHT;
  }
  // fell front p > 20
  if ((gyroData.pitch + ((int)AccErrorY)) > 40) {
    return GYRO_FELL_FRONT;
  }
  // fell back p < -20
  if ((gyroData.pitch + ((int)AccErrorY)) < -40) {
    return GYRO_FELL_BACK;
  }
  // folling left r negative grows
  if ((gyroData.roll + ((int)AccErrorX)) < -30) {
    return GYRO_FOLLING_LEFT;
  }
  // folling right r grows
  if ((gyroData.roll + ((int)AccErrorX)) > 30) {
    return GYRO_FOLLING_RIGHT;
  }
  // folling front p grows
  if ((gyroData.pitch + ((int)AccErrorY)) > 20) {
    return GYRO_FOLLING_FRONT;
  }
  // folling back p negative grows
  if ((gyroData.pitch + ((int)AccErrorY)) < -20) {
    return GYRO_FOLLING_BACK;
  }
  // walk down p > 5
  if ((gyroData.pitch + ((int)AccErrorY)) > 5) {
    return GYRO_DOWN_HILL;
  }
  // walk up p < -5
  if ((gyroData.pitch + ((int)AccErrorY)) < -5) {
    return GYRO_UP_HILL;
  }
  // walking aX > 5 or aY > 5
  return GYRO_NORM;
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
*/