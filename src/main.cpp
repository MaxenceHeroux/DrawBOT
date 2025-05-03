#include <lib.h>

void setup() {
  Serial.begin(9600);
  //LED
  pinMode(LEDU1, OUTPUT); //orange
  pinMode(LEDU2, OUTPUT); //bleu
  //moteurs
  Enable_moteur();
  //PWM
  Enable_PWM();
  //Encodeur
  Enable_encodeur();
  //Wifi
  Enable_wifi();
  //I2C 
  //Wire.begin(SDA,SCL);
  //IMU
  Enable_IMU();
}

void loop() {
  server.handleClient(); //rafraichissement handler (server wifi)
  Remote();
  
  
  if(DEBUG){
    //DEBUG_Blink();
    //DEBUG_pwm();
    //DEBUG_encodeur();
    DEBUG_IMU();
  }
}
