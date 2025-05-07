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
  delay(1000);
}


int consigne_dist_MD =0, consigne_dist_MG =0;
int commande_dist_MD =0, commande_dist_MG =0; //sortie du pid

int consigne_rot_MD =0, consigne_rot_MG =0;
int commande_rot_MD =0, commande_rot_MG =0; //sortie du pid


void loop() {
  server.handleClient(); //rafraichissement handler (server wifi)
  Remote();

  // consigne_dist_MD =(int)Ticks_to_Distance(100); 
  // consigne_dist_MG =(int)Ticks_to_Distance(100); 

  // PID_distance(1,0,0.8); //TODO a regler pid distance 

  // int commande_MD = commande_dist_MD;
  // int commande_MG = commande_dist_MG;

  consigne_rot_MD = 760;

  PID_rotation(1.5,0,0);

  int commande_MD = commande_rot_MD;
  int commande_MG = commande_rot_MG;
  
  PWM('D',commande_MD);
  PWM('G',commande_MG);

  if(DEBUG){
    Enable_Teleplot();       //Obligatoire pour run un DEBUG Teleplot ! 
    // DEBUG_Blink();
    // DEBUG_pwm();
    // DEBUG_encodeur();     //Teleplot
    // DEBUG_IMU();          //Teleplot
    // DEBUG_MAG();          //Teleplot
     DEBUG_angle();        //Teleplot
    // DEBUG_North();        //Teleplot
    //DEBUG_PID_distance();    //Teleplot
  }
}
