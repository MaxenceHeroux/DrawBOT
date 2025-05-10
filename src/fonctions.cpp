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
    DutyCycle = constrain(DutyCycle, -255, 255);
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
    //nb_tic_encodeur_D_prec =0;
    //nb_tic_encodeur_G_prec =0;
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

int Joy_X, Joy_Y;
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

float radius;
float xOrigin;
float yOrigin;
void handleCircle(void) {
    radius = server.arg("radius").toFloat();
    xOrigin = server.arg("xOrigin").toFloat();
    yOrigin = server.arg("yOrigin").toFloat();

    if (DEBUG) {
        Serial.printf("Cercle reçu : rayon=%.2f, centre=(%.2f, %.2f)\n", radius, xOrigin, yOrigin);
    }

    server.send(200, "text/plain", "Cercle OK");
}

int windrose = 0;
void handleRose(void) {
    String type = server.arg("type");
    if (type == "windrose") {
        windrose = 1; //Fais une rose des vents
    } else if (type == "arrow") {
        windrose = 2; //Fais une flèche
    }

    if (DEBUG) {
        Serial.printf("Type de rose des vents reçu : %s\n", type.c_str());
    }

    server.send(200, "text/plain", "Rose OK");
}

int escalier_options = 0; // Fais rien quand il est à 0
void handleSequence(void) {
  String type = server.arg("type");
  
  if (type == "step") {
    escalier_options = 1; //Fais un escalier
  } else if (type == "line") {
    escalier_options = 2; //Avancer en ligne droite
  } else if (type == "rotate") {
    escalier_options = 3; //Fais une rotation de 90°
  }

  if (DEBUG) {
    Serial.print("Séquence sélectionnée : ");
    Serial.println(escalier_options);
  }

  server.send(200, "text/plain","OK");
}

float Kp;
float Ki;
float Kd;
void handlePID() {
    Kp = server.arg("kp").toFloat();
    Ki = server.arg("ki").toFloat();
    Kd = server.arg("kd").toFloat();

    if (DEBUG) {
        Serial.printf("Réception PID : Kp=%.2f, Ki=%.2f, Kd=%.2f\n", Kp, Ki, Kd);
    }

    server.send(200, "text/plain", "PID OK");
}

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
    server.on("/pid", handlePID);           //page debug
    server.on("/circle", handleCircle);     //page cercle
    server.on("/rose", handleRose);         //page rose des vents
    server.on("/sequence",handleSequence);  //page escalier
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
    Vg = constrain(-Joy_Y + Joy_X, -255, 255);
    Vd = constrain(-Joy_Y - Joy_X, -255, 255);

    // Moteur gauche
    PWM('G', Vg);   
    // Moteur droit
    PWM('D', Vd);
  }
}

float angle_total_Z = 0;              // Angle cumulé en degrés
unsigned long temps_precedent = 0;    // Temps de la dernière mesure

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
    if (abs(vitesse_Z) < 4) vitesse_Z = 0; // Filtrage du bruit (dead zone = 3)

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

float max_x = -2043;
float min_x = -5188;
float max_y = 2034;
float min_y = -1327;

float offset_x = (max_x + min_x)/2;
float offset_y = (max_y + min_y)/2;

float scale_x = (max_x - min_x)/2;
float scale_y = (max_y - min_y)/2;
float avg_scale = (scale_x + scale_y)/2;

float cal_x;
float cal_y;

float Find_north (void){ //TODO calibration 
    mag.read();
    float angle;
    cal_x = (mag.m.x - offset_x) * (avg_scale/scale_x);
    cal_y = (mag.m.y - offset_y) * (avg_scale/scale_y);
    angle = atan2(cal_y, cal_x) * 180 / PI;
    return angle;
}

void DEBUG_North (void){ //FIXME north programme
    Serial.print(">North:");
    Serial.println(Find_north());
}

