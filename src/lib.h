#include <Arduino.h>

//User Interface (UI)
#define DEBUG 1
#define CURVILIGNE 1

// User led
#define LEDU1 25
#define LEDU2 26

//moteurs & roues
#define LOWEST_PWM 55
#define HIGHTEST_PWM 255
#define DIAMETRE_ROUE 90
#define ECART_2_ROUES 80
#define TICKS_PAR_TOUR_DE_ROUE 2100
#define DIST_STYLO 132

extern float pos_X , pos_Y;

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
extern int commande_pwm_dist_MD, commande_pwm_dist_MG;
extern int commande_pwm_angle_MD, commande_pwm_angle_MG;
extern float anglerobot;

//Encodeurs
// Encodeur droit
#define ENC_D_CH_A 27
#define ENC_D_CH_B 14
extern long nb_tic_encodeur_D;
// Encodeur gauche
#define ENC_G_CH_A 32
#define ENC_G_CH_B 33
extern long nb_tic_encodeur_G;

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
extern int Joy_X, Joy_Y; 
extern bool joystickConnecte;
extern unsigned long lastJoystickTime;
extern float radius;
extern float xOrigin;
extern float yOrigin;
extern int windrose;
extern int escalier_options;
extern float Kp;
extern float Ki;
extern float Kd;

//IMU
#include "SparkFunLSM6DS3.h" 
extern LSM6DS3 myIMU;

//Magneto
#include <LIS3MDL.h>
extern LIS3MDL mag;

//fonctions.cpp
int signe(int val);
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
void Enable_IMU (void);
void DEBUG_IMU (void);
float Find_angle (void);
void Reset_angle_Z(void);
void DEBUG_angle(void);
void Enable_MAG (void);
void DEBUG_MAG(void);
float Find_north (void);
void DEBUG_North (void);

//Asservissement.cpp
int Ticks_to_Distance (int distance);
int Distance_to_Tick (int ticks);
int Avancer (int dist_consigne, float KP, float KI, float KD);
void Reset_pid_distance (void);
void DEBUG_PID_distance (int consigne_dist);
float Angle_restriction(float angle);
float Get_angle();
int Tourner (int angle, float KP, float KI, float KD);
void Reset_pid_angle(void);
void DEBUG_PID_angle (int consigne_angle);

void Actualiser_co (int delta_D, int delta_G);