#include <lib.h>
Adafruit_LIS3MDL mag;
float mag_offset_x = 0;
float mag_offset_y = 0;
float mag_scale_x = 1;
float mag_scale_y = 1;
bool calibrated = false;


void calibrerMagnetometre(void) {
  mag_offset_x = 0;
  mag_offset_y = 0;
  mag_scale_x = 1;
  mag_scale_y = 1;
  calibrated = true;

  delay(3000);
  float min_x = 9999, max_x = -9999;
  float min_y = 9999, max_y = -9999;

  unsigned long startTime = millis();
  analogWrite(IN_1_D, 150);
  analogWrite(IN_2_D, 0);
  analogWrite(IN_1_G, 150);
  analogWrite(IN_2_G, 0);

  while (millis() - startTime < 7000) {
    sensors_event_t event;
    mag.getEvent(&event);
    float x = event.magnetic.x;
    float y = event.magnetic.y;

    min_x = min(min_x, x); max_x = max(max_x, x);
    min_y = min(min_y, y); max_y = max(max_y, y);
    delay(100);
  }

  Disable_moteur();

  mag_offset_x = (max_x + min_x) / 2.0;
  mag_offset_y = (max_y + min_y) / 2.0;

  float range_x = max_x - min_x;
  float range_y = max_y - min_y;
  float avg_range = (range_x + range_y) / 2.0;

  mag_scale_x = avg_range / range_x;
  mag_scale_y = avg_range / range_y;

  calibrated = true;
}



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
  // Enable_MAG();
  if (!mag.begin_I2C(0x1E, &Wire)) {
    Serial.println("Erreur: LIS3MDL non détecté.");
    while (1);
  }
  calibrerMagnetometre();
}

int consigne_dist;
int consigne_angle;

float consigne_pos_X =0, consigne_pos_Y =0;
float pos_X =-DIST_STYLO, pos_Y =0; 

int i =0;

void loop() {
  server.handleClient(); //rafraichissement handler (server wifi)
  Remote();
  
  int Commande_MD =0, Commande_MG =0;

  if(abs(consigne_dist) <5){ //&& abs(consigne_angle) <10
    i++;
    digitalWrite(LEDU1, HIGH);
    
    Discretiser(); // modifie les consignes    

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
  
  //PID sol
  // Tourner(consigne_angle, 5, 0, 0);   //FIXME : regler coeff et caper les pid I
  // Avancer(consigne_dist, 1,0, 0.05);
  
  //PID ardoise
  Tourner(consigne_angle, 5, 0.02, 0);   //FIXME : regler coeff et caper les pid I
  Avancer(consigne_dist, 2,0.1,0.5); 
  
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
