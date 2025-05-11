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
  Enable_IMU(); //relax
  //Magneto
  Enable_MAG();
}

int consigne_dist;
int consigne_angle;
// int i =0;
// enum Etat { ETAT_AVANCE, ETAT_TOURNE, ETAT_ARRET };
// Etat etat = ETAT_AVANCE;

// struct Etape {
//   int distance;
//   int angle;
// };

// Etape Asservissement_test[] = {
//   {1000, 180},
//   {2000, 360},
//   {3000, 520}
// };

// const int NB_ETAPES = sizeof(Asservissement_test) / sizeof(Asservissement_test[0]);

void loop() {
  server.handleClient(); //rafraichissement handler (server wifi)
  Remote();
  
  int Commande_MD =0, Commande_MG =0;

  // if (i < 2) {
  //   consigne_dist = Asservissement_test[i].distance;
  //   consigne_angle = Asservissement_test[i].angle;

  //   switch (etat) {
  //     case ETAT_AVANCE:
  //       if (Avancer(consigne_dist, 0.3, 0.001, 0.2)) {
  //         etat = ETAT_TOURNE;
  //       } else {
  //         Commande_MD = commande_pwm_dist_MD;
  //         Commande_MG = commande_pwm_dist_MG;
  //       }
  //       break;

  //     case ETAT_TOURNE:
  //       if (Tourner(consigne_angle, 0.3, 0, 0)) {
  //         etat = ETAT_ARRET;
  //       } else {
  //         Commande_MD = commande_pwm_angle_MD;
  //         Commande_MG = commande_pwm_angle_MG;
  //       }
  //       break;

  //     case ETAT_ARRET:
  //       Commande_MD = 0;
  //       Commande_MG = 0;
  //       i++;
  //       if (i < 2) {
  //         etat = ETAT_AVANCE; 
  //       }
  //       break;
  //   }
  // } else {
  //   // Séquence terminée
  //   Commande_MD = 0;
  //   Commande_MG = 0;
  // }

  // consigne_dist =1000;
  // if (!Avancer(consigne_dist, 0.3, 0.001, 0.2)) {
  //   Commande_MD = commande_pwm_dist_MD;
  //   Commande_MG = commande_pwm_dist_MG;
  // } else {
  //   Commande_MD = 0;
  //   Commande_MG = 0;
  // }
  
  // PWM('D', Commande_MD);
  // PWM('G', Commande_MG);

  if(DEBUG){
    //DEBUG_Blink();
    DEBUG_pwm();
    // DEBUG_encodeur();     //Teleplot
    // DEBUG_IMU();          //Teleplot
    // DEBUG_MAG();          //Teleplot
    // DEBUG_angle();        //Teleplot
    // DEBUG_North();        //Teleplot
    //DEBUG_PID_distance(consigne_dist); //Teleplot
    //DEBUG_PID_angle(consigne_angle);    //Teleplot
  }
}
