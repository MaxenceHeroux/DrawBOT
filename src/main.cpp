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
  server.handleClient(); //rafraichissement handler (server wifi)

  //Désactiver les moteurs si plus de commande depuis 2 sec
  if (joystickConnecte && millis() - lastJoystickTime > SPLEEP_TIME) {
    joystickConnecte = false;
    Disable_moteur();
  }
  
  if (joystickConnecte) {
    int Vg = 0, Vd = 0;

    if (abs(Joy_Y) < LOWEST_PWM && abs(Joy_X) < LOWEST_PWM) {
      Disable_moteur(); 
      return;
    }

    Enable_moteur();
    // Calcul des vitesses
    Vg = constrain(Joy_Y - Joy_X, -255, 255);
    Vd = constrain(Joy_Y + Joy_X, -255, 255);

    // Moteur gauche
    if (Vg > 0) PWM('G', Vg, false);   // avancer
    else        PWM('G', -Vg, true);   // reculer

    // Moteur droit
    if (Vd > 0) PWM('D', Vd, false);   // avancer
    else        PWM('D', -Vd, true);   // reculer
  }
  

  if(DEBUG){
    //DEBUG_Blink();
    //DEBUG_pwm();
    DEBUG_encodeur();
  }
}
