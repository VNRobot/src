/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Read and write eeprom for software version and calibration data
*/

#include <EEPROM.h>

// software version hardcoded
// should be changed manually
unsigned char _version = 50;

// get software version
unsigned char getSoftwareVersionEeprom(void) {
  return _version;
}

// write software version to eeprom
void writeSoftwareVersionEeprom(void) {
  // software version address is 0
  EEPROM.update(0, _version);
}

// read software version from eeprom
unsigned char readSoftwareVersionEeprom() {
  // software version address is 0
  return EEPROM.read(0);
}

// write calibration to eeprom
void writeCalibrationEeprom(allMotors calibration) {
  // calibration address is 1 to 10
  EEPROM.update(1, (unsigned char)(calibration.front + 128));
  EEPROM.update(2, (unsigned char)(calibration.rear + 128));
  EEPROM.update(3, (unsigned char)(calibration.m.fl.motor1 + 128));
  EEPROM.update(4, (unsigned char)(calibration.m.fl.motor2 + 128));
  EEPROM.update(5, (unsigned char)(calibration.m.fr.motor1 + 128));
  EEPROM.update(6, (unsigned char)(calibration.m.fr.motor2 + 128));
  EEPROM.update(7, (unsigned char)(calibration.m.rl.motor1 + 128));
  EEPROM.update(8, (unsigned char)(calibration.m.rl.motor2 + 128));
  EEPROM.update(9, (unsigned char)(calibration.m.rr.motor1 + 128));
  EEPROM.update(10, (unsigned char)(calibration.m.rr.motor2 + 128));
}

// read calibration from eeprom
void readCalibrationEeprom(allMotors * calibration) {
  // calibration address is 1 to 10
  calibration->front = (char)(EEPROM.read(1) - 128);
  calibration->rear = (char)(EEPROM.read(2) - 128);
  calibration->m.fl.motor1 = (char)(EEPROM.read(3) - 128);
  calibration->m.fl.motor2 = (char)(EEPROM.read(4) - 128);
  calibration->m.fr.motor1 = (char)(EEPROM.read(5) - 128);
  calibration->m.fr.motor2 = (char)(EEPROM.read(6) - 128);
  calibration->m.rl.motor1 = (char)(EEPROM.read(7) - 128);
  calibration->m.rl.motor2 = (char)(EEPROM.read(8) - 128);
  calibration->m.rr.motor1 = (char)(EEPROM.read(9) - 128);
  calibration->m.rr.motor2 = (char)(EEPROM.read(10) - 128);
}
