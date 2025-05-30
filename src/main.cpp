#include <lib.h>

#define TEMPS_AVANT_START  1 //85
#define TEMPS_APRES_START 15
#define MICROSWITCH_PIN 15
#define PIN_SERVO 12

int consigne_dist;
int consigne_angle;

float consigne_pos_X =0, consigne_pos_Y =0;
float pos_X =-DIST_STYLO, pos_Y =0; 

int i =0;
int temps =0;
bool isBleu = false;

struct Point {
  float X;
  float Y;
};
Point tableau_points[3];

#define ZONE 1 //pour le delay
//SCENE
Point tableau_pointsJ[3] = {
  {1100.0, 100},
  {1500.0, 50},
  {1500.0, 350}
};
Point tableau_pointsB[3] = {
  {0.0, 0.0},
  {1400.0, -150},
  {1700.0, -550}
};

//ZONE1
// Point tableau_pointsJ[3] = {
//   {0.0, 0.0},
//   {600, 500},
//   {1050, 550}
// };
// Point tableau_pointsB[3] = {
//   {0.0, 0.0},
//   {600, -500},
//   {1050, -550}
// };

//ZONE2
// Point tableau_pointsJ[3] = {
//   {0.0, 0.0},
//   {600, 400},
//   {1800, 550}
// };
// Point tableau_pointsB[3] = {
//   {0.0, 0.0},
//   {600, -400},
//   {1800, -550}
// };

//ZONE3
// Point tableau_pointsJ[3] = {
//   {600, 300},
//   {1300, 450},
//   {2500, 200}
// };

// Point tableau_pointsB[3] = {
//   {600, -300},
//   {1300, -450},
//   {2500, -200}
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

  //PAMIS------------------------
  if (myIMU.readFloatAccelY() >0){
    isBleu = true; // on est en mode bleu
    Serial.println("Mode BLEU");
    digitalWrite(LEDU1, LOW); // on allume la led 1
    digitalWrite(LEDU2, HIGH); // on éteint la led 2
  }else{
    isBleu = false; // on est en mode jaune
    Serial.println("Mode JAUNE");
    digitalWrite(LEDU1, HIGH); // on allume la led 1
    digitalWrite(LEDU2, LOW); // on éteint la led 2
  }
  //SERVO
  ledcSetup(0, 5000, 8);
  ledcAttachPin(PIN_SERVO, 0);
  //TIRETTE
  while(digitalRead(MICROSWITCH_PIN)) { 
    delay(1);
  }
  //PAMIS
  delay(TEMPS_AVANT_START*1000);
  delay(4000*(1/ZONE));

  if(isBleu){
    memcpy(tableau_points, tableau_pointsB, sizeof(tableau_points));
  }else{
    memcpy(tableau_points, tableau_pointsJ, sizeof(tableau_points));
  }

  consigne_pos_X = tableau_points[0].X;
  consigne_pos_Y = tableau_points[0].Y;

  temps = millis();
}


void loop() {
  server.handleClient(); //rafraichissement handler (server wifi)
  Remote();
  
  int Commande_MD =0, Commande_MG =0;

  //COLISION
  while(digitalRead(MICROSWITCH_PIN) == HIGH){
    PWM('D',0);
    PWM('G',0);
    delay(1);
    
    //OUT OF TIME
    if((millis()-temps)>TEMPS_APRES_START*1000){
      while(1){
        digitalWrite(LEDU2, HIGH);
        Disable_moteur();
        //SERVO
        ledcWrite(0, 128); // 50% duty
        delay(1000);
      }
    }
  }

  //CHECK POINT 
  if(abs(consigne_dist) <20){ 
    i++;
    digitalWrite(LEDU1, HIGH);
    consigne_pos_X = tableau_points[i].X;
    consigne_pos_Y = tableau_points[i].Y;
  
    if(i>=3) {
      while(1){
        //OUT OF TIME
        digitalWrite(LEDU2, HIGH);
        Disable_moteur();
        //SERVO
        ledcWrite(0, 128); // 50% duty
        delay(1000);           
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
