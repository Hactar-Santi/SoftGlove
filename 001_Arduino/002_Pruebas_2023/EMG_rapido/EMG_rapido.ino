/*
  Myoware Muscle Sensor Test
  myoware-test-plotter.ino
  Demo of Myoware Muscle Sensor
  Output to Serial Monitor
  Use battery-powered computer or USB isolator for safety!
  DroneBot Workshop 2021
  https://dronebotworkshop.com
*/

// Connection to Myoware sensor
#define SENSOR_PIN 0
#define SENSOR_PIN2 8


// Integer for sensor value
int sensorValue;
int sensorValue2;


void setup() {

  // Set up serial port
  Serial.begin(57600);
}

void loop() {

  // Read sensor value
  sensorValue = analogRead(SENSOR_PIN);
  sensorValue2 = analogRead(SENSOR_PIN2);


  // Add "fake" plots to stabilize Y axis
  Serial.print(0); // To freeze the lower limit
  Serial.print(" ");
  Serial.print(1000); // To freeze the upper limit
  Serial.print(" ");
  Serial.print(sensorValue); // To freeze the upper limit
  Serial.print(" ");

  // Print value to Serial Monitor
  Serial.println(sensorValue2);
}
