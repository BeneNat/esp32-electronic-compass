#include <Wire.h>
#include <QMC5883LCompass.h>

QMC5883LCompass compass;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  compass.init();
  Serial.println("Magnetometr zainicjalizowany.");
}

void loop() {
  int x, y, z;
  compass.read();  // Odczyt z magnetometru
  x = compass.getX();
  y = compass.getY();
  z = compass.getZ();

  // Obliczanie kąta azymutu
  float heading = atan2(y, x) * 180 / PI;
  if (heading < 0) {
    heading += 360;
  }

  // Wyświetlanie kąta i kierunku
  Serial.print("Kąt azymutu: ");
  Serial.print(heading);
  Serial.print("° (");
  
  if (heading >= 337.5 || heading < 22.5) {
    Serial.println("Północ)");
  } else if (heading >= 22.5 && heading < 67.5) {
    Serial.println("Północny Wschód)");
  } else if (heading >= 67.5 && heading < 112.5) {
    Serial.println("Wschód)");
  } else if (heading >= 112.5 && heading < 157.5) {
    Serial.println("Południowy Wschód)");
  } else if (heading >= 157.5 && heading < 202.5) {
    Serial.println("Południe)");
  } else if (heading >= 202.5 && heading < 247.5) {
    Serial.println("Południowy Zachód)");
  } else if (heading >= 247.5 && heading < 292.5) {
    Serial.println("Zachód)");
  } else if (heading >= 292.5 && heading < 337.5) {
    Serial.println("Północny Zachód)");
  }

  delay(1000);  // Odczyt co sekundę
}
