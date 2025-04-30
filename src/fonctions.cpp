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