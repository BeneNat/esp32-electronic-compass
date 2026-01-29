#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

Adafruit_MPU6050 mpu;

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22); // SDA: GPIO21, SCL: GPIO22

  if (!mpu.begin()) {
    Serial.println("Nie odnaleziono akcelerometru MPU6050!");
    while (1);
  }
  Serial.println("Akcelerometr MPU6050 znaleziony!");
}

void loop() {
  sensors_event_t accel, gyro, temp;
  mpu.getEvent(&accel, &gyro, &temp);

  Serial.print("Akcelerometr - X: ");
  Serial.print(accel.acceleration.x);
  Serial.print(" Y: ");
  Serial.print(accel.acceleration.y);
  Serial.print(" Z: ");
  Serial.println(accel.acceleration.z);

  delay(1000);
}
