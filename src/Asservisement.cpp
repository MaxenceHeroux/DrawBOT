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
    erreur_rot = Angle_restriction((angle * DEG_TO_RAD) - anglerobot) * RAD_TO_DEG; //angle - anglerobot * RAD_TO_DEG; 

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

void Discretiser(){
    static float x0 = 0;
    static float y0 = 0;
    static bool first_call = true;

    if (mode > 0 && first_call) {
        x0 = pos_X;
        y0 = pos_Y;
        first_call = false;
    }

    switch (mode){
        case 1: { // Escalier
            switch (i) {
                case 1: case 2: case 3: case 4: case 5:
                    consigne_pos_X = x0 + 40 * i;
                    consigne_pos_Y = y0;
                    break;
                case 6: case 7: case 8: case 9: case 10:
                    consigne_pos_X = x0 + 200;
                    consigne_pos_Y = y0 + 20 * (i - 5);
                    break;
                case 11: case 12: case 13: case 14: case 15:
                    consigne_pos_X = x0 + 200 + 80 * (i - 10);
                    consigne_pos_Y = y0 + 100;
                    break;
                default:
                    mode = 0; //= arret
                    first_call = true;
                    break;
            }
            break;
        }
        // case 2:{ //Carré dans carré etc
        //     static int direction = 0; // 0: droite, 1: haut, 2: gauche, 3: bas
        //     static float currentLength = coteCarre; // Longueur initiale du côté
        //     static int step_in_square = 0; // Étape à l’intérieur d’un carré
        //     static int squareIndex = 0;    // Combien de carrés on a fait

        //     const int steps_per_side = 5; // Divise chaque côté en 5 petits pas
        //     float stepLength = currentLength / steps_per_side;

        //     if (squareIndex >= nbCarre) {
        //         mode = 0; // Stop
        //         break;
        //     }

        //     switch (direction % 4) {
        //         case 0: // droite
        //             consigne_pos_X = pos_X + stepLength;
        //             consigne_pos_Y = pos_Y;
        //             break;
        //         case 1: // haut
        //             consigne_pos_X = pos_X;
        //             consigne_pos_Y = pos_Y + stepLength;
        //             break;
        //         case 2: // gauche
        //             consigne_pos_X = pos_X - stepLength;
        //             consigne_pos_Y = pos_Y;
        //             break;
        //         case 3: // bas
        //             consigne_pos_X = pos_X;
        //             consigne_pos_Y = pos_Y - stepLength;
        //             break;
        //     }

        //     step_in_square++;
        //     if (step_in_square >= steps_per_side) {
        //         step_in_square = 0;
        //         direction++;
        //         if (direction % 2 == 0) {
        //             currentLength *= 0.8; // Le carré devient plus petit tous les 2 virages
        //             squareIndex++;
        //         }
        //     }

        //     break;
        // }
        case 2:{ //Carré dans carré etc
            int reste = i % 4; //Correspond au côté qu'on est en train de tracé
            int entier = i/4; //Entier est le nombre de carré(s) tracé(s)

            if(entier <= nbCarre){
                switch (reste) {
                    case 0:{
                        consigne_pos_X = x0 + coteCarre - entier*ecartCarre;
                        consigne_pos_Y = pos_Y;
                        break;
                    }
                    case 1:{
                        consigne_pos_Y = y0 - coteCarre + entier*ecartCarre;
                        consigne_pos_X = pos_X;
                        break;
                    }
                    case 2:{
                        consigne_pos_X = x0 + entier*ecartCarre;
                        consigne_pos_Y = pos_Y;
                        break;
                    }
                    case 3:{
                        consigne_pos_Y = y0 - ((entier + 1) * 50);
                        consigne_pos_X = pos_X;
                        break;
                    }

                    default:{
                        break;
                    }
                }
            } else {
                mode = 0; //Arrêt
            }
            break;
        }
        case 3:{  //Cercle
            if(i>nbPoint){
                mode = 0;
                first_call = true;
            }
            float angle = 2*PI*(i/(float)nbPoint);
            consigne_pos_X = radius * cos(angle); //= ou +=
            consigne_pos_Y = radius * sin(angle); //= ou +=
            break;
        }
        case 4:{  //Rosace
            consigne_pos_X = pos_X + 200;
            mode = 0;
            first_call = true;
            break;
        } 
        case 5:{  //Rose des vents
            break;
        } 
        case 6:{  //Flêche
            break;
        } 
        default:{
            i =0;
            first_call = true;
            break;
        }
    }
}