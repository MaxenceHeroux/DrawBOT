#include <lib.h>

/*--------------------------------------------------------------------------------
  Variable utile : nb_tic_encodeur_D nb_tic_encodeur_G 
  Fonction utile : PWM () Get orientation_by_IMU () Findnorth() EKF()
--------------------------------------------------------------------------------*/ 
/*
int previousMillis =0; //TODO PID vitesse (on a pas fait tt ca pour rien mdr !)

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

    //main 
    // consigne_MD =100;
    // consigne_MG =100;

    // Serial.print(">Commande_moteur_attendu:");
    // Serial.println(consigne_MD);

    // if (Mini_boucle()){ //calcul des vitesses avant le PID
    //   ENC_vitesse();
    //   PID_vitesse(1.1,0.001,-0.5);
    // }  
}
*/

int erreur_dist_D =0, erreur_dist_D_prec =0;
int Integrale_erreur_dist_D =0;
int Derive_erreur_dist_D =0;

int erreur_dist_G =0, erreur_dist_G_prec =0;
int Integrale_erreur_dist_G =0;
int Derive_erreur_dist_G =0;

void PID_distance (float KP, float KI, float KD){
    int commande_ticks_MD, commande_ticks_MG;
    //Moteur droit
    erreur_dist_D = consigne_dist_MD - nb_tic_encodeur_D;                                                                    
    Integrale_erreur_dist_D +=  erreur_dist_D;                                                             
    Derive_erreur_dist_D = erreur_dist_D - erreur_dist_D_prec;  
    commande_ticks_MD = KP * erreur_dist_D + KI * Integrale_erreur_dist_D + KD * Derive_erreur_dist_D;                   
    erreur_dist_D_prec = erreur_dist_D;

    if (commande_ticks_MD > 355) commande_dist_MD =255;
    else commande_dist_MD = map(commande_ticks_MD,0,355,LOWEST_PWM,255);//map
    commande_dist_MD = constrain(commande_dist_MD, -255, 255);
    if(abs (commande_dist_MG) <= LOWEST_PWM) commande_dist_MD =0; //dead zone 

    //moteur gauche 
    erreur_dist_G = consigne_dist_MG - nb_tic_encodeur_G;                                                                    
    Integrale_erreur_dist_G +=  erreur_dist_G;                                                             
    Derive_erreur_dist_G = erreur_dist_G - erreur_dist_G_prec;  
    commande_ticks_MG = KP * erreur_dist_G + KI * Integrale_erreur_dist_G + KD * Derive_erreur_dist_G;                   
    erreur_dist_G_prec = erreur_dist_G;

    if (commande_ticks_MG > 355) commande_dist_MG =255;
    else commande_dist_MG = map(commande_ticks_MG,0,355,LOWEST_PWM,255);//map
    commande_dist_MG = constrain(commande_dist_MG, -255, 255);
    if(abs (commande_dist_MG) <= LOWEST_PWM) commande_dist_MG =0; //dead zone 
}

void DEBUG_PID_distance (void){
    Serial.print(">Consigne_tic_D:");         //PID consigne
    Serial.println(nb_tic_encodeur_D);

    Serial.print(">Consigne_attendu_tic_D:"); //consigne
    Serial.println(consigne_dist_MD);

    Serial.print(">Vitesse_moteur_D:");       //vitesse Moteur
    Serial.println(commande_dist_MD);

    Serial.print(">Consigne_tic_G:");         //PID consigne
    Serial.println(nb_tic_encodeur_G);

    Serial.print(">Consigne_attendu_tic_G:"); //consigne
    Serial.println(consigne_dist_MG);

    Serial.print(">Vitesse_moteur_G:");       //vitesse Moteur 
    Serial.println(commande_dist_MG);
}

float Ticks_to_Distance (int distance){
    float nb_de_ticks_par_roues = 2100;
    float diametre_roue = 90;
    float ticks = (distance / (diametre_roue * PI)) * nb_de_ticks_par_roues;
    return ticks;
}

int erreur_rot_D =0, erreur_rot_D_prec =0;
int Integrale_erreur_rot_D =0;
int Derive_erreur_rot_D =0;

// int erreur_rot_G =0, erreur_rot_G_prec =0;
// int Integrale_erreur_rot_G =0;
// int Derive_erreur_rot_G =0;
int commande_rot_deg_MD, commande_rot_deg_MG;

void PID_rotation(float KP, float KI, float KD){
    //Moteur droit
    erreur_rot_D = consigne_rot_MD - Find_angle();                                                                  
    Integrale_erreur_rot_D +=  erreur_rot_D;    
    Derive_erreur_rot_D = erreur_rot_D - erreur_rot_D_prec;  
    commande_rot_deg_MD = KP * erreur_rot_D + KI * Integrale_erreur_rot_D + KD * Derive_erreur_rot_D;                   
    erreur_rot_D_prec = erreur_rot_D;

    commande_rot_MD = map(commande_rot_deg_MD, -180, 180, -255, 255);
    commande_rot_MD = constrain(commande_rot_MD, -255, 255);

    if (abs(commande_rot_MD) < LOWEST_PWM) { // Dead zone
        commande_rot_MD = 0;
    }

    //apliquer une roation opossé aux deux moteurs
    commande_rot_MG = commande_rot_MD;
    commande_rot_MD = -commande_rot_MD;
}

void DEBUG_PID_angle (void){
    Serial.print(">Consigne_angle_D:");         //PID consigne
    Serial.println(commande_rot_deg_MD);

    Serial.print(">Consigne_attendu_angle_D:"); //consigne
    Serial.println(consigne_rot_MD);

    Serial.print(">Vitesse_moteur_D:");       //vitesse Moteur
    Serial.println(commande_rot_MD);

    Serial.print(">Vitesse_moteur_G:");       //vitesse Moteur 
    Serial.println(commande_rot_MG);
}