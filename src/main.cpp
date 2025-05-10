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


int consigne_dist_MD =0, consigne_dist_MG =0;
int commande_dist_MD =0, commande_dist_MG =0; //sortie du pid

int consigne_rot_MD =0, consigne_rot_MG =0;
int commande_rot_MD =0, commande_rot_MG =0; //sortie du pid


void loop() {
  server.handleClient(); //rafraichissement handler (server wifi)
  Remote();
  
  DEBUG_asservissement(1000,45);

  if(DEBUG){
    // DEBUG_Blink();
    // DEBUG_pwm();
    // DEBUG_encodeur();     //Teleplot
    // DEBUG_IMU();          //Teleplot
    // DEBUG_MAG();          //Teleplot
    // DEBUG_angle();        //Teleplot
    DEBUG_North();        //Teleplot
    DEBUG_PID_distance(); //Teleplot
    DEBUG_PID_angle();    //Teleplot
  }
}
