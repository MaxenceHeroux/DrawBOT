 #include <lib.h>

/*--------------------------------------------------------------------------------
  Variable utile : nb_tic_encodeur_D nb_tic_encodeur_G 
  Fonction utile : PWM () Get orientation_by_IMU () Findnorth() EKF()
--------------------------------------------------------------------------------*/ 

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
    commande_ticks_MD = constrain(commande_ticks_MD, -SEUIL_TICKS_DECELLERATION, SEUIL_TICKS_DECELLERATION); //TODO: Juste contraint = pwm commande cf Tourner
    commande_ticks_MG = constrain(commande_ticks_MG, -SEUIL_TICKS_DECELLERATION, SEUIL_TICKS_DECELLERATION);
    // Mapping pour les petites valeurs
    commande_pwm_dist_MD = map(abs(commande_ticks_MD), 0, SEUIL_TICKS_DECELLERATION, LOWEST_PWM, 255);
    commande_pwm_dist_MG = map(abs(commande_ticks_MG), 0, SEUIL_TICKS_DECELLERATION, LOWEST_PWM, 255);
    commande_pwm_dist_MD *= (commande_ticks_MD >= 0) ? 1 : -1;
    commande_pwm_dist_MG *= (commande_ticks_MG >= 0) ? 1 : -1;
    
    //TODO kick start

    //mouvement fini ?
    if (abs(erreur_dist_D) < 20 && abs(erreur_dist_G) < 20) {
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

int erreur_rot_G, erreur_rot_D;
int I_erreur_rot_D, I_erreur_rot_G;
int D_erreur_rot_D, D_erreur_rot_G;
int erreur_rot_prec_D, erreur_rot_prec_G;
int commande_rot_D, commande_rot_G;
int commande_pwm_angle_MD, commande_pwm_angle_MG;

int Tourner (int angle, float KP, float KI, float KD){ 
    //erreurs
    erreur_rot_D = (angle * TIC_PAR_ANGLE_T_G) - nb_tic_encodeur_D;
    erreur_rot_G = (angle * TIC_PAR_ANGLE_T_D) - nb_tic_encodeur_G;
 
    //erreur i et d 
    I_erreur_rot_D += erreur_rot_D; //TODO anti wind up
    I_erreur_rot_G += erreur_rot_G;
    D_erreur_rot_D =  erreur_rot_D - erreur_rot_prec_D; 
    D_erreur_rot_G =  erreur_rot_G - erreur_rot_prec_G; 
    erreur_rot_prec_D = erreur_rot_D;
    erreur_rot_prec_G = erreur_rot_G;

    //pid
    commande_rot_D = KP * erreur_rot_D + KI * I_erreur_rot_D + KD * D_erreur_rot_D; 
    commande_rot_G = KP * erreur_rot_G + KI * I_erreur_rot_G + KD * D_erreur_rot_G;

    //cap la valeur max a la vitesse max
    commande_pwm_angle_MD = constrain(abs(commande_rot_D), LOWEST_PWM, 255) * signe(commande_rot_D); 
    commande_pwm_angle_MG = constrain(abs(commande_rot_G), LOWEST_PWM, 255) * signe(commande_rot_G); 
   
    //TODO kick start

    //mouvement fini ?
    if (abs(erreur_rot_D) < 5 && abs(erreur_rot_G) < 5) {
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
    erreur_rot_G, erreur_rot_D =0;
    I_erreur_rot_D, I_erreur_rot_G =0;
    D_erreur_rot_D, D_erreur_rot_G =0;
    erreur_rot_prec_D, erreur_rot_prec_G =0;
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