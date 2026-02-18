/*
Walking Robot TurtleV1
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
use MPU6050 to read gyroscope and accelerometer
*/

#include <Wire.h>

// gyro errors structure
typedef struct errors {
float AccErrorX;
float AccErrorY;
float GyroErrorX;
float GyroErrorY;
float GyroErrorZ;
} errors;

// gyro errors
errors gyroErrors = {0, 0, 0, 0, 0};
// buffers to read register into
float floatBuffer[3];
// float data
float floatRollX;         // roll       right - positive   -90 0 90 upsidedown also 0
float floatPitchY;        // pitch      down - positive   -90 0 90 upsidedown also 0
float floatDirection;     // direction  turn left - positive
// time parameters
float timeInterval;
unsigned long currentTime = 0;
unsigned long oldTime;
// gyro state
unsigned char stateGyroOld = GYRO_NORM;
// roll data
short rollMinBuffer;
short rollMaxBuffer;
unsigned char rollMinTimeBuffer;
unsigned char rollMaxTimeBuffer;
// shake parameters
short shakeNow = 0;
short shakeOld = 0;
short rollNow = 0;
short rollOld = 0;
short pitchNow = 0;
short pitchOld = 0;
// walking direction
short walkingDirection = 0;
short walkingDirectionOld = 0;

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
    gyroErrors.AccErrorX += atan((floatBuffer[1]) / sqrt(pow((floatBuffer[0]), 2) + pow((floatBuffer[2]), 2))) * 180 / PI;
    gyroErrors.AccErrorY += atan(-1 * (floatBuffer[0]) / sqrt(pow((floatBuffer[1]), 2) + pow((floatBuffer[2]), 2))) * 180 / PI;
  }
  gyroErrors.AccErrorX /= 200; // deg, positive-turn right, negative-turn left, 0-90. upsige down is the same
  gyroErrors.AccErrorY /= 200; // deg, positive-fase down, negative-face up, 0-90. upsige down is the same
  for (i = 0; i < 200; i++) {
    _readWire(floatBuffer, 0x43);
    floatBuffer[0] /= 131.0;
    floatBuffer[1] /= 131.0;
    floatBuffer[2] /= 131.0;
    gyroErrors.GyroErrorX += floatBuffer[0];
    gyroErrors.GyroErrorY += floatBuffer[1];
    gyroErrors.GyroErrorZ += floatBuffer[2];
  }
  gyroErrors.GyroErrorX /= 200;
  gyroErrors.GyroErrorY /= 200;
  gyroErrors.GyroErrorZ /= 200;
  // write gyro calibration when button pressed
  if (analogRead(A6) < INPUT_GROUNDED) {
    EEPROM.put(16, gyroErrors);
    Serial.println(" writing gyro errors ");
  } else {
    EEPROM.get(16, gyroErrors);
    Serial.println(" reading gyro errors ");
  }
  // print error values
  Serial.print(F(" AccErrorX "));
  Serial.print((int)(gyroErrors.AccErrorX * 10));
  Serial.print(F(" AccErrorY "));
  Serial.print((int)(gyroErrors.AccErrorY * 10));
  Serial.print(F(" GyroErrorX "));
  Serial.print((int)(gyroErrors.GyroErrorX * 10));
  Serial.print(F(" GyroErrorY "));
  Serial.print((int)(gyroErrors.GyroErrorY * 10));
  Serial.print(F(" GyroErrorZ "));
  Serial.println((int)(gyroErrors.GyroErrorZ * 10));
  //AccErrorX 0 AccErrorY -2 GyroErrorX -2 GyroErrorY -5 GyroErrorZ -1
}

// fix rotation angle
float _fixAngle(float angle, float limit){
  if (angle > limit) {
    angle -= limit;
  } else if (angle < -limit) {
    angle += limit;
  }
  return angle;
}

// read gyroscope and accelerometer data
void updateGyro(void) {
  // accelerometer
  _readWire(floatBuffer, 0x3B);
  floatBuffer[0] /= 16384.0;
  floatBuffer[1] /= 16384.0;
  floatBuffer[2] /= 16384.0;
  floatRollX  = (atan(floatBuffer[1] / sqrt(pow(floatBuffer[0], 2) + pow(floatBuffer[2], 2))) * 180 / PI) - gyroErrors.AccErrorX;
  floatPitchY = (atan(-1 * floatBuffer[0] / sqrt(pow(floatBuffer[1], 2) + pow(floatBuffer[2], 2))) * 180 / PI) - gyroErrors.AccErrorY;
  m_gyroState.accUpsideZ = (short)((floatBuffer[2]) * 100);
  // gyroscope
  oldTime = currentTime;
  currentTime = millis();
  timeInterval = (currentTime - oldTime) / 1000.0;
  _readWire(floatBuffer, 0x43);
  floatBuffer[0] = floatBuffer[0] / 131.0 - gyroErrors.GyroErrorX;
  floatBuffer[1] = floatBuffer[1] / 131.0 - gyroErrors.GyroErrorY;
  floatBuffer[2] = floatBuffer[2] / 131.0 - gyroErrors.GyroErrorZ;
  floatDirection  += floatBuffer[2] * timeInterval;
  // short data
  rollNow = (short)floatRollX;
  pitchNow = -(short)floatPitchY;
  // shake data processing
  if (rollNow > rollOld) {
    shakeNow += rollNow - rollOld;
  } else {
    shakeNow += rollOld - rollNow;
  }
  if (pitchNow > pitchOld) {
    shakeNow += pitchNow - pitchOld;
  } else {
    shakeNow += pitchOld - pitchNow;
  }
  // remember roll and pitch
  m_gyroState.accRollX = (rollNow + rollOld) / 2;
  m_gyroState.accPitchY = (pitchNow + pitchOld) / 2;
  // remember position
  rollOld = rollNow;
  pitchOld = pitchNow;
  // cycle statrt
  if (m_sequenceCounter.m == 0) {
    // process direction
    // fix rotation angle value
    floatDirection  = _fixAngle(floatDirection, 180);
    // get gyro data
    m_gyroState.direction =  -((short)floatDirection * 2);
    // process roll
    // remember roll
    m_gyroState.rollMin = rollMinBuffer;
    m_gyroState.rollMax = rollMaxBuffer;
    m_gyroState.rollMinTime = rollMinTimeBuffer;
    m_gyroState.rollMaxTime = rollMaxTimeBuffer;
    // reset roll
    rollMinBuffer = rollNow;
    rollMaxBuffer = rollNow;
    rollMinTimeBuffer = 0;
    rollMaxTimeBuffer = 0;
  } else {
    // find max and min roll
    if (rollNow > rollMaxBuffer) {
      rollMaxBuffer = rollNow;
      rollMaxTimeBuffer = m_sequenceCounter.m;
    }
    if (rollNow < rollMinBuffer) {
      rollMinBuffer = rollNow;
      rollMinTimeBuffer = m_sequenceCounter.m;
    }
  }
  // walk cycle related operations
  if (m_sequenceCounter.m == 0) {
    // process data
    // get status
    m_gyroState.stateGyro = _statusGyro();
    if (stateGyroOld != m_gyroState.stateGyro) {
      stateGyroOld = m_gyroState.stateGyro;
      // debug print
      //_printGyro();
    }
  }
  // remember and reset shake values
  shakeOld = shakeNow;
  shakeNow = 0;
}

// reset gyro data
void resetGyro(void) {
  floatDirection = 0;
  m_gyroState.direction = 0;
  m_gyroState.rollMin = 0;
  m_gyroState.rollMax = 0;
  m_gyroState.rollMinTime = 0;
  m_gyroState.rollMaxTime = 0;
  m_gyroState.stateGyro = GYRO_NORM;
  walkingDirectionOld = 0;
  walkingDirection = 0;
  shakeOld = 0;
  shakeNow = 0;
  rollOld = 0;
  pitchOld = 0;
}

// get walking direction
int getDirectionGyro(void) {
  return m_gyroState.direction - walkingDirection;
}

// remember horizontal direction
void setDirectionGyro(void) {
  walkingDirection = m_gyroState.direction;
  walkingDirectionOld = walkingDirection;
}

// correct horizontal direction
void resetDirectionGyro(void) {
  walkingDirection = m_gyroState.direction;
}

// return to old horizontal direction
void restoreDirectionGyro(void) {
  walkingDirection = walkingDirectionOld;
}

// status of gyro
unsigned char _statusGyro(void) {
  // if shaken wait
  if (shakeNow > (shakeOld * 2)) {
    return GYRO_SHAKEN;
  }
  // upside down
  if ((m_gyroState.accUpsideZ < 0) && (m_robotState.flipStateNow == 1)) {
    return GYRO_UPSIDEDOWN;
  }
  // reset
  if ((m_gyroState.accUpsideZ > 0) && (m_robotState.flipStateNow == -1)) {
    return GYRO_RESET;
  }
  // fell left
  if (m_gyroState.accRollX < -FALLING_ANGLE) {
    return GYRO_FELL_LEFT;
  }
  // fell right
  if (m_gyroState.accRollX > FALLING_ANGLE) {
    return GYRO_FELL_RIGHT;
  }
  // fell front
  if (m_gyroState.accPitchY < -FALLING_ANGLE) {
    return GYRO_FELL_FRONT;
  }
  // fell back
  if (m_gyroState.accPitchY > FALLING_ANGLE) {
    return GYRO_FELL_BACK;
  }
  // folling left
  if (m_gyroState.accRollX < -OFFROAD_ANGLE) {
    return GYRO_FOLLING_LEFT;
  }
  // folling right
  if (m_gyroState.accRollX > OFFROAD_ANGLE) {
    return GYRO_FOLLING_RIGHT;
  }
  // folling front
  if (m_gyroState.accPitchY < -OFFROAD_ANGLE) {
    return GYRO_FOLLING_FRONT;
  }
  // folling back
  if (m_gyroState.accPitchY > OFFROAD_ANGLE) {
    return GYRO_FOLLING_BACK;
  }
  if (m_gyroState.direction - walkingDirection > 4) {
    return GYRO_TURNED_RIGHT;
  }
  if (m_gyroState.direction - walkingDirection < -4) {
    return GYRO_TURNED_LEFT;
  }
  return GYRO_NORM;
}
/*
// print gyro values
void _printGyro(void) {
  // print gyro status
  switch (m_gyroState.stateGyro) {
    case GYRO_NORM:
      Serial.println(F(" GYRO_NORM "));
    break;
    case GYRO_TURNED_RIGHT:
      Serial.println(F(" GYRO_TURNED_RIGHT "));
    break;
    case GYRO_TURNED_LEFT:
      Serial.println(F(" GYRO_TURNED_LEFT "));
    break;
    case GYRO_UPSIDEDOWN:
      Serial.println(F(" GYRO_UPSIDEDOWN "));
    break;
    case GYRO_FELL_LEFT:
      Serial.println(F(" GYRO_FELL_LEFT "));
    break;
    case GYRO_FELL_RIGHT:
      Serial.println(F(" GYRO_FELL_RIGHT "));
    break;
    case GYRO_FELL_FRONT:
      Serial.println(F(" GYRO_FELL_FRONT "));
    break;
    case GYRO_FELL_BACK:
      Serial.println(F(" GYRO_FELL_BACK "));
    break;
    case GYRO_FOLLING_LEFT:
      Serial.println(F(" GYRO_FOLLING_LEFT "));
    break;
    case GYRO_FOLLING_RIGHT:
      Serial.println(F(" GYRO_FOLLING_RIGHT "));
    break;
    case GYRO_FOLLING_FRONT:
      Serial.println(F(" GYRO_FOLLING_FRONT "));
    break;
    case GYRO_SHAKEN:
      Serial.println(F(" GYRO_SHAKEN "));
    break;
    case GYRO_FOLLING_BACK:
      Serial.println(F(" GYRO_FOLLING_BACK "));
    break;
    default:
      Serial.println(F(" Wrong gyro state "));
  }
}
*/