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
  if (joystickConnecte && millis() - lastJoystickTime > 2000) {
    joystickConnecte = false;
    Disable_moteur();
  }
  
  if (joystickConnecte) {
    if (abs(Joy_Y) < 20) {
      Disable_moteur();
    }
    else if (Joy_Y < 0) {
      // reculer
      Enable_moteur();
      PWM('D', abs(Joy_Y), true);
      PWM('G', abs(Joy_Y), true);
    }
    else {
      // avancer
      Enable_moteur();
      Serial.print("else");
      PWM('D', abs(Joy_Y), false);
      PWM('G', abs(Joy_Y), false);
    }
  }  

  
  

  if(DEBUG){
    //DEBUG_Blink();
    //DEBUG_pwm();
    DEBUG_encodeur();
  }
}
