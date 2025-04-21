/*
Walking Hexapod Robot
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Operates servo motors
*/

#include <Servo.h>

// pin numbers for servo motors
enum dPinsServo {
  FL1_MOTOR = 2,
  FL2_MOTOR = 3,
  FR1_MOTOR = 4,
  FR2_MOTOR = 5,
  SL1_MOTOR = 6,
  SL2_MOTOR = 7,
  SR1_MOTOR = 8,
  SR2_MOTOR = 9,
  RL1_MOTOR = 10,
  RL2_MOTOR = 11,
  RR1_MOTOR = 12,
  RR2_MOTOR = 13
};

// init servo library
Servo servo_fl_1;
Servo servo_fl_2;
Servo servo_fr_1;
Servo servo_fr_2;
Servo servo_sl_1;
Servo servo_sl_2;
Servo servo_sr_1;
Servo servo_sr_2;
Servo servo_rl_1;
Servo servo_rl_2;
Servo servo_rr_1;
Servo servo_rr_2;

// better walking 47 points. center point is 24
// 5mm shift
//          shift     -110 -110 -110 -105 -100  -95  -90  -85  -80  -75  -70  -65  -60  -55  -50  -45  -40  -35  -30  -25  -20  -15  -10   -5    0    5   10   15   20   25   30   35   40   45   50   55   60   65   70   75   80   85   90   95  100  100  100
//char m1Walk[47] =  {  22,  22,  22,  22,  22,  22,  22,  22,  22,  22,  23,  24,  25,  26,  27,  28,  30,  31,  33,  35,  37,  39,  41,  43,  45,  47,  50,  53,  56,  58,  61,  64,  68,  71,  74,  77,  81,  84,  88,  92,  97, 102, 108, 116, 126, 126, 126};
//char m2Walk[47] =  { 118, 118, 118, 112, 107, 102,  98,  94,  91,  87,  84,  81,  78,  75,  72,  69,  66,  63,  60,  58,  55,  52,  50,  48,  45,  42,  40,  38,  36,  34,  33,  31,  29,  27,  26,  25,  24,  23,  23,  22,  22,  22,  22,  22,  22,  22,  22};
// better walking 68 points. center point is 35
//char m1Walk[68] =  {  22,  22,  22, 22, 22, 22, 22,22, 22,22,22, 22,22,23, 23,24,24, 25,25,26, 27,28,29, 30,31,32, 33,34,35, 37,38,39, 41,42,43, 45,46,48, 50,52,54, 56,58,60, 62,64,66, 68,70,72, 74,76,78, 81, 83,86,88,91,94,97,100,104,108,114,120, 126, 126, 126};
//char m2Walk[68] =  { 118, 118, 118,114,110,107,104,101,98,95,93, 91,88,86, 84,82,80, 78,76,74, 72,70,68, 66,64,62, 60,58,56, 55,53,51, 50,48,46, 45,43,41, 40,39,37, 36,35,34, 33,32,31, 29,28,27, 26,25,24, 24, 24,23,23,23,22,22, 22, 22, 22, 22, 22,  22,  22,  22};
// walking 93 points. center point is 48
//          shift     -110      -110      -110      -105      -100       -95       -90       -85       -80       -75       -70       -65       -60       -55       -50       -45       -40       -35       -30       -25       -20       -15       -10        -5         0         5        10        15        20        25        30        35        40        45        50        55        60        65        70        75        80        85        90        95       100       100       100
char m1Walk[93] =    {  22,  22,  22,  22,  22,  22,  22,  22,  22,  22,  22,  22,  22,  22,  22,  22,  22,  22,  22,  23,  23,  23,  24,  24,  25,  25,  26,  26,  27,  27,  28,  29,  30,  30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,  48,  50,  51,  53,  54,  56,  57,  58,  59,  61,  62,  64,  66,  68,  69,  71,  72,  74,  75,  77,  79,  81,  82,  84,  86,  88,  90,  92,  94,  97,  99, 102, 105, 108, 112, 116, 121, 126, 126, 126, 126, 126};
char m2Walk[93] =    { 118, 118, 118, 118, 118, 115, 112, 109, 107, 104, 102, 100,  98,  96,  94,  92,  91,  89,  87,  85,  84,  82,  81,  79,  78,  76,  75,  73,  72,  70,  69,  67,  66,  64,  63,  61,  60,  59,  58,  56,  55,  53,  52,  51,  50,  49,  48,  46,  45,  43,  42,  41,  40,  39,  38,  37,  36,  35,  34,  33,  33,  32,  31,  30,  29,  28,  27,  26,  26,  25,  25,  24,  24,  23,  23,  23,  23,  22,  22,  22,  22,  22,  22,  22,  22,  22,  22,  22,  22,  22,  22,  22,  22};

char forwardBallance = 0;
// static ballance
allMotors staticBallance = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
// motors attached flag
bool attached = false;
// motors init values
short m1InitValue = 0;
short m2InitValue = 0;
short m1Value = 0;
short m2Value = 0;

// linear motor move
void _moveLinearServo(allMotors calibration, short calM1, short calM2) {
  bool doLoop = true;
  while (doLoop) {
    if (calM1 > m1Value) {
      m1Value ++;
    } else if (calM1 < m1Value) {
      m1Value --;
    }
    if (calM2 > m2Value) {
      m2Value ++;
    } else if (calM2 < m2Value) {
      m2Value --;
    }
    servo_fl_1.write(limitMotorValue(90 - 30 + m1Value + calibration.m.fl.motor1));
    servo_fl_2.write(limitMotorValue(90 + 30 - m2Value - calibration.m.fl.motor2));
    servo_fr_1.write(limitMotorValue(90 + 30 - m1Value - calibration.m.fr.motor1));
    servo_fr_2.write(limitMotorValue(90 - 30 + m2Value + calibration.m.fr.motor2));
    servo_sl_1.write(limitMotorValue(90 - 30 + m1Value + calibration.m.sl.motor1));
    servo_sl_2.write(limitMotorValue(90 + 30 - m2Value - calibration.m.sl.motor2));
    servo_sr_1.write(limitMotorValue(90 + 30 - m1Value - calibration.m.sr.motor1));
    servo_sr_2.write(limitMotorValue(90 - 30 + m2Value + calibration.m.sr.motor2));
    servo_rl_1.write(limitMotorValue(90 - 30 + m1Value + calibration.m.rl.motor1));
    servo_rl_2.write(limitMotorValue(90 + 30 - m2Value - calibration.m.rl.motor2));
    servo_rr_1.write(limitMotorValue(90 + 30 - m1Value - calibration.m.rr.motor1));
    servo_rr_2.write(limitMotorValue(90 - 30 + m2Value + calibration.m.rr.motor2));
    delay(30);
    if ((calM1 == m1Value) && (calM2 == m2Value)) {
      doLoop = false;
    }
  }
}

// limit angle value
short limitMotorValue(short mAngle) {
  if (mAngle > 180) {
    mAngle = 180;
  } else if (mAngle < 0) {
    mAngle = 0;
  }
  return mAngle;
}

// init servo motors
void initServo(allMotors calibration, short calM1, short calM2) {
  // init motors one by one
  servo_fl_1.attach(FL1_MOTOR, 500, 2500);
  servo_fl_1.write(limitMotorValue(90 - 30 + calM1 + calibration.m.fl.motor1));
  delay(100);
  servo_fr_1.attach(FR1_MOTOR, 500, 2500);
  servo_fr_1.write(limitMotorValue(90 + 30 - calM1 - calibration.m.fr.motor1));
  delay(100);
  servo_rl_1.attach(RL1_MOTOR, 500, 2500);
  servo_rl_1.write(limitMotorValue(90 - 30 + calM1 + calibration.m.rl.motor1));
  delay(100);
  servo_rr_1.attach(RR1_MOTOR, 500, 2500);
  servo_rr_1.write(limitMotorValue(90 + 30 - calM1 - calibration.m.rr.motor1));
  delay(100);
  servo_sl_1.attach(SL1_MOTOR, 500, 2500);
  servo_sl_1.write(limitMotorValue(90 - 30 + calM1 + calibration.m.sl.motor1));
  delay(100);
  servo_sr_1.attach(SR1_MOTOR, 500, 2500);
  servo_sr_1.write(limitMotorValue(90 + 30 - calM1 - calibration.m.sr.motor1));
  delay(100);
  servo_fl_2.attach(FL2_MOTOR, 500, 2500);
  servo_fl_2.write(limitMotorValue(90 + 30 - calM2 - calibration.m.fl.motor2));
  delay(100);
  servo_fr_2.attach(FR2_MOTOR, 500, 2500);
  servo_fr_2.write(limitMotorValue(90 - 30 + calM2 + calibration.m.fr.motor2));
  delay(100);
  servo_rl_2.attach(RL2_MOTOR, 500, 2500);
  servo_rl_2.write(limitMotorValue(90 + 30 - calM2 - calibration.m.rl.motor2));
  delay(100);
  servo_rr_2.attach(RR2_MOTOR, 500, 2500);
  servo_rr_2.write(limitMotorValue(90 - 30 + calM2 + calibration.m.rr.motor2));
  delay(100);
  servo_sl_2.attach(SL2_MOTOR, 500, 2500);
  servo_sl_2.write(limitMotorValue(90 + 30 - calM2 - calibration.m.sl.motor2));
  delay(100);
  servo_sr_2.attach(SR2_MOTOR, 500, 2500);
  servo_sr_2.write(limitMotorValue(90 - 30 + calM2 + calibration.m.sr.motor2));
  delay(100);
  attached = true;
  m1InitValue = calM1;
  m2InitValue = calM2;
  m1Value = calM1;
  m2Value = calM2;
}

// attach wings
void detachServo(allMotors calibration) {
  if (attached) {
    _moveLinearServo(calibration, m1InitValue, m2InitValue);
    servo_sl_1.detach();
    servo_sr_1.detach();
    servo_fl_1.detach();
    servo_fr_1.detach();
    servo_rl_1.detach();
    servo_rr_1.detach();
    servo_sl_2.detach();
    servo_sr_2.detach();
    servo_fl_2.detach();
    servo_fr_2.detach();
    servo_rl_2.detach();
    servo_rr_2.detach();
  }
  attached = false;
  delay(1000);
}

// set servo motors
void setServo(allMotors calibration, short calM1, short calM2) {
  // set motors values after calibration
  _moveLinearServo(calibration, calM1, calM2);
}

// move motors.
void updateServo(allMotors calibration, allMotors motorValue, allMotors motorLift) {
  servo_fl_1.write(limitMotorValue(90 - 30 + (m1Walk[motorValue.m.fl.motor1 + m_forwardCenterServo + forwardBallance] + calibration.m.fl.motor1 + motorLift.m.fl.motor1 + staticBallance.m.fl.motor1)));
  servo_fl_2.write(limitMotorValue(90 + 30 - (m2Walk[motorValue.m.fl.motor2 + m_forwardCenterServo + forwardBallance] + calibration.m.fl.motor2 + motorLift.m.fl.motor2 + staticBallance.m.fl.motor2)));
  servo_fr_1.write(limitMotorValue(90 + 30 - (m1Walk[motorValue.m.fr.motor1 + m_forwardCenterServo + forwardBallance] + calibration.m.fr.motor1 + motorLift.m.fr.motor1 + staticBallance.m.fr.motor1)));
  servo_fr_2.write(limitMotorValue(90 - 30 + (m2Walk[motorValue.m.fr.motor2 + m_forwardCenterServo + forwardBallance] + calibration.m.fr.motor2 + motorLift.m.fr.motor2 + staticBallance.m.fr.motor2)));
  servo_sl_1.write(limitMotorValue(90 - 30 + (m1Walk[motorValue.m.sl.motor1 + m_forwardCenterServo + forwardBallance] + calibration.m.sl.motor1 + motorLift.m.sl.motor1 + staticBallance.m.sl.motor1)));
  servo_sl_2.write(limitMotorValue(90 + 30 - (m2Walk[motorValue.m.sl.motor2 + m_forwardCenterServo + forwardBallance] + calibration.m.sl.motor2 + motorLift.m.sl.motor2 + staticBallance.m.sl.motor2)));
  servo_sr_1.write(limitMotorValue(90 + 30 - (m1Walk[motorValue.m.sr.motor1 + m_forwardCenterServo + forwardBallance] + calibration.m.sr.motor1 + motorLift.m.sr.motor1 + staticBallance.m.sr.motor1)));
  servo_sr_2.write(limitMotorValue(90 - 30 + (m2Walk[motorValue.m.sr.motor2 + m_forwardCenterServo + forwardBallance] + calibration.m.sr.motor2 + motorLift.m.sr.motor2 + staticBallance.m.sr.motor2)));
  servo_rl_1.write(limitMotorValue(90 - 30 + (m1Walk[motorValue.m.rl.motor1 + m_forwardCenterServo + forwardBallance] + calibration.m.rl.motor1 + motorLift.m.rl.motor1 + staticBallance.m.rl.motor1)));
  servo_rl_2.write(limitMotorValue(90 + 30 - (m2Walk[motorValue.m.rl.motor2 + m_forwardCenterServo + forwardBallance] + calibration.m.rl.motor2 + motorLift.m.rl.motor2 + staticBallance.m.rl.motor2)));
  servo_rr_1.write(limitMotorValue(90 + 30 - (m1Walk[motorValue.m.rr.motor1 + m_forwardCenterServo + forwardBallance] + calibration.m.rr.motor1 + motorLift.m.rr.motor1 + staticBallance.m.rr.motor1)));
  servo_rr_2.write(limitMotorValue(90 - 30 + (m2Walk[motorValue.m.rr.motor2 + m_forwardCenterServo + forwardBallance] + calibration.m.rr.motor2 + motorLift.m.rr.motor2 + staticBallance.m.rr.motor2)));
}

void updateStaticBallanceServo(allMotors cBallance) {
  staticBallance.m.fl.motor1 = cBallance.m.fl.motor1;
  staticBallance.m.fl.motor2 = cBallance.m.fl.motor2;
  staticBallance.m.fr.motor1 = cBallance.m.fr.motor1;
  staticBallance.m.fr.motor2 = cBallance.m.fr.motor2;
  staticBallance.m.sl.motor1 = cBallance.m.sl.motor1;
  staticBallance.m.sl.motor2 = cBallance.m.sl.motor2;
  staticBallance.m.sr.motor1 = cBallance.m.sr.motor1;
  staticBallance.m.sr.motor2 = cBallance.m.sr.motor2;
  staticBallance.m.rl.motor1 = cBallance.m.rl.motor1;
  staticBallance.m.rl.motor2 = cBallance.m.rl.motor2;
  staticBallance.m.rr.motor1 = cBallance.m.rr.motor1;
  staticBallance.m.rr.motor2 = cBallance.m.rr.motor2;
}

void setFowardBallanceServo(char ballanceValue) {
  forwardBallance = ballanceValue;
}
