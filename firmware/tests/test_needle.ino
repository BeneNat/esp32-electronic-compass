#include <TFT_eSPI.h>
#include <math.h> 

TFT_eSPI tft = TFT_eSPI();

float previousHeading = -1; // Przechowuje poprzedni kąt igły

void drawCompass(float heading) {
  int centerX = tft.width() / 2;
  int centerY = tft.height() / 2;
  int needleLength = 60;
  int needleWidth = 10;

  // Czyszczenie starej igły (jeśli istnieje poprzedni kąt)
  if (previousHeading >= 0) {
    float angle = previousHeading * PI / 180;
    int x1 = centerX + needleLength * cos(angle);
    int y1 = centerY - needleLength * sin(angle);
    int x2 = centerX + needleWidth * cos(angle + PI / 2);
    int y2 = centerY - needleWidth * sin(angle + PI / 2);
    int x3 = centerX + needleWidth * cos(angle - PI / 2);
    int y3 = centerY - needleWidth * sin(angle - PI / 2);

    // Rysuj starą igłę w kolorze tła
    tft.fillTriangle(centerX, centerY, x2, y2, x3, y3, TFT_GREEN); // Kolor tła
    tft.fillTriangle(centerX, centerY, x1, y1, x3, y3, TFT_GREEN); // Kolor tła
  }

  // Obliczenie nowego kąta igły
  float angle = heading * PI / 180;
  int x1 = centerX + needleLength * cos(angle);
  int y1 = centerY - needleLength * sin(angle);
  int x2 = centerX + needleWidth * cos(angle + PI / 2);
  int y2 = centerY - needleWidth * sin(angle + PI / 2);
  int x3 = centerX + needleWidth * cos(angle - PI / 2);
  int y3 = centerY - needleWidth * sin(angle - PI / 2);

  // Rysowanie nowej igły
  tft.fillTriangle(centerX, centerY, x2, y2, x3, y3, TFT_RED);
  tft.fillTriangle(centerX, centerY, x1, y1, x3, y3, TFT_RED);

  // Zapisz aktualny kąt jako poprzedni
  previousHeading = heading;
}

void setup() {
  tft.init();
  tft.setRotation(1); 
  tft.fillScreen(TFT_GREEN); // Stałe tło kompasu
}

void loop() {
  static float heading = 0;

  // Rysowanie kompasu z nowym kątem
  drawCompass(heading);

  // Zwiększ kąt
  heading += 1; 
  if (heading >= 360) heading = 0;

  delay(50); // Krótkie opóźnienie
}
