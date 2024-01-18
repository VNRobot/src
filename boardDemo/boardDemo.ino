/*
Board Lowrie 1.1 demo code
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Main file
*/

// data buffer
unsigned char dataBuffer = 255;

// runs once on boot or reset
void setup() {
  // Start serial port
  Serial.begin(115200);
  Serial.println("Device started");
  // init servo motors
  initServo();
  // init gyro board MPU6050
  delay(500); // wait for vibrations to stop
  initGyro();
  delay(200);
  resetGyro();
  // init remote control on S1 connector
  initRemote();
}

// the loop function runs over and over again forever
void loop() {
  // read gyro data
  readGyro();
  Serial.print(" Direction: ");
  Serial.print((int)getDirectionGyro());
  // move servo motors
  updateServo();
  // read analog inputs
  readInputs();
  Serial.print(" Battery: ");
  Serial.print((int)getBatteryInputs());
  Serial.print(" Current 1: ");
  Serial.print((int)getCurrent1Inputs());
  Serial.print(" Current 2: ");
  Serial.print((int)getCurrent2Inputs());
  Serial.print(" Current 3: ");
  Serial.println((int)getCurrent3Inputs());
  Serial.print(" left Sensor: ");
  Serial.print((int)getLeftInputs());
  Serial.print(" Right Sensor: ");
  Serial.println((int)getRightInputs());
  // read IR receiver data
  dataBuffer = readRemote();
  if (dataBuffer < 255) {
    Serial.print(" Remote command: ");
    Serial.println((int)dataBuffer);
  }
  // wait for 50 msec
  delay(50);
}