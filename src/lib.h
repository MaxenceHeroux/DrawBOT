#include <Arduino.h>
#define DEBUG 0

// User led
#define LEDU1 25
#define LEDU2 26

// Enable moteurs droit et gauche
#define EN_D 23
#define EN_G 4

// Commande PWM moteur droit
#define IN_1_D 19
#define IN_2_D 18             // direction

// Commande PWM moteur gauche
#define IN_1_G 17
#define IN_2_G 16             // direction
const int pwmChannel_MD = 0;  // canal PWM pour ESP32
const int pwmChannel_MG = 1;  // canal PWM pour ESP32
const int freq = 1000;        // fréquence PWM en Hz
const int resolution = 8;     // résolution 8 bits (0-255)

// Encodeur droit
#define ENC_D_CH_A 27
#define ENC_D_CH_B 14
extern long nb_tic_encodeur_D;
// Encodeur gauche
#define ENC_G_CH_A 32
#define ENC_G_CH_B 33
extern long nb_tic_encodeur_G;

// I2C
#define SDA 21
#define SCL 22
// Adresse I2C
#define ADDR_IMU 0x6B
#define ADDR_MAG 0x1E



#include <foundation.h>
void DEBUG_Blink(void);
void Enable_moteur(void);
void Enable_PWM(void);
void PWM(int pwmchannel, int DutyCycle, boolean Direction);
void DEBUG_pwm(void);
void IRAM_ATTR ENC_ISIR_D();
void IRAM_ATTR ENC_ISIR_G();
void Enable_encodeur(void);
void DEBUG_encodeur(void);

#include<WiFi.h>
#include <WebServer.h>
extern WebServer server;
extern String htmlPage;

void Enable_wifi(void);

extern int YY;