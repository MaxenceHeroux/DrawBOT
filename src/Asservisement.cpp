 #include <lib.h>

/*--------------------------------------------------------------------------------
  Variable utile : nb_tic_encodeur_D nb_tic_encodeur_G 
  Fonction utile : PWM () Get orientation_by_IMU () Findnorth() EKF()
--------------------------------------------------------------------------------*/ 
/*

int previousMillis =0; //TODO PID vitesse EKF distance (on a pas fait tt ca pour rien mdr !) attention a bien renomé les variables 

int Mini_boucle (void){
    if( millis() - previousMillis >10000) { //pour la vitesse c est 100 ms 
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


int Ticks_to_Distance (int distance){
    int nb_de_ticks_par_roues = 2100;
    int diametre_roue = 90;
    float ticks = (distance / (diametre_roue * PI)) * nb_de_ticks_par_roues;
    return (int)ticks;
}

int erreur_dist_D, erreur_dist_G;
int I_erreur_dist_D, I_erreur_dist_G;
int D_erreur_dist_D, D_erreur_dist_G;
int erreur_dist_D_prec, erreur_dist_G_prec;
int commande_ticks_MD, commande_ticks_MG;
int commande_pwm_dist_MD, commande_pwm_dist_MG;

int Avancer (int dist_consigne, float KP, float KI, float KD){ //TODO  ajouter une rampe 28:06 min  + angle correction 34:05 min 
    //erreurs
    erreur_dist_D = Ticks_to_Distance(dist_consigne) - nb_tic_encodeur_D; 
    erreur_dist_G = Ticks_to_Distance(dist_consigne) - nb_tic_encodeur_G; 
    //erreur i et d
    I_erreur_dist_D += erreur_dist_D;
    I_erreur_dist_G += erreur_dist_G; //TODO anti wind up
    D_erreur_dist_D =  erreur_dist_D - erreur_dist_D_prec;
    D_erreur_dist_G =  erreur_dist_G - erreur_dist_G_prec; 
    erreur_dist_D_prec = erreur_dist_D;
    erreur_dist_G_prec = erreur_dist_G;
    //pid
    commande_ticks_MD = KP * erreur_dist_D + KI * I_erreur_dist_D + KD * D_erreur_dist_D;
    commande_ticks_MG = KP * erreur_dist_G + KI * I_erreur_dist_G + KD * D_erreur_dist_G;

    //cap la valeur max a la vitesse max
    commande_ticks_MD = constrain(commande_ticks_MD, -SEUIL_TICKS_DECELLERATION, SEUIL_TICKS_DECELLERATION);
    commande_ticks_MG = constrain(commande_ticks_MG, -SEUIL_TICKS_DECELLERATION, SEUIL_TICKS_DECELLERATION);
    // Mapping pour les petites valeurs
    commande_pwm_dist_MD = map(abs(commande_ticks_MD), 0, SEUIL_TICKS_DECELLERATION, LOWEST_PWM, 255);
    commande_pwm_dist_MG = map(abs(commande_ticks_MG), 0, SEUIL_TICKS_DECELLERATION, LOWEST_PWM, 255);
    commande_pwm_dist_MD *= (commande_ticks_MD >= 0) ? 1 : -1;
    commande_pwm_dist_MG *= (commande_ticks_MG >= 0) ? 1 : -1;
    
    //TODO kick start

    //mouvement fini ?
    if (abs(erreur_dist_D) < 35 && abs(erreur_dist_G) < 35) {
        //PWM a 0
        commande_pwm_dist_MD =0;
        commande_pwm_dist_MG =0;
        //reset erreur 
        Reset_pid_distance();
        return 1;
    } else {
        return 0;
    }
}

void Reset_pid_distance (void){
    erreur_dist_D =0;
    erreur_dist_G =0;
    I_erreur_dist_D =0;
    I_erreur_dist_G =0; 
    D_erreur_dist_D =0;
    D_erreur_dist_G =0; 
    erreur_dist_D_prec =0;
    erreur_dist_G_prec =0;
}

void DEBUG_PID_distance (int consigne_dist){
    Serial.print(">etat_atuel_tick_MD:");                //PID consigne
    Serial.println(nb_tic_encodeur_D);

    Serial.print(">Consigne_attendu_tic_D:");           //consigne
    Serial.println(Ticks_to_Distance(consigne_dist));

    Serial.print(">Vitesse_moteur_D:");                 //vitesse Moteur
    Serial.println(commande_pwm_dist_MD);

    Serial.print(">etat_atuel_tick_MG:");               //PID consigne
    Serial.println(nb_tic_encodeur_G);

    Serial.print(">Consigne_attendu_tic_G:");           //consigne
    Serial.println(Ticks_to_Distance(consigne_dist));

    Serial.print(">Vitesse_moteur_G:");                 //vitesse Moteur 
    Serial.println(commande_pwm_dist_MG);
}

int erreur_rot;
int I_erreur_rot;
int D_erreur_rot;
int erreur_rot_prec;
int commande_angle;
int commande_pwm_angle_MD, commande_pwm_angle_MG;

int Tourner (int angle, float KP, float KI, float KD){ //TODO regler angle (dead zone IMU ?) if roue ne tourne pas alors Ki = 0
    //erreurs
    erreur_rot = angle - Find_angle(); 
    //erreur i et d 
    I_erreur_rot += erreur_rot; //TODO anti wind up
    D_erreur_rot =  erreur_rot - erreur_rot_prec; 
    erreur_rot_prec = erreur_rot;
    //pid
    commande_angle = KP * erreur_rot + KI * I_erreur_rot + KD * D_erreur_rot; //TODO EKF avec l angle via encodeur (dr-dz)/2

    //cap la valeur max a la vitesse max
    commande_angle = constrain(commande_angle, -SEUIL_ANGLE_DECELLERATION, SEUIL_ANGLE_DECELLERATION);
    // Mapping pour les petites valeurs
    commande_pwm_angle_MD = map(abs(commande_angle), 0, SEUIL_ANGLE_DECELLERATION, LOWEST_PWM, 255);
    
    commande_pwm_angle_MD *= (commande_angle >= 0) ? -1 : 1;
    commande_pwm_angle_MG  = - commande_pwm_angle_MD;
   
    //TODO kick start

    //mouvement fini ?
    if (abs(erreur_rot) < 5 ) {
        //PWM a 0
        commande_pwm_angle_MD =0;
        commande_pwm_angle_MG =0;
        //reset erreur 
        Reset_pid_angle();
        return 1;
    } else {
        return 0;
    }
}

void Reset_pid_angle(void){
    erreur_rot = 0;
    I_erreur_rot =0;
    D_erreur_rot =  0; 
    erreur_rot_prec = 0;
}

void DEBUG_PID_angle (int consigne_angle){
    Serial.print(">etat_angle:");                //PID consigne
    Serial.println(Find_angle());

    Serial.print(">Consigne_attendu_angle:");    //consigne
    Serial.println(consigne_angle);

    Serial.print(">Vitesse_moteur_rot_D:");      //vitesse Moteur
    Serial.println(commande_pwm_angle_MD);

    Serial.print(">Vitesse_moteur_rot_G:");      //vitesse Moteur 
    Serial.println(commande_pwm_angle_MG);
}