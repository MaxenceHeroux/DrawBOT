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

  delay(500); //relax...
}

float consigne_dist;
float consigne_angle;

int consigne_pos_X =0, consigne_pos_Y =0;
int pos_X =0, pos_Y =0;

void loop() {
  server.handleClient(); //rafraichissement handler (server wifi)
  Remote();
  
  int Commande_MD =0, Commande_MG =0;

  consigne_pos_X = 100; //mm
  consigne_pos_Y = 100; //mm

  //TODO Actualiser_co (precedent et actuel)

  consigne_dist = sqrt(pow((consigne_pos_X-pos_X),2) + pow((consigne_pos_Y-pos_Y),2)); // ne pas oublier precedent
  consigne_angle = atan2((consigne_pos_Y-pos_Y),(consigne_pos_X-pos_Y)) * RAD_TO_DEG;
  
  //FIXME : regler coeff et caper les pid I
  Tourner(consigne_angle, 1.2, 0, 0); 
  Avancer(consigne_dist, 0.2, 0, 0); // Pas fair ca faire une XY en cercle et calculer angle et distance pa rapport a la position actuelle en live , pas de fin de pid, qunad robot dans le cercle => changement de position 

  if(abs(consigne_pos_X-pos_X)<5 && abs(consigne_pos_Y-pos_Y)<5){
    consigne_pos_X = 0;
    consigne_pos_Y = 0;
  }

  if(!CURVILIGNE && abs(Angle_restriction(consigne_angle *PI/180) - anglerobot)>(3 * PI/180)){ //5 degres de tolerance
    commande_pwm_dist_MD =0;
    commande_pwm_dist_MG =0;
  }

  Commande_MD = constrain(commande_pwm_dist_MD + commande_pwm_angle_MD, -HIGHTEST_PWM, HIGHTEST_PWM); //rotation positive
  Commande_MG = constrain(commande_pwm_dist_MG + commande_pwm_angle_MG, -HIGHTEST_PWM, HIGHTEST_PWM); //rotation negative

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
