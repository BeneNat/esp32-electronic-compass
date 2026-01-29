#include <Wire.h>
#include <QMC5883LCompass.h>
#include <TFT_eSPI.h>  // Biblioteka do obsługi ekranu TFT

QMC5883LCompass compass;
TFT_eSPI tft = TFT_eSPI(); // Obiekt TFT

#define SCREEN_CENTER_X 120 // Środek ekranu (X)
#define SCREEN_CENTER_Y 120 // Środek ekranu (Y)
#define NEEDLE_LENGTH 80    // Długość igły kompasu

void setup() {
  // Inicjalizacja serial i magnetometru
  Serial.begin(115200);
  Wire.begin();
  compass.init();

  // Inicjalizacja ekranu TFT
  tft.init();
  tft.setRotation(0); // Orientacja ekranu
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
}

void loop() {
  // Odczyt danych z magnetometru
  int x, y, z;
  compass.read();
  x = compass.getX();
  y = compass.getY();
  z = compass.getZ();

  // Obliczenie azymutu (w stopniach)
  float heading = atan2(y, x) * 180 / PI;
  if (heading < 0) {
    heading += 360;
  }

  // Wyczyść ekran i narysuj tarczę kompasu
  tft.fillScreen(TFT_BLACK);
  drawCompassFace();

  // Obliczenie pozycji końca igły kompasu
  float radHeading = heading * PI / 180; // Przelicz kąt na radiany
  int needleX = SCREEN_CENTER_X + NEEDLE_LENGTH * cos(radHeading);
  int needleY = SCREEN_CENTER_Y + NEEDLE_LENGTH * sin(radHeading);

  // Rysowanie igły kompasu
  tft.drawLine(SCREEN_CENTER_X, SCREEN_CENTER_Y, needleX, needleY, TFT_RED); // Główna igła
  tft.fillCircle(SCREEN_CENTER_X, SCREEN_CENTER_Y, 5, TFT_WHITE);           // Środek kompasu

  // Wyświetlenie kierunku i kąta na ekranie
  tft.setCursor(10, 200);
  tft.printf("Kierunek: %.1f deg", heading);

  delay(100); // Odświeżanie co 100 ms
}

// Funkcja rysująca tarczę kompasu
void drawCompassFace() {
  // Narysowanie okręgu kompasu
  tft.drawCircle(SCREEN_CENTER_X, SCREEN_CENTER_Y, 100, TFT_WHITE);

  // Narysowanie oznaczeń głównych kierunków
  tft.setCursor(SCREEN_CENTER_X - 10, SCREEN_CENTER_Y - 110);
  tft.print("N"); // Północ
  tft.setCursor(SCREEN_CENTER_X - 10, SCREEN_CENTER_Y + 100);
  tft.print("S"); // Południe
  tft.setCursor(SCREEN_CENTER_X + 90, SCREEN_CENTER_Y - 10);
  tft.print("E"); // Wschód
  tft.setCursor(SCREEN_CENTER_X - 110, SCREEN_CENTER_Y - 10);
  tft.print("W"); // Zachód
}
