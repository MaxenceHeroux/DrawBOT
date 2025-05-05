#include <lib.h>

/*--------------------------------------------------------------------------------
  Variable utile : nb_tic_encodeur_D nb_tic_encodeur_G 
  Fonction utile : PWM () //TODO Get orientation_by_IMU () Findnorth() EKF()
--------------------------------------------------------------------------------*/ 

int previousMillis =0;

int Mini_boucle (void){
    if( millis() - previousMillis >100) {
        previousMillis = millis(); 
        return 1;
    }
    else return 0;
}

long nb_tic_encodeur_D_prec;
long nb_tic_encodeur_G_prec;
float dD =0, dG =0;

void ENC_vitesse (void){
    // on fait le delta de chaque roue
    dD = (float)(nb_tic_encodeur_D - nb_tic_encodeur_D_prec) * COEFF_MOTEUR; //nb de ticks toutes les 100 ms sur MD (vitesse)
    nb_tic_encodeur_D_prec = nb_tic_encodeur_D;

    dG = (float)(nb_tic_encodeur_G - nb_tic_encodeur_G_prec) * COEFF_MOTEUR; //nb de ticks toutes les 100 ms sur MG (vitesse)
    nb_tic_encodeur_G_prec = nb_tic_encodeur_G;

    Serial.print(">Vitesse_droite:");
    Serial.println(dD);
    Serial.print(">Vitesse_Gauche:");
    Serial.println(dG);
}

int erreur_D =0, erreur_D_prec =0;
int Integrale_erreur_D =0;
int Derive_erreur_D =0;
int erreur_G =0, erreur_G_prec =0;
int Integrale_erreur_G =0;
int Derive_erreur_G =0;

void PID_vitesse (float KP, float KI, float KD){ 
    erreur_D = consigne_MD - dD;                                                                    //calcul de l'erreur
    Integrale_erreur_D +=  erreur_D;                                                                //Integrale
    Derive_erreur_D = -(dD - erreur_D_prec); // Dérivée sur la mesure != erreur_D - erreur_D_prec;  //Derivée
    commande_MD = KP * erreur_D + KI * Integrale_erreur_D + KD * Derive_erreur_D;                   //PID
    erreur_D_prec = erreur_D;

    erreur_G = consigne_MG - dG;                                                                    //calcul de l'erreur
    Integrale_erreur_G +=  erreur_G;                                                                //Integrale
    Derive_erreur_G = -(dG - erreur_G_prec);                                                        //Derivée
    commande_MG = KP * erreur_G + KI * Integrale_erreur_G + KD * Derive_erreur_G;                   //PID
    erreur_G_prec = erreur_G;

    commande_MD = constrain(commande_MD, -255, 255);
    commande_MG = constrain(commande_MG, -255, 255);
}

void PID_distance (float KP, float KI, float KD){
    erreur_D = consigne_MD - nb_tic_encodeur_D;                                                                    
    Integrale_erreur_D +=  erreur_D;                                                             
    Derive_erreur_D = erreur_D - erreur_D_prec;  
    commande_ticks_MD = KP * erreur_D + KI * Integrale_erreur_D + KD * Derive_erreur_D;                   
    erreur_D_prec = erreur_D;

    if (commande_ticks_MD > 355) commande_MD =255;
    else commande_MD = map(commande_MD,0,355,20,255);//map
    commande_MD = constrain(commande_MD, -255, 255);
}


