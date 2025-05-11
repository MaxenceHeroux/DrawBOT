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

static bool rotation_terminee = false;

void loop() {
  server.handleClient(); //rafraichissement handler (server wifi)
  Remote();
  
  int Commande_MD =0, Commande_MG =0;

  consigne_dist = 300;
  consigne_angle = 90;
  if (!Tourner(consigne_angle,0.1,0,0)) {
    Commande_MD = commande_pwm_angle_MD;
    Commande_MG = commande_pwm_angle_MG;
    rotation_terminee = false;
    digitalWrite(LEDU1,HIGH);
    digitalWrite(LEDU2,LOW);
  }else {
    if (!rotation_terminee) {
      nb_tic_encodeur_D = 0;
      nb_tic_encodeur_G = 0;
      rotation_terminee = true;
    }
    Avancer(consigne_dist,0.3,0,0.01);
    Commande_MD = commande_pwm_dist_MD;
    Commande_MG = commande_pwm_dist_MG;
    digitalWrite(LEDU1,LOW);
    digitalWrite(LEDU2,HIGH);
  }

  PWM('D', Commande_MD);
  PWM('G', Commande_MG);

  if(DEBUG){
    //DEBUG_Blink();
    // DEBUG_pwm();
    // DEBUG_encodeur();     //Teleplot
    // DEBUG_IMU();          //Teleplot
    // DEBUG_MAG();          //Teleplot
    // DEBUG_angle();        //Teleplot
    // DEBUG_North();        //Teleplot
     DEBUG_PID_distance(consigne_dist); //Teleplot
    // DEBUG_PID_angle(consigne_angle);    //Teleplot
  }
}
