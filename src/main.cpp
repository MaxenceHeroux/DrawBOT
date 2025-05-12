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
  Wire.begin(SDA,SCL);
  //IMU
  Enable_IMU();
  //Magneto
  Enable_MAG();

  delay(1000); //relax...
}

enum EtatRobot {
  ETAT_TOURNE,
  ETAT_AVANCE,
  ETAT_ARRET
};

EtatRobot etat_actuel = ETAT_TOURNE;

int consigne_dist;
int consigne_angle;

static bool rotation_terminee = false;

void loop() {
  server.handleClient(); //rafraichissement handler (server wifi)
  Remote();
  
  int Commande_MD =0, Commande_MG =0;

  consigne_dist = 1000;
  consigne_angle = 90;

  //FSM
  switch (etat_actuel) {
    case ETAT_TOURNE:
      if (Tourner(consigne_angle, 0.25, 0, 0)) {
        commande_pwm_angle_MD = 0;
        commande_pwm_angle_MG = 0;

        nb_tic_encodeur_D = 0;
        nb_tic_encodeur_G = 0;

        etat_actuel = ETAT_AVANCE;
      }

      Commande_MD = commande_pwm_angle_MD;
      Commande_MG = commande_pwm_angle_MG;
      break;

    case ETAT_AVANCE:
      if (Avancer(consigne_dist, 0.3, 0, 0.01)) {
        commande_pwm_dist_MD = 0;
        commande_pwm_dist_MG = 0;

        nb_tic_encodeur_D = 0;
        nb_tic_encodeur_G = 0;

        etat_actuel = ETAT_ARRET;
      }

      Commande_MD = commande_pwm_dist_MD;
      Commande_MG = commande_pwm_dist_MG;
      break;

    case ETAT_ARRET:
      Commande_MD = 0;
      Commande_MG = 0;
      break;
  }

  
  // Commande_MD = constrain(commande_pwm_dist_MD + commande_pwm_angle_MD, -255, 255); //rotation positive
  // Commande_MG = constrain(commande_pwm_dist_MG + commande_pwm_angle_MG, -255, 255); //rotation negative

  PWM('D',Commande_MD);
  PWM('G',Commande_MG);

  if(DEBUG){
    //DEBUG_Blink();
    // DEBUG_pwm();
    // DEBUG_encodeur();     //Teleplot
    // DEBUG_IMU();          //Teleplot
    // DEBUG_MAG();          //Teleplot
    // DEBUG_angle();        //Teleplot
    // DEBUG_North();        //Teleplot
    DEBUG_PID_distance(consigne_dist); //Teleplot
    DEBUG_PID_angle(consigne_angle);    //Teleplot
  }
}
