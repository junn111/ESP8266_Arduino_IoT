#include <Arduino.h>

#define RELAY_PIN 2

// ESP8266 웹 서버 및 UART 기능은 향후 구현합니다.
void setup() {
  Serial.begin(9600);

  digitalWrite(RELAY_PIN, HIGH);
  pinMode(RELAY_PIN, OUTPUT);
}

void loop() {
  digitalWrite(RELAY_PIN, LOW);
  Serial.println("Relay ON");
  delay(1000);

  digitalWrite(RELAY_PIN, HIGH);
  Serial.println("Relay OFF");
  delay(1000);
}