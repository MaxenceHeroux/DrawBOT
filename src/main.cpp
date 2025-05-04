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

void loop() {
  server.handleClient(); //rafraichissement handler (server wifi)
  Remote();

  Enable_Teleplot();

  consigne_MD =200;
  consigne_MG =100;

  Serial.print(">Commande_moteur_attendu:");
  Serial.println(consigne_MD);

  if (Mini_boucle()){ //calcul des vitesses avant le PID
    ENC_vitesse();
    PID(1.1,0.001,-0.5); //TODO PID a regeler avec le poids du robot (pour l instant regler a vide)
  }  

  PWM('D',commande_MD);
  PWM('G',commande_MG);  

  DEBUG_encodeur();   

  if(DEBUG){
    Enable_Teleplot();
    //DEBUG_Blink();
    //DEBUG_pwm();
    DEBUG_encodeur(); //Teleplot
    DEBUG_IMU();      //Teleplot
    DEBUG_MAG();      //Teleplot
    DEBUG_North();    //Teleplot
  }
}
