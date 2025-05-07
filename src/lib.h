#include <Arduino.h>

#define DEBUG 1

// User led
#define LEDU1 25
#define LEDU2 26

//moteurs
#define LOWEST_PWM 20
// Enable moteurs droit et gauche
#define EN_D 23
#define EN_G 4
// Commande PWM moteur droit
#define IN_1_D 19
#define IN_2_D 18            
// Commande PWM moteur gauche
#define IN_1_G 17
#define IN_2_G 16            
const int pwmChannel_MD_1 = 0;  // canal PWM pour ESP32
const int pwmChannel_MD_2 = 1;  // canal PWM pour ESP32
const int pwmChannel_MG_1 = 2;  // canal PWM pour ESP32
const int pwmChannel_MG_2 = 3;  // canal PWM pour ESP32
const int freq = 1000;        // fréquence PWM en Hz
const int resolution = 8;     // résolution 8 bits (0-255)

//PID
extern int consigne_dist_MD, consigne_dist_MG;
extern int commande_dist_MD, commande_dist_MG;

extern int consigne_rot_MD, consigne_rot_MG;
extern int commande_rot_MD, commande_rot_MG;

//Encodeurs
#define COEFF_MOTEUR 0.66 // ~255/350 // commande pwm 255 = 255 tour de roue par mini boucle, objectif : exprimer les tiques en proportion du pwm 255 -> 350 ticks X->, permet le calcul de l'erreur et le pid sans faire (pwm = cb de ticks et apres erreur puis re pwm pour le pid)
// Encodeur droit
#define ENC_D_CH_A 27
#define ENC_D_CH_B 14
extern long nb_tic_encodeur_D;
//extern long nb_tic_encodeur_D_prec;
// Encodeur gauche
#define ENC_G_CH_A 32
#define ENC_G_CH_B 33
extern long nb_tic_encodeur_G;
//extern long nb_tic_encodeur_G_prec;

// I2C
#include <Wire.h>
#define SDA 21
#define SCL 22
#define ADDR_IMU 0x6B
#define ADDR_MAG 0x1E

//Wifi - server
#include<WiFi.h>
#include <WebServer.h>
#define SPLEEP_TIME 500
extern WebServer server;
extern String htmlPage;
extern int Joy_X, Joy_Y; //sortie du joystick
extern bool joystickConnecte;
extern unsigned long lastJoystickTime;

//IMU
#include "SparkFunLSM6DS3.h" //todo registre = moins d espace gaspiller avec le spi ...
extern LSM6DS3 myIMU;

//Magneto
#include <LIS3MDL.h>
#define CALIBRATION_MAG_X (-773 - 4750)/2
#define CALIBRATION_MAG_Y (2753 - 2345)/2
#define CALIBRATION_MAG_Z (3158 - 1612)/2
extern LIS3MDL mag;

//fonctions.cpp
void DEBUG_Blink(void);
void Enable_moteur(void);
void Disable_moteur(void);
void Enable_PWM(void);
void PWM(char moteur, int DutyCycle);
void DEBUG_pwm(void);
void IRAM_ATTR ENC_ISIR_D();
void IRAM_ATTR ENC_ISIR_G();
void Enable_encodeur(void);
void DEBUG_encodeur(void);
void handleRoot(void);
void handleJoystick(void);
void Enable_wifi(void);
void Remote (void);
void Enable_Teleplot(void);
void Enable_IMU (void);
void DEBUG_IMU (void);
float Find_angle (void);
void DEBUG_angle(void);
void Enable_MAG (void);
void DEBUG_MAG(void);
float Find_north (void);
void DEBUG_North (void);

//Asservissement.cpp
int Mini_boucle (void);
void ENC_vitesse (void);
void PID_vitesse (float KP, float KI, float KD);
void PID_distance (float KP, float KI, float KD);
void DEBUG_PID_distance (void);
float Ticks_to_Distance (int ticks);
void PID_rotation(float KP, float KI, float KD);