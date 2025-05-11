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
int consigne_dist_prec =0, consigne_angle_prec=0;

enum Etat { ETAT_AVANCE, ETAT_TOURNE, ETAT_ARRET };
Etat etat = ETAT_AVANCE;

void loop() {
  server.handleClient(); //rafraichissement handler (server wifi)
  Remote();

  int Commande_MD =0, Commande_MG =0;

  consigne_dist = (int)Kp;
  consigne_angle =(int)Ki;

  if (consigne_dist != consigne_dist_prec || consigne_angle_prec != consigne_angle) {
    etat = ETAT_AVANCE;
    Reset_pid_angle();
    Reset_pid_distance();
  }

  consigne_dist_prec =  consigne_dist;
  consigne_angle_prec = consigne_angle;

  //FSM
  switch (etat) {
    case ETAT_AVANCE:
      if (Avancer(consigne_dist, 0.3, 0.001, 0.2)) {
        etat = ETAT_TOURNE;
      } else {
        Commande_MD = commande_pwm_dist_MD;
        Commande_MG = commande_pwm_dist_MG;
      }
      break;

    case ETAT_TOURNE:
      if (Tourner(consigne_angle, 0.3, 0, 0)) {
        etat = ETAT_ARRET;
      } else {
        Commande_MD = commande_pwm_angle_MD;
        Commande_MG = commande_pwm_angle_MG;
      }
      break;

    case ETAT_ARRET:
      Commande_MD = 0;
      Commande_MG = 0;
      break;
  }

  PWM('D', Commande_MD);
  PWM('G', Commande_MG);

  if(DEBUG){
    // DEBUG_Blink();
    // DEBUG_pwm();
    // DEBUG_encodeur();     //Teleplot
     DEBUG_IMU();          //Teleplot
     DEBUG_MAG();          //Teleplot
     DEBUG_angle();        //Teleplot
    // DEBUG_North();        //Teleplot
    DEBUG_PID_distance(consigne_dist); //Teleplot
    DEBUG_PID_angle(consigne_angle);    //Teleplot
  }
}
