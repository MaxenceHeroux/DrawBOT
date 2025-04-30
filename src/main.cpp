#include <lib.h>

void setup() {
  Serial.begin(9600);

  pinMode(LEDU1, OUTPUT); //orange
  pinMode(LEDU2, OUTPUT); //bleu
  //moteurs
  Enable_moteur();
  //PWM
  Enable_PWM();
  //Encodeur
  Enable_encodeur();
  //I2C 

  //Wifi
  Enable_wifi();
}

void loop() {
  server.handleClient(); //wifi

  if(DEBUG){
    //DEBUG_Blink();
    //DEBUG_pwm();
    DEBUG_encodeur();
  }
}
