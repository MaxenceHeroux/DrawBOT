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

int consigne_dist;
int consigne_angle;
int consigne_dist_tps, consigne_angle_tps; //TODO changement coordonnées = reset 

static bool rotation_terminee = false;

void loop() {
  server.handleClient(); //rafraichissement handler (server wifi)
  Remote();
  
  int Commande_MD =0, Commande_MG =0;
  
  consigne_dist = 1000;
  consigne_angle = -45;

  if(!CURVILIGNE && abs(Angle_restriction(consigne_angle *PI/180) - anglerobot)>(5 * PI/180)){ //5 degres de tolerance
    consigne_dist =0;
  }

  Tourner(consigne_angle, 2, 1, 0);
  Avancer(consigne_dist, 0.2, 0, 0); //Pas fair ca faire une XY en cercle et calculer angle et distance pa rapport a la position actuelle en live , pas de fin de pid, qunad robot dans le cercle => changement de position 

  Commande_MD = constrain(commande_pwm_dist_MD + commande_pwm_angle_MD, -255, 255); //rotation positive
  Commande_MG = constrain(commande_pwm_dist_MG + commande_pwm_angle_MG, -255, 255); //rotation negative

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
