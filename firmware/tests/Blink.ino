#ifndef LED_BUILTIN
#define LED_BUILTIN 2  // Jeśli nie ma LED_BUILTIN, ustaw pin na 2 (dla ESP32)
#endif

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);
}
