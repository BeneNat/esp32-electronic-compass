#include <Wire.h>

void setup() {
  Wire.begin(21, 22); // SDA: GPIO21, SCL: GPIO22
  Serial.begin(115200);
  Serial.println("Skanowanie I2C...");
  
  for (uint8_t address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    if (Wire.endTransmission() == 0) {
      Serial.print("Znaleziono urządzenie I2C pod adresem 0x");
      Serial.println(address, HEX);
    }
  }
}

void loop() {}