 #include <lib.h>

/*--------------------------------------------------------------------------------
  Variable utile : nb_tic_encodeur_D nb_tic_encodeur_G 
  Fonction utile : PWM () Get orientation_by_IMU () Findnorth() EKF()
--------------------------------------------------------------------------------*/ 
/*

// int previousMillis =0; //TODO PID vitesse EKF distance (on a pas fait tt ca pour rien mdr !) attention a bien renomé les variables 

// int Mini_boucle (void){
//     if( millis() - previousMillis >10000) { //pour la vitesse c est 100 ms 
//         previousMillis = millis(); 
//         return 1;
//     }
//     else return 0;
// }

// long nb_tic_encodeur_D_prec;
// long nb_tic_encodeur_G_prec;
// float dD =0, dG =0;

// void ENC_vitesse (void){
//     // on fait le delta de chaque roue
//     dD = (float)(nb_tic_encodeur_D - nb_tic_encodeur_D_prec) * COEFF_MOTEUR; //nb de ticks toutes les 100 ms sur MD (vitesse)
//     nb_tic_encodeur_D_prec = nb_tic_encodeur_D;

//     dG = (float)(nb_tic_encodeur_G - nb_tic_encodeur_G_prec) * COEFF_MOTEUR; //nb de ticks toutes les 100 ms sur MG (vitesse)
//     nb_tic_encodeur_G_prec = nb_tic_encodeur_G;

//     Serial.print(">Vitesse_droite:");
//     Serial.println(dD);
//     Serial.print(">Vitesse_Gauche:");
//     Serial.println(dG);
// }



// void PID_vitesse (float KP, float KI, float KD){ 
//     erreur_D = consigne_MD - dD;                                                                    //calcul de l'erreur
//     Integrale_erreur_D +=  erreur_D;                                                                //Integrale
//     Derive_erreur_D = -(dD - erreur_D_prec); // Dérivée sur la mesure != erreur_D - erreur_D_prec;  //Derivée
//     commande_MD = KP * erreur_D + KI * Integrale_erreur_D + KD * Derive_erreur_D;                   //PID
//     erreur_D_prec = erreur_D;

//     erreur_G = consigne_MG - dG;                                                                    //calcul de l'erreur
//     Integrale_erreur_G +=  erreur_G;                                                                //Integrale
//     Derive_erreur_G = -(dG - erreur_G_prec);                                                        //Derivée
//     commande_MG = KP * erreur_G + KI * Integrale_erreur_G + KD * Derive_erreur_G;                   //PID
//     erreur_G_prec = erreur_G;

//     commande_MD = constrain(commande_MD, -255, 255);
//     commande_MG = constrain(commande_MG, -255, 255);

//     //main 
//     // consigne_MD =100;
//     // consigne_MG =100;

//     // Serial.print(">Commande_moteur_attendu:");
//     // Serial.println(consigne_MD);

//     // if (Mini_boucle()){ //calcul des vitesses avant le PID
//     //   ENC_vitesse();
//     //   PID_vitesse(1.1,0.001,-0.5);
//     // }  
// }
// */

// int erreur_dist_D =0, erreur_dist_D_prec =0;
// int Integrale_erreur_dist_D =0;
// int Derive_erreur_dist_D =0;

// int erreur_dist_G =0, erreur_dist_G_prec =0;
// int Integrale_erreur_dist_G =0;
// int Derive_erreur_dist_G =0;

// int PID_distance (float KP, float KI, float KD){ //TODO : kp roue droite kp roue gauche etc (sinon moyenne de l avancement et un pid)
//     int commande_ticks_MD, commande_ticks_MG;
//     //Moteur droit
//     erreur_dist_D = consigne_dist_MD - nb_tic_encodeur_D;                                                                    
//     Integrale_erreur_dist_D +=  erreur_dist_D;                                                             
//     Derive_erreur_dist_D = erreur_dist_D - erreur_dist_D_prec;  
//     commande_ticks_MD = KP * erreur_dist_D + KI * Integrale_erreur_dist_D + KD * Derive_erreur_dist_D;                   
//     erreur_dist_D_prec = erreur_dist_D;
//     //TODO Integrale_erreur_dist_D = constrain(Integrale_erreur_dist_D, -I_MAX, I_MAX); //anti wind up

//     int sign_D = signe(commande_ticks_MD);
//     if (abs(commande_ticks_MD) > SEUIL_TICKS_DECELLERATION) commande_dist_MD = 255;
//     else commande_dist_MD = map(abs(commande_ticks_MD), 0, SEUIL_TICKS_DECELLERATION, LOWEST_PWM, 255);
//     commande_dist_MD = abs(commande_ticks_MD) * sign_D;
//     commande_dist_MD = constrain(commande_dist_MD, -255, 255);
//     if (abs(commande_dist_MD) < LOWEST_PWM) commande_dist_MD = 0;

//     //moteur gauche 
//     erreur_dist_G = consigne_dist_MG - nb_tic_encodeur_G;                                                                    
//     Integrale_erreur_dist_G +=  erreur_dist_G;                                                             
//     Derive_erreur_dist_G = erreur_dist_G - erreur_dist_G_prec;  
//     commande_ticks_MG = KP * erreur_dist_G + KI * Integrale_erreur_dist_G + KD * Derive_erreur_dist_G;                   
//     erreur_dist_G_prec = erreur_dist_G;

//     int sign_G = signe(commande_ticks_MG);
//     if (abs(commande_ticks_MG) > SEUIL_TICKS_DECELLERATION) commande_dist_MG = 255;
//     else commande_dist_MG = map(abs(commande_ticks_MG), 0, SEUIL_TICKS_DECELLERATION, LOWEST_PWM, 255);
//     commande_dist_MG = abs(commande_ticks_MG) * sign_G;
//     commande_dist_MG = constrain(commande_dist_MG, -255, 255);
//     if (abs(commande_dist_MG) < LOWEST_PWM) commande_dist_MG = 0;

//     //reset des erreurs
//     if (abs(erreur_dist_D) < 10 && abs(erreur_dist_G) < 10) {
//         reset_PID_distance();
//         return 1;
//     }
//     return 0;
// }

// void reset_PID_distance() {
//     erreur_dist_D = erreur_dist_G = 0;
//     erreur_dist_D_prec = erreur_dist_G_prec = 0;
//     Integrale_erreur_dist_D = Integrale_erreur_dist_G = 0;
//     Derive_erreur_dist_D = Derive_erreur_dist_G = 0;
// }

// void DEBUG_PID_distance (void){
//     Serial.print(">Consigne_tic_D:");         //PID consigne
//     Serial.println(nb_tic_encodeur_D);

//     Serial.print(">Consigne_attendu_tic_D:"); //consigne
//     Serial.println(consigne_dist_MD);

//     Serial.print(">Vitesse_moteur_D:");       //vitesse Moteur
//     Serial.println(commande_dist_MD);

//     Serial.print(">Consigne_tic_G:");         //PID consigne
//     Serial.println(nb_tic_encodeur_G);

//     Serial.print(">Consigne_attendu_tic_G:"); //consigne
//     Serial.println(consigne_dist_MG);

//     Serial.print(">Vitesse_moteur_G:");       //vitesse Moteur 
//     Serial.println(commande_dist_MG);
// }

// float Ticks_to_Distance (int distance){
//     float nb_de_ticks_par_roues = 2100;
//     float diametre_roue = 90;
//     float ticks = (distance / (diametre_roue * PI)) * nb_de_ticks_par_roues;
//     return ticks;
// }

// int erreur_rot_D =0, erreur_rot_D_prec =0;
// int Integrale_erreur_rot_D =0;
// int Derive_erreur_rot_D =0;
// int commande_rot_deg_MD;


// int PID_rotation(float KP, float KI, float KD){ //TODO EKF avec l angle via encodeur (dr-dz)/2
//     //Moteur droit
//     erreur_rot_D = consigne_rot_MD - Find_angle();                                                                  
//     Integrale_erreur_rot_D +=  erreur_rot_D;    
//     Derive_erreur_rot_D = erreur_rot_D - erreur_rot_D_prec;  
//     commande_rot_deg_MD = KP * erreur_rot_D + KI * Integrale_erreur_rot_D + KD * Derive_erreur_rot_D;                   
//     erreur_rot_D_prec = erreur_rot_D;

//     int sign_rot = signe(commande_rot_deg_MD);
//     if(abs(commande_rot_deg_MD)>45){
//         commande_rot_MD = 255 * sign_rot;
//     }else {
//         commande_rot_MD = map(commande_rot_deg_MD, -45, 45, -255, 255);
//     }   
//     commande_rot_MD = constrain(commande_rot_MD, -255, 255);

//     if (abs(commande_rot_MD) < LOWEST_PWM) { // Dead zone
//         commande_rot_MD = 0;
//     }

//     // if (abs(commande_rot_MD) < LOWEST_PWM && abs(commande_rot_deg_MD) > 10) { //FIXME Kick start
//     //     commande_rot_MD = signe(commande_rot_MD) * 80; // kickstart à 80
//     // }

//     //apliquer une roation opossé aux deux moteurs
//     commande_rot_MG = commande_rot_MD;
//     commande_rot_MD = -commande_rot_MD;

//     //reset des erreurs
//     if (abs(erreur_rot_D) < 5 ) { 
//         reset_PID_rotation();
//         return 1;
//     }
//     return 0;
// }

// void reset_PID_rotation() {
//     erreur_rot_D  = 0;
//     erreur_rot_D_prec = 0;
//     Integrale_erreur_rot_D = 0;
//     Derive_erreur_rot_D = 0;
// }

// void DEBUG_PID_angle (void){
//     Serial.print(">Consigne_angle_D:");         //PID consigne
//     Serial.println(commande_rot_deg_MD);

//     Serial.print(">Consigne_attendu_angle_D:"); //consigne
//     Serial.println(consigne_rot_MD);

//     Serial.print(">Vitesse_moteur_D:");         //vitesse Moteur
//     Serial.println(commande_rot_MD);

//     Serial.print(">Vitesse_moteur_G:");         //vitesse Moteur 
//     Serial.println(commande_rot_MG);
// }

// int nb_tic_encodeur_D_prec, nb_tic_encodeur_G_prec;
// int deplacement (int dist, int angle){

//     //nb de tic ne doivent pas interagir 
//     nb_tic_encodeur_D_prec = nb_tic_encodeur_D;
//     nb_tic_encodeur_G_prec = nb_tic_encodeur_G;

//     int commande_MD, commande_MG;
    
//     //consignes
//     consigne_dist_MD =(int)Ticks_to_Distance(dist);  
//     consigne_dist_MG =(int)Ticks_to_Distance(dist); 
//     consigne_rot_MD = angle;

//     if (!PID_rotation(0.7,0,0.05)){ //si roation pas finie
//         digitalWrite(LEDU2,HIGH);
//         digitalWrite(LEDU1,LOW);
//         commande_MD = commande_rot_MD;
//         commande_MG = commande_rot_MG;
//         PWM('D',commande_MD);
//         PWM('G',commande_MG);
//         //nb de tic ne doivent pas interagir 
//         nb_tic_encodeur_D = nb_tic_encodeur_D_prec;
//         nb_tic_encodeur_G = nb_tic_encodeur_G_prec;
//     }else {
//         digitalWrite(LEDU2,LOW);
//         if (!PID_distance(0.3,0.001,0.2)){ //si distance pas finie
//         digitalWrite(LEDU1,HIGH);
//         commande_MD = commande_dist_MD;
//         commande_MG = commande_dist_MG; 
//         PWM('D',commande_MD);
//         PWM('G',commande_MG);
//         }
//     }
   
//     if(PID_distance(0.3,0.001,0.2) && PID_rotation(1.5,0,0)) return 1;

//     return 0;
// }

// void DEBUG_asservissement (int dist , int angle){
//     int commande_MD, commande_MG;
//     if(millis()<3000){
//         //DISTANCE
//         consigne_dist_MD =(int)Ticks_to_Distance(dist);  
//         consigne_dist_MG =(int)Ticks_to_Distance(dist); 
        
//         //TODO  ajouter une rampe 28:06 min  + angle correction 34:05 min 
//         PID_distance(0.3,0.001,0.2); //TODO a regler pid distance 

//         commande_MD = commande_dist_MD;
//         commande_MG = commande_dist_MG; 
//     }else{
//         //ANGLE
//         consigne_rot_MD = angle;

//         PID_rotation(1.5,0,0); //TODO regler angle (dead zone IMU ?) if roue ne tourne pas alors Ki = 0

//         commande_MD = commande_rot_MD;
//         commande_MG = commande_rot_MG;
//     }
    
//     PWM('D',commande_MD);
//     PWM('G',commande_MG);
// }

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

int Avancer (int dist_consigne, float KP, float KI, float KD){
    //erreurs
    erreur_dist_D = Ticks_to_Distance(dist_consigne) - nb_tic_encodeur_D; 
    erreur_dist_G = Ticks_to_Distance(dist_consigne) - nb_tic_encodeur_G; 
    //erreur i et  
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
        PWM('D', commande_pwm_dist_MD);
        PWM('G', commande_pwm_dist_MG);
        //reset erreur 
        Reset_pid_distance();
        return 0;
    } else {
        //commande
        PWM('D', commande_pwm_dist_MD);
        PWM('G', commande_pwm_dist_MG);
        return 1;
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