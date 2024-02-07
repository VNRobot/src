/*
Board Lowrie 1.1 demo code
Licensed GNU GPLv3 by VN ROBOT INC 2023
Arduino nano
Main file
Demonstrate Lowrie 1.1 board features
 - Operates up to 12 servo motors
 - Reads gyroscope/accelerometer MPU6050
 - Reads power input voltage im mV
 - Reads power current from three independed channels in mA
    channel 1: power current on connectors S1, S2, S3, S4
    channel 2: power current on connectors S5, S6, S7, S8
    channel 3: power current on connectors S9, S10, S11, S12
 - Reads analog sensors raw values
 - Reads Infrared Remote Control Receiver
*/

// data buffer
int dataBuffer = 0;

// runs once on boot or reset
void setup() {
  // Start serial port
  Serial.begin(115200);
  Serial.println("Device started");
  // init servo motors
  initServo();
  // init gyro board MPU6050
  // Uncomment if you have hardware installed
  delay(500); // wait for vibrations to stop
  initGyro();
  delay(200);
  resetGyro();
  // init remote control on S1 connector
  // Uncomment if you have hardware installed
  // initRemote();
  // init ultrasonic sensor HC-SR04
  // Uncomment if you have hardware installed
  // initUltrasonic();
}

// the loop function runs over and over again forever
void loop() {
  // read gyro data
  // Uncomment if you have hardware installed
  readGyro();
  Serial.print(" Direction: ");
  dataBuffer = (int)getDirectionGyro();
  Serial.print(dataBuffer);
  // move servo motors
  updateServo(dataBuffer);
  // read analog inputs
  readInputs();
  Serial.print(" Battery: ");
  Serial.print((int)getBatteryInputs());
  Serial.print(" Current 1: ");
  Serial.print((int)getCurrent1Inputs());
  Serial.print(" Current 2: ");
  Serial.print((int)getCurrent2Inputs());
  Serial.print(" Current 3: ");
  Serial.print((int)getCurrent3Inputs());
  // Uncomment if you have hardware installed
  //Serial.print(" Ultrasonic: ");
  //Serial.print((int)readUltrasonic());
  Serial.print(" left Sensor: ");
  Serial.print((int)getLeftInputs());
  Serial.print(" Right Sensor: ");
  Serial.print((int)getRightInputs());
  // read IR receiver data
  // Uncomment if you have hardware installed
  // dataBuffer = (int)readRemote();
  // if (dataBuffer < 255) {
  //   Serial.print(" Remote command: ");
  //   Serial.print(dataBuffer);
  // }
  Serial.println("");
  // wait for 50 msec
  delay(50);
}