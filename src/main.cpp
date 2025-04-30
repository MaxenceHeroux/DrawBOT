#include <lib.h>


void setup() {
  pinMode(LEDU1, OUTPUT); //orange
  pinMode(LEDU2, OUTPUT); //bleu
  //moteurs
  Enable_moteur();
  //PWM
  Enable_PWM();
  //Encodeur
  pinMode(ENC_D_CH_A, INPUT);
  pinMode(ENC_D_CH_A, INPUT);
  pinMode(ENC_G_CH_A, INPUT);
  pinMode(ENC_G_CH_B, INPUT);
  //I2C 

}

void loop() {
  if(DEBUG){
    DEBUG_Blink();
    DEBUG_pwm();
  }
}
