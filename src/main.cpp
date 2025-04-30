#include <lib.h>

void setup() {
  pinMode(LEDU1, OUTPUT);
  pinMode(LEDU2, OUTPUT);
  //moteurs
  pinMode(EN_D, OUTPUT);
  pinMode(EN_G, OUTPUT);
  //PWM
  pinMode(IN_1_D, OUTPUT);
  pinMode(IN_2_D, OUTPUT);
  pinMode(IN_1_G, OUTPUT);
  pinMode(IN_2_G, OUTPUT);
  //Encodeur
  pinMode(ENC_D_CH_A, INPUT);
  pinMode(ENC_D_CH_A, INPUT);
  pinMode(ENC_G_CH_A, INPUT);
  pinMode(ENC_G_CH_B, INPUT);
  //I2C 

}

void loop() {
  Blink();
}