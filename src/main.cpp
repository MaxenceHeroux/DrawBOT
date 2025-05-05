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


void loop() {
  server.handleClient(); //rafraichissement handler (server wifi)
  Remote();
  
  if(DEBUG){
    Enable_Teleplot();
    //DEBUG_Blink();
    //DEBUG_pwm();
    // DEBUG_encodeur(); //Teleplot
    // DEBUG_IMU();      //Teleplot
    // DEBUG_MAG();      //Teleplot
    DEBUG_angle();    //Teleplot
    //DEBUG_North();    //Teleplot
  }
}
