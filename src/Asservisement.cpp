 #include <lib.h>

/*--------------------------------------------------------------------------------
  Variable utile : nb_tic_encodeur_D nb_tic_encodeur_G 
  Fonction utile : PWM () Get orientation_by_IMU () Findnorth() EKF()
--------------------------------------------------------------------------------*/ 

int Ticks_to_Distance (int distance){
    float ticks = (distance / (DIAMETRE_ROUE * PI)) * TICKS_PAR_TOUR_DE_ROUE;
    return (int)ticks;
}

int Distance_to_Tick (int ticks){
    float dist = (ticks * (DIAMETRE_ROUE * PI)) / TICKS_PAR_TOUR_DE_ROUE;
    return dist;
}

int erreur_dist;
int I_erreur_dist;
int D_erreur_dist;
int erreur_dist_prec;
int commande_ticks;
int commande_pwm_dist_MD, commande_pwm_dist_MG;

int Avancer(int dist_consige, float KP, float KI, float KD){ //TODO  ajouter une rampe 28:06 min
    //erreurs
    // erreur_dist_D = Ticks_to_Distance(dist_consigne) - nb_tic_encodeur_D; 
    // erreur_dist_G = Ticks_to_Distance(dist_consigne) - nb_tic_encodeur_G; 
    // erreur_dist = (erreur_dist_D+erreur_dist_G)/2;
    erreur_dist = dist_consige;
    //erreur i et d
    I_erreur_dist += erreur_dist;
    D_erreur_dist =  erreur_dist - erreur_dist_prec;
    erreur_dist_prec = erreur_dist;
    //pid
    commande_ticks = KP * erreur_dist + KI * I_erreur_dist + KD * D_erreur_dist;
    
    //cap la valeur max a la vitesse max
    commande_pwm_dist_MD = constrain(abs(commande_ticks), 0, HIGHTEST_PWM-50) * signe(commande_ticks); //TODO -max angle
    commande_pwm_dist_MG = constrain(abs(commande_ticks), 0, HIGHTEST_PWM-50) * signe(commande_ticks);  

    //TODO kick start

    return 0;
}

void Reset_pid_distance (void){
    erreur_dist =0;
    I_erreur_dist =0; 
    D_erreur_dist =0;
    erreur_dist_prec =0;
}

void DEBUG_PID_distance (int consigne_dist){
    Serial.print(">etat_atuel_tick_MD:");               //PID consigne
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


float Angle_restriction(float angle) {
    while (angle > PI) angle -= 2 * PI;
    while (angle < -PI) angle += 2 * PI;
    return angle;
}

float anglerobot;
long nb_tic_encodeur_D_prec =0, nb_tic_encodeur_G_prec =0;

float Get_angle(){   
    const float distanceParPas = (DIAMETRE_ROUE * PI) / TICKS_PAR_TOUR_DE_ROUE;          // mm/pas
    const float facteurAngle = ECART_2_ROUES / distanceParPas;

    long delta_D = nb_tic_encodeur_D - nb_tic_encodeur_D_prec;
    long delta_G = nb_tic_encodeur_G - nb_tic_encodeur_G_prec;

    float deltaAngle = (delta_D - delta_G) / facteurAngle;      // en radians
    anglerobot += deltaAngle;
    anglerobot = Angle_restriction(anglerobot);

    Actualiser_co(delta_D, delta_G);

    nb_tic_encodeur_D_prec = nb_tic_encodeur_D; 
    nb_tic_encodeur_G_prec = nb_tic_encodeur_G;

    return anglerobot * 180.0 / PI; //degres
} 

int erreur_rot;
int I_erreur_rot;
int D_erreur_rot;
int erreur_rot_prec;
int commande_rot;
int commande_pwm_angle_MD, commande_pwm_angle_MG;

int Tourner (int angle, float KP, float KI, float KD){ 
    //erreurs
    erreur_rot = angle - anglerobot * RAD_TO_DEG; 
    //erreur i et d 
    I_erreur_rot += erreur_rot; //TODO anti wind up
    D_erreur_rot =  erreur_rot - erreur_rot_prec; 
    erreur_rot_prec = erreur_rot;
    //pid
    commande_rot = KP * erreur_rot + KI * I_erreur_rot + KD * D_erreur_rot;
    
    //cap la valeur max a la vitesse max
    commande_pwm_angle_MD = constrain(abs(commande_rot), 0, HIGHTEST_PWM) * signe(commande_rot); 
    commande_pwm_angle_MG = - commande_pwm_angle_MD;
    //TODO kick start

    return 0;
}

void Reset_pid_angle(void){
    erreur_rot=0;
    I_erreur_rot =0;
    D_erreur_rot =0;
    erreur_rot_prec =0;
}

void DEBUG_PID_angle (int consigne_angle){
    Serial.print(">etat_angle_IMU:");            //PID consigne
    Serial.println(Find_angle());

    Serial.print(">etat_angle_Tick:");           //PID consigne
    Serial.println(anglerobot* 180.0 / PI);

    Serial.print(">Consigne_attendu_angle:");    //consigne
    Serial.println(consigne_angle);

    Serial.print(">Vitesse_moteur_rot_D:");      //vitesse Moteur
    Serial.println(commande_pwm_angle_MD);

    Serial.print(">Vitesse_moteur_rot_G:");      //vitesse Moteur 
    Serial.println(commande_pwm_angle_MG);
}

void Actualiser_co (int delta_D, int delta_G){
    float dist = Distance_to_Tick((delta_D + delta_G)/2);
    pos_X += (dist * cos(anglerobot));
    pos_Y += (dist * sin(anglerobot));
}

void Discretiser(int nb_de_point, int i){
    float angle = 2*PI*(i/(float)nb_de_point);
    consigne_pos_X += 50 * cos(angle);
    consigne_pos_Y += 50 * sin(angle);
    Serial.print(">angle_rond:");
    Serial.println(angle);
    Serial.print(">i:");
    Serial.println(i);
}