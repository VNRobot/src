/*
Walking Robot Lowrie
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Read and write eeprom for software version and calibration data
*/

#include <EEPROM.h>

// software version hardcoded
// should be changed manually
unsigned char getSoftwareVersionEeprom(void) {
  return 50;
}

// write software version to eeprom
unsigned char writeSoftwareVersionEeprom(unsigned char version) {
  // software version address is 0
  EEPROM.update(0, version);
}

// read software version from eeprom
unsigned char readSoftwareVersionEeprom() {
  // software version address is 0
  return EEPROM.read(0);
}

// write calibration to eeprom
void writeCalibrationEeprom(void) {
  // calibration address is 1 to 10
  EEPROM.update(1, (unsigned char)(m_calibration.front + 128));
  EEPROM.update(2, (unsigned char)(m_calibration.rear + 128));
  EEPROM.update(3, (unsigned char)(m_calibration.m.fl.motor1 + 128));
  EEPROM.update(4, (unsigned char)(m_calibration.m.fl.motor2 + 128));
  EEPROM.update(5, (unsigned char)(m_calibration.m.fr.motor1 + 128));
  EEPROM.update(6, (unsigned char)(m_calibration.m.fr.motor2 + 128));
  EEPROM.update(7, (unsigned char)(m_calibration.m.rl.motor1 + 128));
  EEPROM.update(8, (unsigned char)(m_calibration.m.rl.motor2 + 128));
  EEPROM.update(9, (unsigned char)(m_calibration.m.rr.motor1 + 128));
  EEPROM.update(10, (unsigned char)(m_calibration.m.rr.motor2 + 128));
}

// read calibration from eeprom
void readCalibrationEeprom(void) {
  // calibration address is 1 to 10
  m_calibration.front = (char)(EEPROM.read(1) - 128);
  m_calibration.rear = (char)(EEPROM.read(2) - 128);
  m_calibration.m.fl.motor1 = (char)(EEPROM.read(3) - 128);
  m_calibration.m.fl.motor2 = (char)(EEPROM.read(4) - 128);
  m_calibration.m.fr.motor1 = (char)(EEPROM.read(5) - 128);
  m_calibration.m.fr.motor2 = (char)(EEPROM.read(6) - 128);
  m_calibration.m.rl.motor1 = (char)(EEPROM.read(7) - 128);
  m_calibration.m.rl.motor2 = (char)(EEPROM.read(8) - 128);
  m_calibration.m.rr.motor1 = (char)(EEPROM.read(9) - 128);
  m_calibration.m.rr.motor2 = (char)(EEPROM.read(10) - 128);
}
