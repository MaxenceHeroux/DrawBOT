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
  int commande_MD, commande_MG;

  if(millis()<10000){ //TODO  ajouter une rampe 28:06 min  + angle correction 34:05 min 
    //DISTANCE
    consigne_dist_MD =(int)Ticks_to_Distance(300);  
    consigne_dist_MG =(int)Ticks_to_Distance(300); 

    PID_distance(0.3,0.001,0.2); //TODO a regler pid distance 

    commande_MD = commande_dist_MD;
    commande_MG = commande_dist_MG; 

    PWM('D',commande_MD);
    PWM('G',commande_MG);
  }else{
    //ANGLE
    consigne_rot_MD = -45;

    PID_rotation(1.5,0,0); //TODO regler angle (dead zone IMU ?) if roue ne tourne pas alors Ki = 0

    commande_MD = commande_rot_MD;
    commande_MG = commande_rot_MG;
    
    PWM('D',commande_MD);
    PWM('G',commande_MG);
  }

  // consigne_dist_MD =(int)Ticks_to_Distance(0);  
  // consigne_dist_MG =(int)Ticks_to_Distance(0); 
  // consigne_rot_MD = -90;
  // PID_distance(1,0,0.8);
  // PID_rotation(1.5,0,0);

  // commande_MD = commande_dist_MD - commande_rot_MD;
  // commande_MG = commande_dist_MG + commande_rot_MG; 

  // int max_val = max(abs(commande_MG), abs(commande_MD));
  // if (max_val > 255) {
  //     float scale = 255.0 / max_val;
  //     commande_MD *= scale;
  //     commande_MG *= scale;
  // }

  // commande_MD = constrain((int)commande_MD, -255, 255);
  // commande_MG = constrain((int)commande_MG, -255, 255);

  // if (abs(commande_MD) < LOWEST_PWM) commande_MD = 0;
  // if (abs(commande_MG) < LOWEST_PWM) commande_MG = 0;

  // PWM('D',commande_MD);
  // PWM('G',commande_MG);


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
