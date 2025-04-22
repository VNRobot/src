/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Operates servo motors
*/

#include <Servo.h>

// pin numbers for servo motors
enum dPinsServo {
  SW1_MOTOR = 2,
  SW2_MOTOR = 3,
  ST1_MOTOR = 4,
  ST2_MOTOR = 5,
  FL1_MOTOR = 6,
  FL2_MOTOR = 7,
  FR1_MOTOR = 8,
  FR2_MOTOR = 9,
  RL1_MOTOR = 10,
  RL2_MOTOR = 11,
  RR1_MOTOR = 12,
  RR2_MOTOR = 13
};

// init servo library
Servo servo_sw_1;
Servo servo_sw_2;
Servo servo_st_1;
Servo servo_st_2;
Servo servo_fl_1;
Servo servo_fl_2;
Servo servo_fr_1;
Servo servo_fr_2;
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
//                   {   5,   5,   5,   5,   5,   5,   5,   5,   5,   5,   5,   5,   5,   5,   5,   5,   6,   6,   6,   7,   7,   7,   8,   9,  10,  10,  11,  11,  12,  12,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  28,  29,  30,  31,  32,  33,  34,  36,  38,  39,  40,  42,  44,  45,  46,  48,  50,  51,  52,  54,  56,  58,  60,  62,  64,  66,  68,  69,  70,  72,  74,  76,  78,  80,  82,  84,  86,  88,  90,  92,  94,  96,  98, 100, 103, 103, 103, 103, 103};
//                   { 105, 105, 105, 105, 105, 102, 100,  98,  96,  94,  92,  90,  88,  86,  84,  83,  82,  80,  78,  76,  75,  73,  72,  71,  70,  68,  66,  64,  63,  61,  60,  58,  57,  55,  54,  52,  51,  49,  48,  47,  45,  44,  43,  41,  40,  38,  37,  36,  34,  32,  31,  30,  29,  27,  26,  25,  24,  23,  22,  21,  20,  19,  18,  17,  16,  15,  14,  13,  12,  11,  10,  10,   9,   8,   8,   7,   7,   7,   6,   6,   6,   5,   5,   5,   5,   5,   5,   5,   5,   5,   5,   5,   5};
//
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
    servo_fl_1.write(limitMotorValue(90 - 30 + m1Value + calibration.fl.motor1));
    servo_fl_2.write(limitMotorValue(90 + 30 - m2Value - calibration.fl.motor2));
    servo_fr_1.write(limitMotorValue(90 + 30 - m1Value - calibration.fr.motor1));
    servo_fr_2.write(limitMotorValue(90 - 30 + m2Value + calibration.fr.motor2));
    servo_rl_1.write(limitMotorValue(90 - 30 + m1Value + calibration.rl.motor1));
    servo_rl_2.write(limitMotorValue(90 + 30 - m2Value - calibration.rl.motor2));
    servo_rr_1.write(limitMotorValue(90 + 30 - m1Value - calibration.rr.motor1));
    servo_rr_2.write(limitMotorValue(90 - 30 + m2Value + calibration.rr.motor2));
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
  // init motor pins
  // motor connection pins
  //   f,  r, fl1, fl2, fr1, fr2, rl1, rl2, rr1, rr2
  //   3,  4,   5,   6,   7,   8,   9,  10,  11,  12
  // 
  // init motors one by one
  servo_st_1.attach(ST1_MOTOR, 500, 2500);
  servo_st_1.write(90 - calibration.st.motor1);
  delay(100);
  servo_st_2.attach(ST2_MOTOR, 500, 2500);
  servo_st_2.write(90 - calibration.st.motor2);
  delay(100);
  servo_fl_1.attach(FL1_MOTOR, 500, 2500);
  servo_fl_1.write(limitMotorValue(90 - 30 + calM1 + calibration.fl.motor1));
  delay(100);
  servo_fr_1.attach(FR1_MOTOR, 500, 2500);
  servo_fr_1.write(limitMotorValue(90 + 30 - calM1 - calibration.fr.motor1));
  delay(100);
  servo_rl_1.attach(RL1_MOTOR, 500, 2500);
  servo_rl_1.write(limitMotorValue(90 - 30 + calM1 + calibration.rl.motor1));
  delay(100);
  servo_rr_1.attach(RR1_MOTOR, 500, 2500);
  servo_rr_1.write(limitMotorValue(90 + 30 - calM1 - calibration.rr.motor1));
  delay(100);
  servo_fl_2.attach(FL2_MOTOR, 500, 2500);
  servo_fl_2.write(limitMotorValue(90 + 30 - calM2 - calibration.fl.motor2));
  delay(100);
  servo_fr_2.attach(FR2_MOTOR, 500, 2500);
  servo_fr_2.write(limitMotorValue(90 - 30 + calM2 + calibration.fr.motor2));
  delay(100);
  servo_rl_2.attach(RL2_MOTOR, 500, 2500);
  servo_rl_2.write(limitMotorValue(90 + 30 - calM2 - calibration.rl.motor2));
  delay(100);
  servo_rr_2.attach(RR2_MOTOR, 500, 2500);
  servo_rr_2.write(limitMotorValue(90 - 30 + calM2 + calibration.rr.motor2));
  delay(100);
  servo_sw_1.attach(SW1_MOTOR, 500, 2500);
  servo_sw_1.write(90 - calibration.sw.motor1);
  delay(100);
  servo_sw_2.attach(SW2_MOTOR, 500, 2500);
  servo_sw_2.write(90 - calibration.sw.motor2);
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
    servo_sw_1.detach();
    servo_sw_2.detach();
    servo_st_1.detach();
    servo_st_2.detach();
    servo_fl_1.detach();
    servo_fr_1.detach();
    servo_rl_1.detach();
    servo_rr_1.detach();
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
  servo_sw_1.write(90 - calibration.sw.motor1);
  servo_sw_2.write(90 - calibration.sw.motor2);
  servo_st_1.write(90 - calibration.st.motor1);
  servo_st_2.write(90 - calibration.st.motor2);
  _moveLinearServo(calibration, calM1, calM2);
}

// move motors.
void updateServo(allMotors calibration, allMotors motorValue, allMotors motorLift, motors centerMotors) {
  servo_sw_1.write(limitMotorValue(90 - (calibration.sw.motor1)));
  servo_sw_2.write(limitMotorValue(90 - (calibration.sw.motor2)));
  servo_st_1.write(limitMotorValue(90 - (centerMotors.motor1 + calibration.st.motor1)));
  servo_st_2.write(limitMotorValue(90 - (centerMotors.motor2 + calibration.st.motor2)));
  servo_fl_1.write(limitMotorValue(90 - 30 + (m1Walk[motorValue.fl.motor1 + m_forwardCenterServo + forwardBallance] + calibration.fl.motor1 + motorLift.fl.motor1 + staticBallance.fl.motor1)));
  servo_fl_2.write(limitMotorValue(90 + 30 - (m2Walk[motorValue.fl.motor2 + m_forwardCenterServo + forwardBallance] + calibration.fl.motor2 + motorLift.fl.motor2 + staticBallance.fl.motor2)));
  servo_fr_1.write(limitMotorValue(90 + 30 - (m1Walk[motorValue.fr.motor1 + m_forwardCenterServo + forwardBallance] + calibration.fr.motor1 + motorLift.fr.motor1 + staticBallance.fr.motor1)));
  servo_fr_2.write(limitMotorValue(90 - 30 + (m2Walk[motorValue.fr.motor2 + m_forwardCenterServo + forwardBallance] + calibration.fr.motor2 + motorLift.fr.motor2 + staticBallance.fr.motor2)));
  servo_rl_1.write(limitMotorValue(90 - 30 + (m1Walk[motorValue.rl.motor1 + m_forwardCenterServo + forwardBallance] + calibration.rl.motor1 + motorLift.rl.motor1 + staticBallance.rl.motor1)));
  servo_rl_2.write(limitMotorValue(90 + 30 - (m2Walk[motorValue.rl.motor2 + m_forwardCenterServo + forwardBallance] + calibration.rl.motor2 + motorLift.rl.motor2 + staticBallance.rl.motor2)));
  servo_rr_1.write(limitMotorValue(90 + 30 - (m1Walk[motorValue.rr.motor1 + m_forwardCenterServo + forwardBallance] + calibration.rr.motor1 + motorLift.rr.motor1 + staticBallance.rr.motor1)));
  servo_rr_2.write(limitMotorValue(90 - 30 + (m2Walk[motorValue.rr.motor2 + m_forwardCenterServo + forwardBallance] + calibration.rr.motor2 + motorLift.rr.motor2 + staticBallance.rr.motor2)));
}

void updateStaticBallanceServo(allMotors cBallance) {
  staticBallance.fl.motor1 = cBallance.fl.motor1;
  staticBallance.fl.motor2 = cBallance.fl.motor2;
  staticBallance.fr.motor1 = cBallance.fr.motor1;
  staticBallance.fr.motor2 = cBallance.fr.motor2;
  staticBallance.rl.motor1 = cBallance.rl.motor1;
  staticBallance.rl.motor2 = cBallance.rl.motor2;
  staticBallance.rr.motor1 = cBallance.rr.motor1;
  staticBallance.rr.motor2 = cBallance.rr.motor2;
}

void setFowardBallanceServo(char ballanceValue) {
  forwardBallance = ballanceValue;
}
