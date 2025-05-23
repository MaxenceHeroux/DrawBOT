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

  //delay(500); //relax...
}

int consigne_dist;
int consigne_angle;

float consigne_pos_X =0, consigne_pos_Y =0;
float pos_X =0, pos_Y =0;

void loop() {
  server.handleClient(); //rafraichissement handler (server wifi)
  Remote();
  
  int Commande_MD =0, Commande_MG =0;

  consigne_pos_X = 000;
  consigne_pos_Y = 1000;

  consigne_dist = sqrt(pow((consigne_pos_X-pos_X),2) + pow((consigne_pos_Y-pos_Y),2));
  consigne_angle = atan2((consigne_pos_Y-pos_Y),(consigne_pos_X-pos_X)) * RAD_TO_DEG;
  
  // TODO XY vers arg et module + In cercle/zone + Postion absolue MAJ
  // consigne_dist = 800; 
  // consigne_angle = 45; // sans trigo
 
  Get_angle();
  Tourner(consigne_angle, 5, 0, 0);   //FIXME : regler coeff et caper les pid I
  Avancer(consigne_dist, 0.2, 0, 0); 
  
  delay(15); //attention a ne pas retirer !

  if(!CURVILIGNE && abs(Angle_restriction(consigne_angle *PI/180) - anglerobot)>(3 * PI/180)){ //5 degres de tolerance
    commande_pwm_dist_MD =0;
    commande_pwm_dist_MG =0;
  }

  Commande_MD = constrain(commande_pwm_dist_MD + commande_pwm_angle_MD, -HIGHTEST_PWM, HIGHTEST_PWM); //rotation positive
  Commande_MG = constrain(commande_pwm_dist_MG + commande_pwm_angle_MG, -HIGHTEST_PWM, HIGHTEST_PWM); //rotation negative

  PWM('D',Commande_MD);
  PWM('G',Commande_MG);

  if(DEBUG){
    // DEBUG_Blink();
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
