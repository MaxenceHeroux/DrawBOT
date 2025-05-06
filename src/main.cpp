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
}

int commande_MD =0, commande_MG =0;
int consigne_MD =0, consigne_MG =0;
int commande_ticks_MD =0, commande_ticks_MG =0;

void loop() {
  server.handleClient(); //rafraichissement handler (server wifi)
  Remote();

  consigne_MD =(int)Ticks_to_Distance(100); 
  consigne_MG =(int)Ticks_to_Distance(100); 

  PID_distance(1,0,0.85); //TODO a regler pid distance 

  PWM('D',commande_MD);
  PWM('G',commande_MG);    

  if(DEBUG){
    Enable_Teleplot();       //Obligatoire pour run un DEBUG Teleplot ! 
    // DEBUG_Blink();
    // DEBUG_pwm();
    // DEBUG_encodeur();     //Teleplot
    // DEBUG_IMU();          //Teleplot
    // DEBUG_MAG();          //Teleplot
    // DEBUG_angle();        //Teleplot
    // DEBUG_North();        //Teleplot
    DEBUG_PID_distance();    //Teleplot
  }
}
