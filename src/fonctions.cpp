#include <lib.h>

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

void PWM(char moteur, int DutyCycle, boolean Direction){ //Direction = 1 (Avancer)
    DutyCycle = constrain(DutyCycle, 0, 255);
    if (moteur == 'D'){
        if (Direction){
            ledcWrite(pwmChannel_MD_1, 0);          //(Avancer)
            ledcWrite(pwmChannel_MD_2, DutyCycle);
        }else{
            ledcWrite(pwmChannel_MD_1, DutyCycle);  //(Reculer)
            ledcWrite(pwmChannel_MD_2, 0);
        }
    }else{
        if (Direction){
            ledcWrite(pwmChannel_MG_1, DutyCycle);  //(Avancer)
            ledcWrite(pwmChannel_MG_2, 0);
        }else{
            ledcWrite(pwmChannel_MG_1, 0);          //(Reculer)
            ledcWrite(pwmChannel_MG_2, DutyCycle);
        }
    }
}

void DEBUG_pwm(void){
    PWM('D',255,true);
    PWM('G',255,true);
    delay(500);
    PWM('D',255,false);
    PWM('G',255,false);
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

const char* ssid = "DrawBOT";
const char* password = "DrawBOT1234";
WebServer server(80); //port 80 
bool joystickConnecte = false;


void handleRoot(void) {
    server.send(200, "text/html; charset=UTF-8", htmlPage);
}

unsigned long lastJoystickTime = 0;
void handleJoystick(void) {
    Joy_X = server.arg("x").toInt(); 
    Joy_Y = server.arg("y").toInt();
    joystickConnecte = true; //activations de la commande des joysticks
    lastJoystickTime = millis();
    //DEBUG
    if(DEBUG){
        Serial.printf("Joystick (entiers) : X=%d, Y=%d\n", Joy_X, Joy_Y);
    }
    server.send(200, "text/plain", "OK");
}

//TODO void handleButton(void){}

int Joy_X, Joy_Y;
void Enable_wifi(void){
    // Définir l'IP statique pour le point d'accès
    IPAddress local_IP(192, 168, 14, 14);     // nouvelle IP
    IPAddress gateway(192, 168, 14, 14);      // souvent identique en mode AP
    IPAddress subnet(255, 255, 255, 0);       // masque
    WiFi.softAPConfig(local_IP, gateway, subnet);

    // Démarrer l'ESP32 en point d'accès
    WiFi.softAP(ssid, password);
    Serial.println("Point d'accès Wi-Fi actif !");
    Serial.print("Adresse IP : ");
    Serial.println(WiFi.softAPIP());  //192.168.14.14

    // Configurer le serveur web
    server.on("/", handleRoot);             //page principale 
    server.on("/joy", handleJoystick);      //page joystick
    server.on("/favicon.ico", []() {        // Évite les erreurs
        server.send(204); 
    });
    server.onNotFound([]() {                // Catch-all pour toutes autres routes
        server.send(404, "text/plain", "Not found");
    });

    server.begin();
    Serial.println("Serveur HTTP lancé.");
}

void Remote (void){
    //Désactiver les moteurs si plus de commande depuis 2 sec
  if (joystickConnecte && millis() - lastJoystickTime > SPLEEP_TIME) {
    joystickConnecte = false;
    Disable_moteur();
  }
  
  if (joystickConnecte) {
    int Vg = 0, Vd = 0;

    if (abs(Joy_Y) < LOWEST_PWM && abs(Joy_X) < LOWEST_PWM) {
      Disable_moteur(); 
      return;
    }

    Enable_moteur();
    // Calcul des vitesses
    Vg = constrain(Joy_Y - Joy_X, -255, 255);
    Vd = constrain(Joy_Y + Joy_X, -255, 255);

    // Moteur gauche
    if (Vg > 0) PWM('G', Vg, false);   // avancer
    else        PWM('G', -Vg, true);   // reculer

    // Moteur droit
    if (Vd > 0) PWM('D', Vd, false);   // avancer
    else        PWM('D', -Vd, true);   // reculer
  }
}

float  i =0;
float angle_total_Z = 0;              // Angle cumulé en degrés
unsigned long temps_precedent = 0;    // Temps de la dernière mesure

void Enable_Teleplot(void){
    // Print log (teleplot)
    i += 0.1;
    Serial.print("loop:");
    Serial.println(i);
}

LSM6DS3 myIMU;
void Enable_IMU (void){
    delay(1000); //relax...
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
    if (abs(vitesse_Z) < 3.5) vitesse_Z = 0; // Filtrage du bruit (dead zone = 3)

    unsigned long temps_actuel = millis();
    float duree = (temps_actuel - temps_precedent) / 1000.0; //sec
    temps_precedent = temps_actuel;

    angle_total_Z += vitesse_Z * duree; //integrale de la vitesse angulaire cumulée

    // // Normaliser l'angle entre 0° et 360°
    // angle_total_Z = fmod(angle_total_Z, 360.0);
    // if (angle_total_Z < 0) angle_total_Z += 360.0;

    return angle_total_Z;
    
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
      while (1);
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

float Find_north (void){ //TODO calibration 
    mag.read();
    float heading = atan2(mag.m.y, -mag.m.x) * 180 / PI;
    if (heading < 0) heading += 360;
    return heading;
}

void DEBUG_North (void){ //FIXME north programme
    Serial.print(">North:");
    Serial.println(Find_north());
}