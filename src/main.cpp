#include <Arduino.h>

void setup() {
  pinMode(25, OUTPUT);  // Définit GPIO 25 comme une sortie
  pinMode(26, OUTPUT);  // Définit GPIO 25 comme une sortie
}

void loop() {
  digitalWrite(25, HIGH);  // Allume la LED
  digitalWrite(26, HIGH);  // Allume la LED
  delay(500);              // Attend 500 ms
  digitalWrite(25, LOW);   // Éteint la LED
  digitalWrite(26, LOW);   // Éteint la LED
  delay(500);              // Attend 500 ms
}


