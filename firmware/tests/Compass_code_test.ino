#include <QMC5883LCompass.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_MPU6050.h>
#include <TFT_eSPI.h>

// Obiekty czujników i ekranu
QMC5883LCompass compass;
Adafruit_MPU6050 mpu;
TFT_eSPI tft = TFT_eSPI();

// Kalibracja magnetometru
float minX = -1000, maxX = 1000;
float minY = -1000, maxY = 1000;

float previousHeading = -1;

void setup() {
  Serial.begin(115200);

  // Inicjalizacja ekranu TFT
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);

  // Inicjalizacja magnetometru
  compass.init();
  compass.setCalibration(-1000, 1000, -1000, 1000, -1000, 1000);

  // Inicjalizacja MPU6050
  if (!mpu.begin()) {
    Serial.println("Nie udało się znaleźć MPU6050");
    while (1);
  }
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
}

void loop() {
  // Odczyt danych z magnetometru i akcelerometru
  compass.read();
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  float X = compass.getX();
  float Y = compass.getY();
  float Z = compass.getZ();

  // Kalibracja
  float calibratedX = (X - (minX + maxX) / 2.0) / (maxX - minX);
  float calibratedY = (Y - (minY + maxY) / 2.0) / (maxY - minY);

  // Kompensacja przechyłu
  float pitch = atan2(a.acceleration.y, sqrt(a.acceleration.x * a.acceleration.x + a.acceleration.z * a.acceleration.z));
  float roll = atan2(-a.acceleration.x, a.acceleration.z);

  float Xh = calibratedX * cos(pitch) + Z * sin(pitch);
  float Yh = calibratedY * cos(roll) - Z * sin(roll) * cos(pitch);

  // Obliczanie azymutu
  float heading = atan2(Yh, Xh) * 180 / PI;
  if (heading < 0) heading += 360;

  // Rysowanie tylko w przypadku zmiany
  if (abs(heading - previousHeading) > 1) {
    displayCompass(heading);
    previousHeading = heading;
  }

  delay(50);
}

void displayCompass(float heading) {
  int centerX = tft.width() / 2;
  int centerY = tft.height() / 2;
  int needleLength = 50;

  int needleX = centerX + needleLength * cos(heading * PI / 180);
  int needleY = centerY - needleLength * sin(heading * PI / 180);

  // Czyszczenie poprzedniej igły
  tft.fillCircle(centerX, centerY, needleLength + 2, TFT_BLUE);

  // Rysowanie nowej igły
  tft.drawLine(centerX, centerY, needleX, needleY, TFT_RED);
  tft.drawCircle(centerX, centerY, 3, TFT_WHITE);
  tft.setCursor(10, 10);
  tft.printf("Kierunek: %.1f", heading);
}
