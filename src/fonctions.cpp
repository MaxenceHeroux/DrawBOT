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
    Serial.print("Ticks avec sens :   G:");
    Serial.print(nb_tic_encodeur_G);
    Serial.print("   D:");
    Serial.println(nb_tic_encodeur_D);
    delay(500);
}

const char* ssid = "DrawBOT";
const char* password = "DrawBOT1234";
WebServer server(80); //port 80 
bool joystickConnecte = false;


void handleRoot(void) {
    server.send(200, "text/html", htmlPage);
}

unsigned long lastJoystickTime = 0;
void handleJoystick(void) {
    Joy_X = server.arg("x").toInt(); 
    Joy_Y = server.arg("y").toInt();
    joystickConnecte = true; //activations de la commande des joysticks
    lastJoystickTime = millis();
    Serial.printf("Joystick (entiers) : X=%d, Y=%d\n", Joy_X, Joy_Y);
    server.send(200, "text/plain", "OK");
}

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
    server.on("/", handleRoot); //page principale 
    server.on("/joy", handleJoystick); //page joystick

    server.begin();
    Serial.println("Serveur HTTP lancé.");
}