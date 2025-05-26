#include <lib.h>
int signe(int val) {
    if (val > 0) return 1;
    if (val < 0) return -1;
    return 0;
}

void DEBUG_Blink(void){
    digitalWrite(LEDU1, HIGH); 
    digitalWrite(LEDU2, HIGH);  
    delay(500);             
    digitalWrite(LEDU1, LOW); 
    digitalWrite(LEDU2, LOW);  
    delay(500);             
}

void Enable_moteur(void){
    pinMode(EN_D, OUTPUT);
    pinMode(EN_G, OUTPUT);
    //enable
    digitalWrite(EN_D, HIGH);
    digitalWrite(EN_G, HIGH);
}

void Disable_moteur(void){
    pinMode(EN_D, OUTPUT);
    pinMode(EN_G, OUTPUT);
    //enable
    digitalWrite(EN_D, LOW);
    digitalWrite(EN_G, LOW);
}

void Enable_PWM(void){
    //PWM moteur 1
    pinMode(IN_1_D, OUTPUT);
    pinMode(IN_2_D, OUTPUT);
    ledcSetup(pwmChannel_MD_1, freq, resolution);
    ledcAttachPin(IN_1_D, pwmChannel_MD_1);
    ledcSetup(pwmChannel_MD_2, freq, resolution);
    ledcAttachPin(IN_2_D, pwmChannel_MD_2);

    //PWM moteur 2
    pinMode(IN_1_G, OUTPUT);
    pinMode(IN_2_G, OUTPUT);
    ledcSetup(pwmChannel_MG_1, freq, resolution);
    ledcAttachPin(IN_1_G, pwmChannel_MG_1);
    ledcSetup(pwmChannel_MG_2, freq, resolution);
    ledcAttachPin(IN_2_G, pwmChannel_MG_2);
}

void PWM(char moteur, int DutyCycle){ //pwm >0 = avancer
    boolean Direction = true; 
    DutyCycle = constrain(DutyCycle, -HIGHTEST_PWM, HIGHTEST_PWM);
    if (DutyCycle <0) { //si pwm negatif alors inversion de la direction 
        Direction = ! Direction;
    }
    if (moteur == 'D'){
        if (Direction){
            ledcWrite(pwmChannel_MD_1, 0);                  //(Avancer)
            ledcWrite(pwmChannel_MD_2, abs(DutyCycle));
        }else{
            ledcWrite(pwmChannel_MD_1, abs(DutyCycle));     //(Reculer)
            ledcWrite(pwmChannel_MD_2, 0);
        }
    }else{
        if (Direction){
            ledcWrite(pwmChannel_MG_1, abs(DutyCycle));     //(Avancer)
            ledcWrite(pwmChannel_MG_2, 0);
        }else{
            ledcWrite(pwmChannel_MG_1, 0);                  //(Reculer)
            ledcWrite(pwmChannel_MG_2, abs(DutyCycle));
        }
    }
}

void DEBUG_pwm(void){
    PWM('D',255);
    PWM('G',255);
    delay(500);
    PWM('D',-255);
    PWM('G',-255);
    delay(500);
}

void IRAM_ATTR ENC_ISIR_D() {
    bool A = digitalRead(ENC_D_CH_A);
    bool B = digitalRead(ENC_D_CH_B);
  
    // Si A a changé, on regarde B pour le sens
    if (A == B) {
        nb_tic_encodeur_D++;    // Sens avant
    } else {
        nb_tic_encodeur_D--;    // Sens arrière
    }
}

void IRAM_ATTR ENC_ISIR_G() {
    bool A = digitalRead(ENC_G_CH_A);
    bool B = digitalRead(ENC_G_CH_B);
  
    // Si A a changé, on regarde B pour le sens
    if (A == B) {
        nb_tic_encodeur_G--;    // Sens arrière
    } else {
        nb_tic_encodeur_G++;    // Sens avant
    }
}

long nb_tic_encodeur_D;
long nb_tic_encodeur_G;
void Enable_encodeur(void){
    pinMode(ENC_D_CH_A, INPUT_PULLUP);
    pinMode(ENC_D_CH_A, INPUT_PULLUP);
    pinMode(ENC_G_CH_A, INPUT_PULLUP);
    pinMode(ENC_G_CH_B, INPUT_PULLUP);
    nb_tic_encodeur_D =0;
    nb_tic_encodeur_G =0;
    attachInterrupt(digitalPinToInterrupt(ENC_D_CH_A), ENC_ISIR_D, CHANGE);
    attachInterrupt(digitalPinToInterrupt(ENC_G_CH_A), ENC_ISIR_G, CHANGE);
}

void DEBUG_encodeur(void){
    Serial.print(">Ticks_G:");
    Serial.println(nb_tic_encodeur_G);
    Serial.print(">Ticks_D:");
    Serial.println(nb_tic_encodeur_D);
}

float angle_total_Z = 0;              // Angle cumulé en degrés
unsigned long temps_precedent = 0;    // Temps de la dernière mesure

LSM6DS3 myIMU;
void Enable_IMU (void){
    if (myIMU.begin() != 0) {
        Serial.println("Erreur : LSM6DS3 non détecté !");
    } else {
        Serial.println("Initialize LSM6DS3!");
    }
}

void DEBUG_IMU (void){
    /*gyro = vitesse angulaire
    Accelerometre = poids graviationnel sur les axes X, Y et Z*/

    // Accéléromètre
    Serial.print(">Accel_X:");
    Serial.println(myIMU.readFloatAccelX()); 

    Serial.print(">Accel_Y:");
    Serial.println(myIMU.readFloatAccelY());

    Serial.print(">Accel_Z:");
    Serial.println(myIMU.readFloatAccelZ());

    // Gyroscope
    Serial.print(">Gyro_X:");
    Serial.println(myIMU.readFloatGyroX());

    Serial.print(">Gyro_Y:");
    Serial.println(myIMU.readFloatGyroY());

    Serial.print(">Gyro_Z:");
    Serial.println(myIMU.readFloatGyroZ());

    // Température
    Serial.print(">Temp_C:");
    Serial.println(myIMU.readTempC());
}


float Find_angle (void){
    float vitesse_Z = myIMU.readFloatGyroZ();
    if (abs(vitesse_Z) < 3.5) vitesse_Z = 0; // Filtrage du bruit (dead zone = 3.5)

    unsigned long temps_actuel = millis();
    float duree = (temps_actuel - temps_precedent) / 1000.0; //sec
    temps_precedent = temps_actuel;

    angle_total_Z += vitesse_Z * duree;
    return angle_total_Z;
}

void Reset_angle_Z(void) {
    angle_total_Z = 0;
}

void DEBUG_angle(void){
    Serial.print(">AngleZ:");
    Serial.println(Find_angle());
}

LIS3MDL mag;
void Enable_MAG (void){  //calibration : min: { -4750,  -2345,  -1612}   max: {  -773,  +2753,  +3158}
    if (!mag.init())
    {
      Serial.println("Erreur : magnetometer non détecté !");
      //while (1);
    }else{
        Serial.println("Initialize magnetometer!");
    }
  
    mag.enableDefault();
}

void DEBUG_MAG(void){
    mag.read();
    Serial.print(">MagX:");
    Serial.println(mag.m.x);    // Affiche mag X
    Serial.print(">MagY:");
    Serial.println(mag.m.y);    // Affiche mag Y
    Serial.print(">MagZ:");
    Serial.println(mag.m.z);    // Affiche mag Z
}

float Find_north(void){ //FIXME north programme
    mag.read();
    float heading = atan2(mag.m.y, -mag.m.x) * 180 / PI;
    if (heading < 0) heading += 360;
    return heading;
}

void DEBUG_North (void){ 
    Serial.print(">North:");
    Serial.println(Find_north());
}

