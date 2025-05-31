#include <Arduino.h>
#include <Wire.h>
#include <SparkFunLSM6DS3.h>
#include <Adafruit_LIS3MDL.h>
#include <Adafruit_Sensor.h>
#include <WiFi.h>
#include <WebServer.h>
#include <lib.h>

// Définition des pins I2C
#define I2C_SDA 21
#define I2C_SCL 22

// Moteurs
#define EN_D 23
#define EN_G 4
#define IN_1_D 19
#define IN_2_D 18
#define IN_1_G 17
#define IN_2_G 16

// Encodeurs
#define ENC_G_CH_A 32
#define ENC_D_CH_A 27

// Constantes du robot
#define diametre 90.0
#define circomference (diametre * PI)
#define distance_roue 83.0
#define ticks_par_tour 1080.0
#define MM_par_tick (circomference / ticks_par_tour)
#define distance_stylo 140.0

#define DECLINAISON_MAGNETIQUE 1.7833

volatile long ticks_g = 0;
volatile long ticks_d = 0;
long last_ticks_g = 0;
long last_ticks_d = 0;
int vitesse = 140;
float targetDistance = 0;
float targetAngle = 0;
bool isMoving = false;
bool isRotating = false;

enum EtatRobot {
  REPOS,
  MOVING_FORWARD,
  ROTATING
};






EtatRobot currentState = REPOS;

LSM6DS3 imu(I2C_MODE, 0x6B);
Adafruit_LIS3MDL mag;

float gyroZ = 0;
float angleZ = 0;
unsigned long lastGyroTime = 0;

// PID pour la distance
float kp_dist = 1.00; // à ajuster
float kd_dist = 1.0; // à ajuster
float ki_dist = 0.16; // à ajuster
float lastErrorDist = 0;
float integralDist = 0;


// PID pour la correction directionnelle
float kp_dir = 1.0; // Augmenté pour une correction plus rapide
float ki_dir = 0.04; // Ajusté pour réduire les erreurs accumulées
float kd_dir = 0.08; // Ajusté pour réduire les oscillations
float lastErrorDir = 0;
float integralDir = 0;

float mag_offset2_x = 0;
float mag_offset2_y = 0;
float mag_scale_x = 1;
float mag_scale_y = 1;
bool calibrated = false;


void IRAM_ATTR handleLeftEncoder() {
  ticks_g++;
}


void IRAM_ATTR handleRightEncoder() {
  ticks_d++;
}


void generateStopPage(const String& sequenceName) {
  String page = "<!DOCTYPE html>";
  page += "<html lang='fr'>";
  page += "<head>";
  page += "    <meta charset='UTF-8' http-equiv='refresh' content='3;url=/'>";
  page += "    <meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  page += "    <title>Drawbot</title>";
  page += "    <style>";
  page += "        body {";
  page += "            background-color: #f0f8ff;";
  page += "            display: flex;";
  page += "            justify-content: center;";
  page += "            align-items: center;";
  page += "            height: 100vh;";
  page += "            margin: 0;";
  page += "        }";
  page += "        .container {";
  page += "            border: 2px solid #000;";
  page += "            padding: 20px;";
  page += "            background-color: #ffffff;";
  page += "            text-align: center;";
  page += "            box-shadow: 5px 5px 15px rgba(0, 0, 0, 0.2);";
  page += "        }";
  page += "        button {";
  page += "            margin: 10px;";
  page += "            padding: 10px 20px;";
  page += "            font-size: 16px;";
  page += "            cursor: pointer;";
  page += "            border: none;";
  page += "            border-radius: 5px;";
  page += "            background-color: #007bff;";
  page += "            color: white;";
  page += "        }";
  page += "        button:hover {";
  page += "            background-color: #0056b3;";
  page += "        }";
  page += "    </style>";
  page += "</head>";
  page += "<body>";
  page += "    <div class='container'>";
  page += "        <h1>" + sequenceName + "</h1>";
  page += "        <button onclick=\"location.href='/stop'\">Arrêter la séquence</button>";
  page += "    </div>";
  page += "</body>";
  page += "</html>";

  server.send(200, "text/html", page);
}


void resetEncoders() {
  ticks_g = 0;
  ticks_d = 0;
  last_ticks_g = 0;
  last_ticks_d = 0;
}


void arreterMoteurs() {
  analogWrite(IN_1_D, 0);
  analogWrite(IN_2_D, 0);
  analogWrite(IN_1_G, 0);
  analogWrite(IN_2_G, 0);
  isMoving = false;
  isRotating = false;
  currentState = REPOS;
}


void avancer(int speedLeft, int speedRight) {
  analogWrite(IN_1_D, 0);
  analogWrite(IN_2_D, speedRight);
  analogWrite(IN_1_G, speedLeft);
  analogWrite(IN_2_G, 0);
}


void reculer(int speedLeft, int speedRight) {
  analogWrite(IN_1_D, speedRight);
  analogWrite(IN_2_D, 0);
  analogWrite(IN_1_G, 0);
  analogWrite(IN_2_G, speedLeft);
}


void tournerGauche(int speed) {
  analogWrite(IN_1_D, 0);
  analogWrite(IN_2_D, speed);
  analogWrite(IN_1_G, 0);
  analogWrite(IN_2_G, speed);
}


void tournerDroite(int speed) {
  analogWrite(IN_1_D, speed);
  analogWrite(IN_2_D, 0);
  analogWrite(IN_1_G, speed);
  analogWrite(IN_2_G, 0);
}


void stopRobot() {
  arreterMoteurs();
  resetEncoders();

  String page = "<!DOCTYPE html>";
  page += "<html lang='fr'>";
  page += "<head>";
  page += "    <meta charset='UTF-8'>";
  page += "    <meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  page += "    <title>Drawbot</title>";
  page += "    <style>";
  page += "        body {";
  page += "            background-color: #f0f8ff;";
  page += "            display: flex;";
  page += "            justify-content: center;";
  page += "            align-items: center;";
  page += "            height: 100vh;";
  page += "            margin: 0;";
  page += "        }";
  page += "        .container {";
  page += "            border: 2px solid #000;";
  page += "            padding: 20px;";
  page += "            background-color: #ffffff;";
  page += "            text-align: center;";
  page += "            box-shadow: 5px 5px 15px rgba(0, 0, 0, 0.2);";
  page += "        }";
  page += "        button {";
  page += "            margin: 10px;";
  page += "            padding: 10px 20px;";
  page += "            font-size: 16px;";
  page += "            cursor: pointer;";
  page += "            border: none;";
  page += "            border-radius: 5px;";
  page += "            background-color: #007bff;";
  page += "            color: white;";
  page += "        }";
  page += "        button:hover {";
  page += "            background-color: #0056b3;";
  page += "        }";
  page += "    </style>";
  page += "</head>";
  page += "<body>";
  page += "    <div class='container'>";
  page += "        <h1>Robot arrêté</h1>";
  page += "        <button onclick=\"location.href='/'\">Retour à l'accueil</button>";
  page += "    </div>";
  page += "</body>";
  page += "</html>";

  server.send(200, "text/html", page);
}


float getLeftDistance() {
  return ticks_g * MM_par_tick;
}


float getRightDistance() {
  return ticks_d * MM_par_tick;
}


float getAverageDistance() {
  return (getLeftDistance() + getRightDistance()) / 2.0;
}


void updateGyroAngle() {
  unsigned long currentTime = millis();
  float dt = (currentTime - lastGyroTime) / 1000.0;
  gyroZ = imu.readFloatGyroZ();
  angleZ += gyroZ * dt;
  lastGyroTime = currentTime;
}


void moveDistance(float distanceMm) {
  resetEncoders();

  float distance_compensee = distanceMm - 40.0 -(0.11 * distanceMm);
  targetDistance = distance_compensee;
  currentState = MOVING_FORWARD;
  isMoving = true;
  lastErrorDist = targetDistance; // initialiser l'erreur
  integralDist = 0;
  if (distance_compensee > 0) {
    avancer(vitesse, vitesse);
  } 
  else {
    reculer(vitesse, vitesse);
  }
}


void rotate(float angleDegrees) {
  resetEncoders();
  angleZ = 0;
  lastGyroTime = millis();
  targetAngle = angleDegrees;
  currentState = ROTATING;
  isRotating = true;
}


void updateEtatRobot() {
  static unsigned long lastTime = 0;
  unsigned long now = millis();
  float dt = (now - lastTime) / 1000.0;
  if (dt <= 0) dt = 0.01;
  lastTime = now;  
  switch (currentState) {
    case MOVING_FORWARD: {
      // PID pour la distance
      float currentDistance = getAverageDistance();
      float errorDist = targetDistance - currentDistance;
      integralDist += errorDist * dt;
      float derivativeDist = (errorDist - lastErrorDist) / dt;
      float pid_dist = kp_dist * errorDist + ki_dist * integralDist + kd_dist * derivativeDist;
      lastErrorDist = errorDist;

      // PID pour la correction directionnelle
      float errorDir = ticks_g - ticks_d; // Différence entre les encodeurs
      integralDir += errorDir * dt;
      float derivativeDir = (errorDir - lastErrorDir) / dt;
      float pid_dir = kp_dir * errorDir + ki_dir * integralDir + kd_dir * derivativeDir;
      lastErrorDir = errorDir;

      // Calcul des vitesses avec les deux PIDs
      int baseSpeed = vitesse + constrain(pid_dist, -50, 50);
      int leftSpeed = baseSpeed - constrain(pid_dir, -30, 30);
      int rightSpeed = baseSpeed + constrain(pid_dir, -30, 30);

      // Limites de vitesse
      leftSpeed = constrain(leftSpeed, 110, 255);
      rightSpeed = constrain(rightSpeed, 110, 255);
      rightSpeed = rightSpeed + 4;

      if (abs(errorDist) < 10.0) {
        arreterMoteurs();
        currentState = REPOS;
        isMoving = false;
      } 
      else if (targetDistance > 0) {
        if (currentDistance < targetDistance) {
          avancer(leftSpeed, rightSpeed);
        } else {
          arreterMoteurs();
          currentState = REPOS;
          isMoving = false;
        }
      } else {
        if (currentDistance > targetDistance) {
          reculer(leftSpeed, rightSpeed);
        } else {
          arreterMoteurs();
          currentState = REPOS;
          isMoving = false;
        }
      }
      break;
    }
    case ROTATING: {
      updateGyroAngle();
      float error = targetAngle - angleZ;
      
      if (abs(error) < 5.0) {
        arreterMoteurs();
        currentState = REPOS;
        isRotating = false;
      } else {
        int rotateSpeed = vitesse;
        if (error > 0) {
          tournerDroite(rotateSpeed);
        } else {
          tournerGauche(rotateSpeed);
        }
      }
      break;
    }
    case REPOS:
      break;
  }
}


void ATTENDREREPOS() {
  while (currentState != REPOS) {
    updateEtatRobot();
    delay(10);  // Augmenté de 10ms à 20ms pour plus de stabilité
  }
}


void calibrerMagnetometre() {
  delay(3000);


  float min_x = 9999, max_x = -9999;
  float min_y = 9999, max_y = -9999;

  unsigned long startTime = millis();
  tournerDroite(vitesse);

  while (millis() - startTime < 7000) {
    sensors_event_t event;
    mag.getEvent(&event);

    float x = event.magnetic.x;
    float y = event.magnetic.y;

    if (x < min_x) min_x = x;
    if (x > max_x) max_x = x;
    if (y < min_y) min_y = y;
    if (y > max_y) max_y = y;

    delay(100);
  }

  arreterMoteurs();

  mag_offset2_x = (max_x + min_x) / 2.0;
  mag_offset2_y = (max_y + min_y) / 2.0;

  float range_x = max_x - min_x;
  float range_y = max_y - min_y;
  float avg_range = (range_x + range_y) / 2.0;

  mag_scale_x = avg_range / range_x;
  mag_scale_y = avg_range / range_y;


  calibrated = true;
}


float lireCapMagnetique() {
  if (!calibrated) return 0;

  sensors_event_t event;
  mag.getEvent(&event);

  float x_cal = (event.magnetic.x - mag_offset2_x) * mag_scale_x;
  float y_cal = (event.magnetic.y - mag_offset2_y) * mag_scale_y;

  float angle = atan2(y_cal, x_cal) * 180.0 / PI;
  if (angle < 0) angle += 360;
  angle += DECLINAISON_MAGNETIQUE;
  if (angle >= 360) angle -= 360;

  return angle;
}


void rechercherNord() {
  float cap_actuel = lireCapMagnetique();
  float erreur = cap_actuel;
  if (erreur > 180) erreur -= 360;



  if (abs(erreur) < 3.0) {
    arreterMoteurs();
    resetEncoders();
    delay(1000);
    return;
  }

  int vitesse_ajustee = vitesse;
  if (abs(erreur) < 15) vitesse_ajustee *= 0.6;
  tournerDroite(vitesse_ajustee);
}


void avancerVersNord() {
  static unsigned long derniere_verification = 0;
  float distance_parcourue = getAverageDistance();

  if (millis() - derniere_verification > 500) {
    float cap_actuel = lireCapMagnetique();
    float erreur = cap_actuel;
    if (erreur > 180) erreur -= 360;



    if (abs(erreur) > 10) {
      arreterMoteurs();
      return;
    }
    derniere_verification = millis();
  }

  moveDistance(300.0);
}


void drawSquare() {
  
  for (int i = 0; i < 4; i++) {    
    // Avancer
    moveDistance(800.0);
    ATTENDREREPOS();
    delay(500);
    
    // Tourner
    rotate(90.0);
    ATTENDREREPOS();
    delay(500);  
  }
}


void drawStairs() {
  for (int i = 0; i < 5; i++) {
    moveDistance(100.0);
    ATTENDREREPOS();
    rotate(-90.0);
    ATTENDREREPOS();
    moveDistance(100.0);
    ATTENDREREPOS();
    rotate(90.0);
    ATTENDREREPOS();
  }

}


void drawTest(){
  moveDistance(300.0);
  ATTENDREREPOS();
  delay(10);
}


void drawCircle(float distance){
  moveDistance(distance);
  ATTENDREREPOS();
}


void demanderDistance() {
  String page = "<!DOCTYPE html>";
  page += "<html lang='fr'>";
  page += "<head>";
  page += "    <meta charset='UTF-8'>";
  page += "    <meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  page += "    <title>Drawbot - Séquence 2</title>";
  page += "    <style>";
  page += "        body {";
  page += "            background-color: #f0f8ff;";
  page += "            display: flex;";
  page += "            justify-content: center;";
  page += "            align-items: center;";
  page += "            height: 100vh;";
  page += "            margin: 0;";
  page += "        }";
  page += "        .container {";
  page += "            border: 2px solid #000;";
  page += "            padding: 20px;";
  page += "            background-color: #ffffff;";
  page += "            text-align: center;";
  page += "            box-shadow: 5px 5px 15px rgba(0, 0, 0, 0.2);";
  page += "        }";
  page += "        input, button {";
  page += "            margin: 10px;";
  page += "            padding: 10px 20px;";
  page += "            font-size: 16px;";
  page += "        }";
  page += "        button {";
  page += "            cursor: pointer;";
  page += "            border: none;";
  page += "            border-radius: 5px;";
  page += "            background-color: #007bff;";
  page += "            color: white;";
  page += "        }";
  page += "        button:hover {";
  page += "            background-color: #0056b3;";
  page += "        }";
  page += "    </style>";
  page += "</head>";
  page += "<body>";
  page += "    <div class='container'>";
  page += "        <h1>Séquence 2 - Escalier</h1>";
  page += "        <form action='/start-stairs' method='GET'>";
  page += "            <label for='distance'>Distance entre marches (2-20 cm):</label>";
  page += "            <input type='number' id='distance' name='distance' min='2' max='20'>";
  page += "            <button type='submit'>Démarrer</button>";
  page += "        </form>";
  page += "        <button onclick=\"location.href='/'\">Retour</button>";
  page += "    </div>";
  page += "</body>";
  page += "</html>";

  server.send(200, "text/html", page);
}


void startRobot1() {
  generateStopPage("Séquence 1 en cours...");

  // Avancer de 20 cm (ajusté pour distance_stylo)
  moveDistance(200.0);
  ATTENDREREPOS();
  delay(500);

  resetEncoders();
  avancer(32, 155);
  while (getAverageDistance() < 39.0) {
    // Attendre que la distance parcourue atteigne 10 cm
    updateEtatRobot();
    delay(10);
  }

  resetEncoders();
  avancer(155, 35);
  while (getAverageDistance() < 44.0) {
    // Attendre que la distance parcourue atteigne 10 cm
    updateEtatRobot();
    delay(10);
  }
  delay(100);
  avancer(165, 65);
  while (getAverageDistance() < 57.0) {
    // Attendre que la distance parcourue atteigne 10 cm
    updateEtatRobot();
    delay(10);
  }


  moveDistance(100.0);
  delay(200);
  ATTENDREREPOS();


  // Arrêter le robot
  arreterMoteurs();
  resetEncoders();
  stopRobot();
}


void startRobot2() {
  if (server.hasArg("distance")){
    float distanceCm = server.arg("distance").toFloat();
    if (distanceCm >= 2 && distanceCm <= 20){
      generateStopPage("Séquence 2 en cours...");
      drawCircle(distanceCm * 10.0);
      arreterMoteurs();
      resetEncoders();
      stopRobot();
    }
    else{
      server.send(400,"tekxt/plain","Veuillez choisir une distance entre 2 et 20 cm.");
    }
  }
  else{
    arreterMoteurs();
    resetEncoders();
    stopRobot();
  }
}


void startRobot3() {
  generateStopPage("Séquence 3 en cours...");
  calibrerMagnetometre();
  while (true) {
    rechercherNord();
    delay(50);
    float erreur = lireCapMagnetique(); 
    if (erreur > 180) erreur -= 360;
    if (abs(erreur) < 3.0) break;
  }
  avancerVersNord();
  ATTENDREREPOS();

  // Avancer de 30 cm
  moveDistance(300.0);
  ATTENDREREPOS();
  delay(500);
  
  // Réinitialiser les encodeurs pour la phase de mouvement asymétrique
  resetEncoders();
  
  // Avancer avec vitesse asymétrique (roue gauche plus lente que roue droite)
  unsigned long startTime = millis();
  avancer(80, 185);
  while (millis() - startTime < 50) { // 500ms = 1/2 seconde
    delay(10);
  }
  
  // Arrêter les moteurs avant de reculer
  arreterMoteurs();
  delay(500);

  unsigned long startTime2 = millis();
  reculer(75, 185);
  while (millis() - startTime2 < 160) { // 500ms = 1/2 seconde
    delay(10);
  }

  resetEncoders();
  unsigned long startTime4 = millis();
  reculer(45, 150);
  while (millis() - startTime4 < 200) { // 500ms = 1/2 seconde
    delay(10);
  }

  resetEncoders();
  unsigned long startTime3 = millis();
  reculer(175, 45);
  while (millis() - startTime3 < 165) { // 500ms = 1/2 seconde
    delay(10);
  }

  delay(100);
  resetEncoders();

  unsigned long startTimebis = millis();
  avancer(140, 155);
  while (millis() - startTimebis < 270) { // 500ms = 1/2 seconde
    delay(10);
  }

  resetEncoders();
  
  moveDistance(85);
  ATTENDREREPOS();

  


  // Arrêter complètement le robot
  arreterMoteurs();
  resetEncoders();
  stopRobot();
}





//FIN