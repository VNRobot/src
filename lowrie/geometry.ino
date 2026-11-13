/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
calculates servo motor angle
*/

// nalf distance between motors in mm
#define GEOMETRY_SPACE_L                     17
// extra hight for rear motors
#define GEOMETRY_SPACE_H                     22
// lower leg length
#define LOWER_LEG_NORM                       105
#define LOWER_LEG_LONG                       125
// upper leg length
#define UPPER_LEG_NORM                       70

// structure for extra geometry
typedef struct geometries {
  bool frontEnable;
  bool rearEnable;
  bool extraEnable;
  bool extraFront;
  bool extraRear;
  bool extraExtra;
} geometries;

// extra geometry
geometries geometryParam = {
  false,    // bool frontEnable;
  false,    // bool rearEnable;
  false,    // bool extraEnable;
  false,    // bool extraFront;
  false,    // bool extraRear;
  false     // bool extraExtra;
};
// motors attached flag
bool geometryAttached = false;
// side flipped flags
char flippedL = 1;
char flippedR = 1;
// ride enabled
bool rideEnabled = false;

/*
uses
m_legsValue
*/

// calculate motor 1 and motor 2 angles
short _calculateMotorAngle(int Hval, int Sval, char motorNum) {
  // for first motor mirror shift
  if (motorNum == 1) {
    Sval = -Sval;
  }
  // calculate Lvalue and angles
  Sval -= GEOMETRY_SPACE_L; // nalf distance between motors in mm
  float Lval = (float)sqrt(Hval * Hval + Sval * Sval);
  float AngleC = (float)((acos(Hval / Lval) * 180) / 3.14);
  if (Sval < 0) {
    AngleC = -AngleC;
  }
  float AngleB = (float)((acos((Lval * Lval + UPPER_LEG_NORM * UPPER_LEG_NORM - LOWER_LEG_NORM * LOWER_LEG_NORM) / (2 * Lval * UPPER_LEG_NORM)) * 180) / 3.14);
  //AngleB = (acos((Lval * Lval - 6125) / (Lval * 140)) * 180) / 3.14;
  return (int)(90 - AngleB - AngleC);
}

// calculate motor 1 and motor 2 angles
short _calculateMotorAngleExtra(int Hval, int Sval, char motorNum) {
  // extra hight for rear motors
  Hval += GEOMETRY_SPACE_H;
  // for first motor mirror shift
  if (motorNum == 1) {
    Sval = -Sval;
  }
  // calculate Lvalue and angles
  Sval -= GEOMETRY_SPACE_L; // nalf distance between motors in mm
  float Lval = (float)sqrt(Hval * Hval + Sval * Sval);
  float AngleC = (float)((acos(Hval / Lval) * 180) / 3.14);
  if (Sval < 0) {
    AngleC = -AngleC;
  }
  float AngleB = (float)((acos((Lval * Lval + UPPER_LEG_NORM * UPPER_LEG_NORM - LOWER_LEG_LONG * LOWER_LEG_LONG) / (2 * Lval * UPPER_LEG_NORM)) * 180) / 3.14);
  //AngleB = (acos((Lval * Lval - 6125) / (Lval * 140)) * 180) / 3.14;
  return (int)(90 - AngleB - AngleC);
}

// attach geometry
void attachGeometry(bool frontE, bool rearE, bool extraE, bool extraF, bool extraR, bool extraEx) {
  geometryParam.frontEnable = frontE;
  geometryParam.rearEnable = rearE;
  geometryParam.extraEnable = extraE;
  geometryParam.extraFront = extraF;
  geometryParam.extraRear = extraR;
  geometryParam.extraExtra = extraEx;
  geometryAttached = true;
}

// init geometry
void initGeometry(void) {
}

// move leg motors.
void updateLegsGeometry(void) {
  if (geometryAttached) {
    if (!rideEnabled) {
      if (geometryParam.frontEnable) {
        if (geometryParam.extraFront) {
          m_legsValue.fl.angle1 = flippedL * (_calculateMotorAngleExtra(m_legsValue.fl.hight, m_legsValue.fl.shift, 1));
          m_legsValue.fl.angle2 = - flippedL * (_calculateMotorAngleExtra(m_legsValue.fl.hight, m_legsValue.fl.shift, 2));
          m_legsValue.fr.angle1 = - flippedR * (_calculateMotorAngleExtra(m_legsValue.fr.hight, m_legsValue.fr.shift, 1));
          m_legsValue.fr.angle2 = flippedR * (_calculateMotorAngleExtra(m_legsValue.fr.hight, m_legsValue.fr.shift, 2));
        } else {
          m_legsValue.fl.angle1 = flippedL * (_calculateMotorAngle(m_legsValue.fl.hight, m_legsValue.fl.shift, 1));
          m_legsValue.fl.angle2 = - flippedL * (_calculateMotorAngle(m_legsValue.fl.hight, m_legsValue.fl.shift, 2));
          m_legsValue.fr.angle1 = - flippedR * (_calculateMotorAngle(m_legsValue.fr.hight, m_legsValue.fr.shift, 1));
          m_legsValue.fr.angle2 = flippedR * (_calculateMotorAngle(m_legsValue.fr.hight, m_legsValue.fr.shift, 2));
        }
      }
      if (geometryParam.rearEnable) {
        if (geometryParam.extraRear) {
          m_legsValue.rl.angle1 = flippedL * (_calculateMotorAngleExtra(m_legsValue.rl.hight, m_legsValue.rl.shift, 1));
          m_legsValue.rl.angle2 = - flippedL * (_calculateMotorAngleExtra(m_legsValue.rl.hight, m_legsValue.rl.shift, 2));
          m_legsValue.rr.angle1 = - flippedR * (_calculateMotorAngleExtra(m_legsValue.rr.hight, m_legsValue.rr.shift, 1));
          m_legsValue.rr.angle2 = flippedR * (_calculateMotorAngleExtra(m_legsValue.rr.hight, m_legsValue.rr.shift, 2));
        } else {
          m_legsValue.rl.angle1 = flippedL * (_calculateMotorAngle(m_legsValue.rl.hight, m_legsValue.rl.shift, 1));
          m_legsValue.rl.angle2 = - flippedL * (_calculateMotorAngle(m_legsValue.rl.hight, m_legsValue.rl.shift, 2));
          m_legsValue.rr.angle1 = - flippedR * (_calculateMotorAngle(m_legsValue.rr.hight, m_legsValue.rr.shift, 1));
          m_legsValue.rr.angle2 = flippedR * (_calculateMotorAngle(m_legsValue.rr.hight, m_legsValue.rr.shift, 2));
        }
      }
      if (geometryParam.extraEnable) {
        if (geometryParam.extraExtra) {
          m_legsValue.el.angle1 = flippedL * (_calculateMotorAngleExtra(m_legsValue.el.hight, m_legsValue.el.shift, 1));
          m_legsValue.el.angle2 = - flippedL * (_calculateMotorAngleExtra(m_legsValue.el.hight, m_legsValue.el.shift, 2));
          m_legsValue.er.angle1 = - flippedR * (_calculateMotorAngleExtra(m_legsValue.er.hight, m_legsValue.er.shift, 1));
          m_legsValue.er.angle2 = flippedR * (_calculateMotorAngleExtra(m_legsValue.er.hight, m_legsValue.er.shift, 2));
        } else {
          m_legsValue.el.angle1 = flippedL * (_calculateMotorAngle(m_legsValue.el.hight, m_legsValue.el.shift, 1));
          m_legsValue.el.angle2 = - flippedL * (_calculateMotorAngle(m_legsValue.el.hight, m_legsValue.el.shift, 2));
          m_legsValue.er.angle1 = - flippedR * (_calculateMotorAngle(m_legsValue.er.hight, m_legsValue.er.shift, 1));
          m_legsValue.er.angle2 = flippedR * (_calculateMotorAngle(m_legsValue.er.hight, m_legsValue.er.shift, 2));
        }
      }
    }
  }
}

// set flipped state
void setFlippedGeometry(char flipL, char flipR) {
  if (geometryAttached) {
    flippedL = flipL;
    flippedR = flipR;
  }
}

// enable ride
void rideModeGeometry(bool doRide) {
  if (geometryAttached) {
    if (doRide) {
      if (!rideEnabled) {
        rideEnabled = true;
      }
    } else {
      rideEnabled = false;
    }
  }
}
