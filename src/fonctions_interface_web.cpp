#include <lib.h>

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

int mode = 0; //Ne fait rien quand le mode est à 0.
float radius;
float nbPoint;
void handleCircle(void) {
    String type = server.arg("type");

    if(type == "Circle"){
        mode = 3; //Fais un cercle
    }else if(type == "Rosace"){
        mode = 4; //Fais une rosace
    }

    radius = server.arg("radius").toFloat();
    nbPoint = server.arg("points").toFloat();

    if (DEBUG) {
        Serial.printf("Cercle reçu : rayon=%.2f, nombre de points=%.2f et mode :%d\n", radius, nbPoint, mode);
    }

    server.send(200, "text/plain", "Cercle OK");
}

void handleRose(void) {
    String type = server.arg("type");
    if (type == "windrose") {
        mode = 5; //Fais une rose des vents
    } else if (type == "arrow") {
        mode = 6; //Fais une flèche
    }

    if (DEBUG) {
        Serial.printf("Type de rose des vents reçu : %s\n", type.c_str());
    }

    server.send(200, "text/plain", "Rose OK");
}

int nbCarre;
int coteCarre;
void handleSequence(void) {
  String type = server.arg("type");
  
  if (type == "step") {
    mode = 1; //Fais un escalier
  } else if (type == "multiCarre") {
    mode = 2; //Fais un carré dans un carré dans un carré etc.
  }

    nbCarre = server.arg("nb_carre").toInt();
    coteCarre = server.arg("coter").toInt();


  if (DEBUG) {
    Serial.printf("Nombre de carré : %d, cote du plus grand carre = %d\n.", nbCarre, coteCarre);
    Serial.print("Séquence sélectionnée : ");
    Serial.println(mode);
  }

  server.send(200, "text/plain","OK");
}

float Kp_ang;
float Ki_ang;
float Kd_ang;
float Kp_dist;
float Ki_dist;
float Kd_dist;
void handlePID() {
    Kp_ang = server.arg("kp_ang").toFloat();
    Ki_ang = server.arg("ki_ang").toFloat();
    Kd_ang = server.arg("kd_ang").toFloat();
    Kp_dist = server.arg("kp_dist").toFloat();
    Ki_dist = server.arg("ki_dist").toFloat();
    Kd_dist = server.arg("kd_dist").toFloat();

    if (DEBUG) {
        Serial.printf("Réception PID : Kp_ang=%.2f, Ki_ang=%.2f, Kd_ang=%.2f, Kp_dist=%.2f, Ki_dist=%.2f, Kd_dist=%.2f\n", Kp_ang, Ki_ang, Kd_ang, Kp_dist, Ki_dist, Kd_dist);
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