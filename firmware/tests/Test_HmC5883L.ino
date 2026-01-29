#include <Wire.h>
#include <QMC5883LCompass.h>

QMC5883LCompass compass;

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22); // SDA: GPIO21, SCL: GPIO22 dla ESP32

  // Inicjalizacja QMC5883L
  compass.init();
  //compass.setSamplingRate(50); // Ustaw częstotliwość próbkowania na 50 Hz
  //compass.setScale(2);         // Zakres pomiaru w gaussach (2 gaussy)
  //compass.setOrientation(0);   // Standardowa orientacja czujnika

  Serial.println("QMC5883L zainicjalizowany!");
}

void loop() {
  int x, y, z; // Dane z osi X, Y, Z
  compass.read(); // Odczyt danych z magnetometru
  x = compass.getX();
  y = compass.getY();
  z = compass.getZ();

  // Wyświetlenie danych magnetycznych
  Serial.print("Magnetometr - X: ");
  Serial.print(x);
  Serial.print(" Y: ");
  Serial.print(y);
  Serial.print(" Z: ");
  Serial.println(z);

  delay(1000); // Odczyt co sekundę
}
