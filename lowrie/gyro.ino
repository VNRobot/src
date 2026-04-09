/*
Walking Robot Lowrie
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
// shake parameters
short shakeNow = 0;
short shakeOld = 0;
short rollNow = 0;
short rollOld = 0;
short pitchNow = 0;
short pitchOld = 0;
short deltaRoll = 0;
short deltaPitch = 0;
long rollAverage = 0;
long pitchAverage = 0;
short rollAverageOld = 0;
short pitchAverageOld = 0;
// walking direction
short walkingDirection = 0;
short walkingDirectionOld = 0;
// flipped state
bool gyroFlipped = false;

/*
uses
m_gyroState
m_getButtonPressed()
*/

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

// set flipped state
void setFlippedGyro(bool flipped) {
  gyroFlipped = flipped;
}

// init gyroscope and accelerometer MPU6050 using I2C
void initGyro(bool calibrationMode) {
  int i;
  // check for calibration mode
  if (calibrationMode) {
    while (!m_getButtonPressed()) {
      delay(100);
    }
    delay(1000);
  }
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
  if (calibrationMode) {
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
void updateGyro(unsigned char counter) {
  // accelerometer
  _readWire(floatBuffer, 0x3B);
  floatBuffer[0] /= 16384.0;
  floatBuffer[1] /= 16384.0;
  floatBuffer[2] /= 16384.0;
  floatRollX  = (atan(floatBuffer[1] / sqrt(pow(floatBuffer[0], 2) + pow(floatBuffer[2], 2))) * 180 / PI) - gyroErrors.AccErrorX;
  floatPitchY = (atan(-1 * floatBuffer[0] / sqrt(pow(floatBuffer[1], 2) + pow(floatBuffer[2], 2))) * 180 / PI) - gyroErrors.AccErrorY;
  m_gyroState.aUpsideAverage = (short)((floatBuffer[2]) * 100);
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
  // data now
  m_gyroState.aRollNow = rollNow;
  m_gyroState.aPitchNow = pitchNow;
  // deltas
  deltaRoll = rollNow - rollOld;
  deltaPitch = pitchNow - pitchOld;
  // remember position
  rollOld = rollNow;
  pitchOld = pitchNow;
  // add average
  rollAverage += rollNow;
  pitchAverage += pitchNow;
  // get lift value
  m_gyroState.aLiftFL = deltaRoll + deltaPitch;
  m_gyroState.aLiftFR = -deltaRoll + deltaPitch;
  m_gyroState.aLiftRL = deltaRoll - deltaPitch;
  m_gyroState.aLiftRR = -deltaRoll - deltaPitch;
  // cycle statrt
  if (counter == 0) {
    // remember roll and pitch
    m_gyroState.aRollAverage = rollAverage / MAIN_FULL_CYCLE;
    m_gyroState.aPitchAverage = pitchAverage / MAIN_FULL_CYCLE;
    // reset average
    rollAverage = 0;
    pitchAverage = 0;
    // shake data processing
    if (m_gyroState.aRollAverage > rollAverageOld) {
      shakeNow += m_gyroState.aRollAverage - rollAverageOld;
    } else {
      shakeNow -= m_gyroState.aRollAverage - rollAverageOld;
    }
    if (m_gyroState.aPitchAverage > pitchAverageOld) {
      shakeNow += m_gyroState.aPitchAverage - pitchAverageOld;
    } else {
      shakeNow -= m_gyroState.aPitchAverage - pitchAverageOld;
    }
    rollAverageOld = m_gyroState.aRollAverage;
    pitchAverageOld = m_gyroState.aPitchAverage;
    // process direction
    // fix rotation angle value
    floatDirection  = _fixAngle(floatDirection, 180);
    // get gyro data
    m_gyroState.direction =  -((short)floatDirection * 2);
    // get status
    m_gyroState.stateGyro = _statusGyro();
    if (stateGyroOld != m_gyroState.stateGyro) {
      stateGyroOld = m_gyroState.stateGyro;
      // debug print
      //_printGyro();
    }
    //_printLineGyroDebug();
    // remember shake values
    shakeOld = shakeNow;
    shakeNow = 0;
  }
  //_printRollGyroDebug();
}

// reset gyro data
void resetGyroZero(void) {
  floatDirection = 0;
  m_gyroState.direction = 0;
  m_gyroState.stateGyro = GYRO_NORM;
  walkingDirectionOld = 0;
  walkingDirection = 0;
  shakeOld = 0;
  shakeNow = 0;
  rollOld = 0;
  pitchOld = 0;
  rollAverageOld = 0;
  pitchAverageOld = 0;
}

// get walking direction
short getDirectionGyro(void) {
  return m_gyroState.direction - walkingDirection;
}

// remember horizontal direction
void setDirectionGyroZero(void) {
  walkingDirection = m_gyroState.direction;
  walkingDirectionOld = walkingDirection;
}

// correct horizontal direction
void resetDirectionGyroZero(void) {
  walkingDirection = m_gyroState.direction;
}

// return to old horizontal direction
void restoreDirectionGyroZero(void) {
  walkingDirection = walkingDirectionOld;
}

// reverse horizontal direction
void reverseDirectionGyroZero(void) {
  if ((walkingDirection < 30) && (walkingDirection > -30)) {
    // not reverset yet
    if (m_gyroState.direction < 0) {
      walkingDirection = m_gyroState.direction + 180;
    } else {
      walkingDirection = m_gyroState.direction - 180;
    }
  }
}

// status of gyro
unsigned char _statusGyro(void) {
  // if shaken wait
  if (shakeNow < (shakeOld * 2)) {
    // not shaken 
    // fell left
    if (m_gyroState.aRollAverage < -FALLING_ANGLE) {
      return GYRO_FELL_LEFT;
    }
    // fell right
    if (m_gyroState.aRollAverage > FALLING_ANGLE) {
      return GYRO_FELL_RIGHT;
    }
    // fell front
    if (m_gyroState.aPitchAverage < -FALLING_ANGLE) {
      return GYRO_FELL_FRONT;
    }
    // fell back
    if (m_gyroState.aPitchAverage > FALLING_ANGLE) {
      return GYRO_FELL_BACK;
    }
    // check upside down
    if ((m_gyroState.aUpsideAverage < 0) && (!gyroFlipped)) {
      // flip event. update flip state
      return GYRO_UPSIDEDOWN;
    }
    // position normal reset walking
    if ((m_gyroState.aUpsideAverage > 0) && (gyroFlipped)) {
      return GYRO_RESET;
    }
  }
  // upside down. recover fail
  if ((m_gyroState.aUpsideAverage < 0) && (gyroFlipped)) {
    // do nothing for now
  }
  return GYRO_NORM;
}

// check surface
bool getSurfaceFlatGyro(void) {
  // check robot roll
  if ((m_gyroState.aRollAverage < -SLOP_ANGLE) || (m_gyroState.aRollAverage > SLOP_ANGLE)) {
    return false;
  }
  // check robot pitch
  if ((m_gyroState.aPitchAverage < -SLOP_ANGLE) || (m_gyroState.aPitchAverage > SLOP_ANGLE)) {
    return false;
  }
  return true;
}

/*
// print gyro values
void _printGyro(void) {
  // print gyro status
  switch (m_gyroState.stateGyro) {
    case GYRO_NORM:
      Serial.println(F(" GYRO_NORM "));
    break;
    case GYRO_UPSIDEDOWN:
      Serial.println(F(" GYRO_UPSIDEDOWN "));
    break;
    case GYRO_RESET:
      Serial.println(F(" GYRO_RESET "));
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
    default:
      Serial.println(F(" Wrong gyro state "));
  }
}

// print gyro values
void _printLineGyroDebug(void) {
  Serial.print(" shaken ");
  Serial.print((int)shakeNow);
  Serial.print(" aRoll ");
  Serial.print((int)m_gyroState.aRollAverage);
  Serial.print(" aPitch ");
  Serial.print((int)m_gyroState.aPitchAverage);
  Serial.print(" Z ");
  Serial.print((int)m_gyroState.aUpsideAverage);
  Serial.print(" direction ");
  Serial.println((int)m_gyroState.direction);
}

// print gyro values
void _printRollGyroDebug(void) {
  Serial.print(" aLiftFL ");
  Serial.print((int)m_gyroState.aLiftFL);
  Serial.print(" aLiftFR ");
  Serial.print((int)m_gyroState.aLiftFR);
  Serial.print(" aLiftRL ");
  Serial.print((int)m_gyroState.aLiftRL);
  Serial.print(" aLiftRR ");
  Serial.println((int)m_gyroState.aLiftRR);
}
*/