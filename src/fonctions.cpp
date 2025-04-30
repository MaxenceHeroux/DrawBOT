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

void Enable_PWM(void){
    //PWM moteur 1
    pinMode(IN_1_D, OUTPUT);
    pinMode(IN_2_D, OUTPUT);
    ledcSetup(pwmChannel_MD, freq, resolution);
    ledcAttachPin(IN_1_D, pwmChannel_MD);
    digitalWrite(IN_2_D, LOW);  //Direction par defaut (Avant)

    //PWM moteur 2
    pinMode(IN_1_G, OUTPUT);
    pinMode(IN_2_G, OUTPUT);
    ledcSetup(pwmChannel_MG, freq, resolution);
    ledcAttachPin(IN_1_G, pwmChannel_MG);
    digitalWrite(IN_2_G, HIGH);  //Direction par defaut (Avant)
}

void PWM(int pwmchannel, int DutyCycle, boolean Direction){
    if (pwmchannel == pwmChannel_MD){
        //Direction = 1 (Avancer)
        if (Direction){
            digitalWrite(IN_2_D, LOW);  //(Avancer)
            ledcWrite(pwmChannel_MD, DutyCycle);
        }else{
            digitalWrite(IN_2_D, HIGH);  //(Reculer)
            ledcWrite(pwmChannel_MD, DutyCycle);
        }
    }else{
        if (Direction){
            digitalWrite(IN_2_G, HIGH);  //(Avancer)
            ledcWrite(pwmChannel_MG, DutyCycle);
        }else{
            digitalWrite(IN_2_G, LOW);  //(Reculer)
            ledcWrite(pwmChannel_MG, DutyCycle);
        }
    }
}

void DEBUG_pwm(void){
    PWM(pwmChannel_MD,100,true);
    PWM(pwmChannel_MG,100,true);
    delay(500);
    PWM(pwmChannel_MD,100,false);
    PWM(pwmChannel_MG,100,false);
    delay(500);
    PWM(pwmChannel_MD,255,true);
    PWM(pwmChannel_MG,255,false);
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