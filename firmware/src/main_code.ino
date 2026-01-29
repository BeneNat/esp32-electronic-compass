#include <Wire.h>
#include <QMC5883LCompass.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <TFT_eSPI.h>
#include <math.h>

QMC5883LCompass compass;  // Compass
Adafruit_MPU6050 mpu;     // Accelerometer
TFT_eSPI tft = TFT_eSPI();  // Display initialization

#define SCREEN_CENTER_X 120 // Screen center (x)
#define SCREEN_CENTER_Y 160 // Screen center (y)
#define COMPASS_CENTER_X 120  // Compass center (x)
#define COMPASS_CENTER_Y 195  // Compass center (y)
#define NEEDLE_LENGTH 80    // Compass needle length

void setup() {
  Serial.begin(115200);
  Wire.begin();

  compass.init();   // Compass initialization
  compass.setCalibrationOffsets(489.00, -693.00, -456.00);  // Calibration offsets
  compass.setCalibrationScales(1.02, 1.02, 0.96);           // Calibration scales
  compass.setSmoothing(8, true);

  mpu.begin();      // Accelerometer initialization
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);   // Set accelerometer range
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);        // Set Gyroscope range
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);     // Set filter bandwidth
  
  // Dispaly settings
  tft.init();   // Display initialization
  tft.setRotation(0);           // Set vertical orientation
  tft.fillScreen(TFT_BLACK);    // Set backgrounf color to black
  tft.setTextColor(TFT_WHITE);  // Set text color to white, second argument set text background color
  tft.setTextSize(2);   // Increase font size 2x

  /*// Test grid
  for(int x=0; x<=240; x+=10){
    tft.drawLine(x, 0, x, 320, TFT_DARKGREY);
  }
  for(int y=0; y<=320; y+=10){
    tft.drawLine(0, y, 240, y, TFT_DARKGREY);
  }*/
}

void loop() {
  int x, y, z, a, b;
	char myArray[3];
  static float lastAzimuth = 0;  // Store last azimuth
	
	compass.read();
  
  // To get the X, Y, or Z sensor readings.
	int magX = compass.getX();
	int magY = compass.getY();
	int magZ = compass.getZ();
	
  //To get the calculated azimuth (compass degree) value.
	a = compass.getAzimuth();
	/*This will divide the 360 range of the compass into 16 parts
  and return a value of 0-15 in clockwise order.
  In this case 0 = N, 4 = E, 8 = S, 12 = W.*/
	b = compass.getBearing(a);

  /*To get a 16 point text representation of the direction the sensor
  is facing you can call getDirection(azimuth);. This will produce a char array[3]
  with letters representing each direction. Because we can't return an array we need
  to pass the values by reference.*/
	compass.getDirection(myArray, a);
  
  // Test printing to consol
	Serial.print("X: ");
	Serial.print(magX);

	Serial.print(" Y: ");
	Serial.print(magY);

	Serial.print(" Z: ");
	Serial.print(magZ);

	Serial.print(" Azimuth: ");
	Serial.print(a);

	Serial.print(" Bearing: ");
	Serial.print(b);

	Serial.print(" Direction: ");
	Serial.print(myArray[0]);   // first component of direction
	Serial.print(myArray[1]);   // second component of direction
	Serial.print(myArray[2]);   // third component of direction

	Serial.println();

  tft.fillRect(150, 20, 40, 16, TFT_BLACK); // Clear display (direction)
  tft.setCursor(30, 20);
  tft.printf("Kierunek: %c%c%c", myArray[0], myArray[1], myArray[2]); // Display direction (XXX)

  // Get accelerometer data
  sensors_event_t accel, gyro, temp;
  mpu.getEvent(&accel, &gyro, &temp);
  float accX = accel.acceleration.x;
  float accY = accel.acceleration.y;
  float accZ = accel.acceleration.z;

  // Variables for tilt compensation
  float roll = atan2(accY, accZ);
  float pitch = atan(-accX / sqrt(accY * accY + accZ * accZ));

  // Compensated X and Y
  float compensatedX = magX * cos(pitch) + magZ * sin(pitch);
  float compensatedY = magX * sin(roll) * sin(pitch) + magY * cos(roll) - magZ * sin(roll) * cos(pitch);

  float azimuth = atan2(compensatedY, compensatedX) * 180 / PI;
  if (azimuth < 0) azimuth += 360;  // Make sure azimuth is [0; 360]

  Serial.print("Azimuth: ");
  Serial.println(azimuth);
  Serial.print("Pitch: ");
  Serial.println(pitch * 180 / PI);
  Serial.print("Roll: ");
  Serial.println(roll * 180 / PI);

  tft.fillRect(150, 50, 50, 16, TFT_BLACK);   // Clear display temp
  tft.setCursor(10, 50);
  tft.printf("Temperatura:%.1f C", temp.temperature); // Display temperature

  // Draw compass face
  tft.drawCircle(COMPASS_CENTER_X, COMPASS_CENTER_Y, NEEDLE_LENGTH + 10, TFT_WHITE);
  tft.drawCircle(COMPASS_CENTER_X, COMPASS_CENTER_Y, NEEDLE_LENGTH + 5, TFT_WHITE);

  // Draw direction markings
  tft.setCursor(COMPASS_CENTER_X - 5, COMPASS_CENTER_Y - NEEDLE_LENGTH - 35);
  tft.print("N");
  tft.setCursor(COMPASS_CENTER_X - 5, COMPASS_CENTER_Y + NEEDLE_LENGTH + 23);
  tft.print("S");
  tft.setCursor(COMPASS_CENTER_X + NEEDLE_LENGTH + 23, COMPASS_CENTER_Y - 8);
  tft.print("E");
  tft.setCursor(COMPASS_CENTER_X - NEEDLE_LENGTH - 32, COMPASS_CENTER_Y - 8);
  tft.print("W");
  
  // Draw direction lines
  //tft.fillRect(COMPASS_CENTER_X - 1, COMPASS_CENTER_Y - NEEDLE_LENGTH - 15, 3, 10, TFT_RED);
  // For "N"
  tft.fillRect(COMPASS_CENTER_X - 2, COMPASS_CENTER_Y - NEEDLE_LENGTH - 15, 5, 10, TFT_RED);
  tft.fillCircle(COMPASS_CENTER_X, COMPASS_CENTER_Y - NEEDLE_LENGTH - 15, 2, TFT_RED);
  tft.fillCircle(COMPASS_CENTER_X, COMPASS_CENTER_Y - NEEDLE_LENGTH - 5, 2, TFT_RED);
  // For "S"
  tft.fillRect(COMPASS_CENTER_X - 2, COMPASS_CENTER_Y + NEEDLE_LENGTH + 5, 5, 10, TFT_WHITE);
  tft.fillCircle(COMPASS_CENTER_X, COMPASS_CENTER_Y + NEEDLE_LENGTH + 15, 2, TFT_WHITE);
  tft.fillCircle(COMPASS_CENTER_X, COMPASS_CENTER_Y + NEEDLE_LENGTH + 5, 2, TFT_WHITE);
  // For "E"
  tft.fillRect(COMPASS_CENTER_X + NEEDLE_LENGTH + 5, COMPASS_CENTER_Y - 2, 10, 5, TFT_WHITE);
  tft.fillCircle(COMPASS_CENTER_X + NEEDLE_LENGTH + 5, COMPASS_CENTER_Y, 2, TFT_WHITE);
  tft.fillCircle(COMPASS_CENTER_X + NEEDLE_LENGTH + 15, COMPASS_CENTER_Y, 2, TFT_WHITE);
  // For "W"
  tft.fillRect(COMPASS_CENTER_X - NEEDLE_LENGTH - 15, COMPASS_CENTER_Y - 2, 10, 5, TFT_WHITE);
  tft.fillCircle(COMPASS_CENTER_X - NEEDLE_LENGTH - 5, COMPASS_CENTER_Y, 2, TFT_WHITE);
  tft.fillCircle(COMPASS_CENTER_X - NEEDLE_LENGTH - 15, COMPASS_CENTER_Y, 2, TFT_WHITE);


  // Draw intermediate direction markings outside the circles
  tft.setCursor(COMPASS_CENTER_X + (NEEDLE_LENGTH + 30) * cos(-45 * PI / 180) - 10, COMPASS_CENTER_Y + (NEEDLE_LENGTH + 30) * sin(-45 * PI / 180) - 8);
  tft.print("NE");

  tft.setCursor(COMPASS_CENTER_X + (NEEDLE_LENGTH + 30) * cos(45 * PI / 180) - 10, COMPASS_CENTER_Y + (NEEDLE_LENGTH + 30) * sin(45 * PI / 180) - 8);
  tft.print("SE");

  tft.setCursor(COMPASS_CENTER_X + (NEEDLE_LENGTH + 30) * cos(135 * PI / 180) - 15, COMPASS_CENTER_Y + (NEEDLE_LENGTH + 30) * sin(135 * PI / 180) - 8);
  tft.print("SW");

  tft.setCursor(COMPASS_CENTER_X + (NEEDLE_LENGTH + 25) * cos(-135 * PI / 180) - 15, COMPASS_CENTER_Y + (NEEDLE_LENGTH + 25) * sin(-135 * PI / 180) - 8);
  tft.print("NW");

  // Draw circles for "NE", "SE", "SW", "NW"
  int intermediateLineLength = 6; // Length of the intermediate direction markers

  // For "NE"
  int neStartX = COMPASS_CENTER_X + (NEEDLE_LENGTH + 5) * cos(-45 * PI / 180);
  int neStartY = COMPASS_CENTER_Y + (NEEDLE_LENGTH + 5) * sin(-45 * PI / 180);
  int neMiddleX = COMPASS_CENTER_X + (NEEDLE_LENGTH + 5 + intermediateLineLength / 2) * cos(-45 * PI / 180);
  int neMiddleY = COMPASS_CENTER_Y + (NEEDLE_LENGTH + 5 + intermediateLineLength / 2) * sin(-45 * PI / 180);
  int neEndX = COMPASS_CENTER_X + (NEEDLE_LENGTH + 5 + intermediateLineLength) * cos(-45 * PI / 180);
  int neEndY = COMPASS_CENTER_Y + (NEEDLE_LENGTH + 5 + intermediateLineLength) * sin(-45 * PI / 180);
  tft.fillCircle(neStartX, neStartY, 2, TFT_WHITE); // First circle
  tft.fillCircle(neMiddleX, neMiddleY, 2, TFT_WHITE); // Second circle
  tft.fillCircle(neEndX, neEndY, 2, TFT_WHITE);   // Last circle

  // For "SE"
  int seStartX = COMPASS_CENTER_X + (NEEDLE_LENGTH + 5) * cos(45 * PI / 180);
  int seStartY = COMPASS_CENTER_Y + (NEEDLE_LENGTH + 5) * sin(45 * PI / 180);
  int seMiddleX = COMPASS_CENTER_X + (NEEDLE_LENGTH + 5 + intermediateLineLength / 2) * cos(45 * PI / 180);
  int seMiddleY = COMPASS_CENTER_Y + (NEEDLE_LENGTH + 5 + intermediateLineLength / 2) * sin(45 * PI / 180);
  int seEndX = COMPASS_CENTER_X + (NEEDLE_LENGTH + 5 + intermediateLineLength) * cos(45 * PI / 180);
  int seEndY = COMPASS_CENTER_Y + (NEEDLE_LENGTH + 5 + intermediateLineLength) * sin(45 * PI / 180);
  tft.fillCircle(seStartX, seStartY, 2, TFT_WHITE); // First circle
  tft.fillCircle(seMiddleX, seMiddleY, 2, TFT_WHITE); // Second circle
  tft.fillCircle(seEndX, seEndY, 2, TFT_WHITE);   // Last circle

  // For "SW"
  int swStartX = COMPASS_CENTER_X + (NEEDLE_LENGTH + 5) * cos(135 * PI / 180);
  int swStartY = COMPASS_CENTER_Y + (NEEDLE_LENGTH + 5) * sin(135 * PI / 180);
  int swMiddleX = COMPASS_CENTER_X + (NEEDLE_LENGTH + 5 + intermediateLineLength / 2) * cos(135 * PI / 180);
  int swMiddleY = COMPASS_CENTER_Y + (NEEDLE_LENGTH + 5 + intermediateLineLength / 2) * sin(135 * PI / 180);
  int swEndX = COMPASS_CENTER_X + (NEEDLE_LENGTH + 5 + intermediateLineLength) * cos(135 * PI / 180);
  int swEndY = COMPASS_CENTER_Y + (NEEDLE_LENGTH + 5 + intermediateLineLength) * sin(135 * PI / 180);
  tft.fillCircle(swStartX, swStartY, 2, TFT_WHITE); // First circle
  tft.fillCircle(swMiddleX, swMiddleY, 2, TFT_WHITE); // Second circle
  tft.fillCircle(swEndX, swEndY, 2, TFT_WHITE);   // Last circle

  // For "NW"
  int nwStartX = COMPASS_CENTER_X + (NEEDLE_LENGTH + 5) * cos(-135 * PI / 180);
  int nwStartY = COMPASS_CENTER_Y + (NEEDLE_LENGTH + 5) * sin(-135 * PI / 180);
  int nwMiddleX = COMPASS_CENTER_X + (NEEDLE_LENGTH + 5 + intermediateLineLength / 2) * cos(-135 * PI / 180);
  int nwMiddleY = COMPASS_CENTER_Y + (NEEDLE_LENGTH + 5 + intermediateLineLength / 2) * sin(-135 * PI / 180);
  int nwEndX = COMPASS_CENTER_X + (NEEDLE_LENGTH + 5 + intermediateLineLength) * cos(-135 * PI / 180);
  int nwEndY = COMPASS_CENTER_Y + (NEEDLE_LENGTH + 5 + intermediateLineLength) * sin(-135 * PI / 180);
  tft.fillCircle(nwStartX, nwStartY, 2, TFT_WHITE); // First circle
  tft.fillCircle(nwMiddleX, nwMiddleY, 2, TFT_WHITE); // Second circle
  tft.fillCircle(nwEndX, nwEndY, 2, TFT_WHITE);   // Last circle

  // Remove old needle
  int oldXNorth = COMPASS_CENTER_X + NEEDLE_LENGTH * cos((lastAzimuth - 90) * PI / 180);
  int oldYNorth = COMPASS_CENTER_Y + NEEDLE_LENGTH * sin((lastAzimuth - 90) * PI / 180);
  int oldXSouth = COMPASS_CENTER_X - NEEDLE_LENGTH * cos((lastAzimuth - 90) * PI / 180);
  int oldYSouth = COMPASS_CENTER_Y - NEEDLE_LENGTH * sin((lastAzimuth - 90) * PI / 180);
  tft.drawLine(COMPASS_CENTER_X, COMPASS_CENTER_Y, oldXNorth, oldYNorth, TFT_BLACK);
  tft.drawLine(COMPASS_CENTER_X, COMPASS_CENTER_Y, oldXSouth, oldYSouth, TFT_BLACK);

  // Draw new needle
  int needleXNorth = COMPASS_CENTER_X + NEEDLE_LENGTH * cos((azimuth - 90) * PI / 180);
  int needleYNorth = COMPASS_CENTER_Y + NEEDLE_LENGTH * sin((azimuth - 90) * PI / 180);
  int needleXSouth = COMPASS_CENTER_X - NEEDLE_LENGTH * cos((azimuth - 90) * PI / 180);
  int needleYSouth = COMPASS_CENTER_Y - NEEDLE_LENGTH * sin((azimuth - 90) * PI / 180);
  tft.drawLine(COMPASS_CENTER_X, COMPASS_CENTER_Y, needleXNorth, needleYNorth, TFT_RED);  // Północ
  tft.drawLine(COMPASS_CENTER_X, COMPASS_CENTER_Y, needleXSouth, needleYSouth, TFT_WHITE); // Południe

  // Save current azimuth to compare in next loop run
  lastAzimuth = azimuth;

  // Center of compass
  tft.fillCircle(COMPASS_CENTER_X, COMPASS_CENTER_Y, 3, TFT_WHITE);

  delay(100);   // Delay 100 [ms]
}