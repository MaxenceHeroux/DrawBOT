#include <lib.h>

#define TEMPS_AVANT_START 15

int consigne_dist;
int consigne_angle;

float consigne_pos_X =0, consigne_pos_Y =0;
float pos_X =-DIST_STYLO, pos_Y =0; 

int i =0;

struct Point {
  float X;
  float Y;
};

//SCENE
Point tableau_points[3] = {
  {0.0, 0.0},
  {1400.0, 150},
  {1700.0, 550}
};

//ZONE1
// Point tableau_points[3] = {
//   {0.0, 0.0},
//   {600, 550},
//   {1050, 550}
// };



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

  //PAMIS
  // delay(85000);

  consigne_pos_X = tableau_points[0].X;
  consigne_pos_Y = tableau_points[0].Y;

  // long int temps = millis();
}



void loop() {
  server.handleClient(); //rafraichissement handler (server wifi)
  Remote();
  
  int Commande_MD =0, Commande_MG =0;

  if(abs(consigne_dist) <20){ 
    i++;
    digitalWrite(LEDU1, HIGH);
    consigne_pos_X = tableau_points[i].X;
    consigne_pos_Y = tableau_points[i].Y;
  
    if(i>=3) {
      while(1){
        digitalWrite(LEDU2, HIGH);
        Disable_moteur();
      }
    }
    // Discretiser(50, 20, i); //modifie les consignes    

    Reset_pid_distance();
    Reset_pid_angle();

    PWM('D',0);
    PWM('G',0);
    delay(500);
    digitalWrite(LEDU1, LOW);
  }

  consigne_dist = sqrt(pow((consigne_pos_X-pos_X),2) + pow((consigne_pos_Y-pos_Y),2)) - DIST_STYLO; 
  consigne_angle = atan2((consigne_pos_Y-pos_Y),(consigne_pos_X-pos_X)) * RAD_TO_DEG;

  Get_angle();
  
  Tourner(consigne_angle, 10, 0.5, 0);   //FIXME : regler coeff et caper les pid I
  Avancer(consigne_dist, 1,0.1,0); 
  
  delay(10); //attention a ne pas retirer !

  if(!CURVILIGNE && abs(Angle_restriction(consigne_angle *PI/180) - anglerobot)>(3 * PI/180)){ //5 degres de tolerance
    commande_pwm_dist_MD =0;
    commande_pwm_dist_MG =0;
  }

  Commande_MD = constrain(commande_pwm_dist_MD + commande_pwm_angle_MD, -HIGHTEST_PWM, HIGHTEST_PWM); //rotation positive
  Commande_MG = constrain(commande_pwm_dist_MG + commande_pwm_angle_MG, -HIGHTEST_PWM, HIGHTEST_PWM); //rotation negative

  PWM('D',Commande_MD);
  PWM('G',Commande_MG);

  if(DEBUG){ //TODO "ifdef etc"
    // DEBUG_Blink();
    // DEBUG_pwm();
    // DEBUG_encodeur();     //Teleplot
    // DEBUG_IMU();          //Teleplot
    // DEBUG_MAG();          //Teleplot
    // DEBUG_angle();        //Teleplot
    // DEBUG_North();        //Teleplot
    // DEBUG_PID_distance(consigne_dist);  //Teleplot
    // DEBUG_PID_angle(consigne_angle);    //Teleplot

    Serial.print(">posx:");
    Serial.println(pos_X);
    Serial.print(">posy:");
    Serial.println(pos_Y);
    Serial.print(">consigne_pox:");
    Serial.println(consigne_pos_X);
    Serial.print(">consigne_poy:");
    Serial.println(consigne_pos_Y);

    Serial.print(">distance:");
    Serial.println(consigne_dist);

    Serial.print(">consigneangle:");
    Serial.println(consigne_angle);
    Serial.print(">angle:");
    Serial.println(anglerobot);
  }
}
