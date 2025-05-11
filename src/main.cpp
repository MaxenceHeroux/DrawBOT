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

void loop() {
  server.handleClient(); //rafraichissement handler (server wifi)
  Remote();

  consigne_dist = (int)Kp;
  if (Avancer(consigne_dist,0.3,0.001,0.2)){
    digitalWrite(LEDU1,HIGH);
  }else {
    digitalWrite(LEDU1,LOW);
    digitalWrite(LEDU2,HIGH);
  }

  if(DEBUG){
    // DEBUG_Blink();
    // DEBUG_pwm();
    // DEBUG_encodeur();     //Teleplot
    // DEBUG_IMU();          //Teleplot
    // DEBUG_MAG();          //Teleplot
    //DEBUG_angle();        //Teleplot
    // DEBUG_North();        //Teleplot
    DEBUG_PID_distance(consigne_dist); //Teleplot
    //DEBUG_PID_angle();    //Teleplot
  }
}
