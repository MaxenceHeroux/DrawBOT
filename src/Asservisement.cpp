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

    //Met à jour le point de départ
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
                    i = 0;
                    break;
            }
            break;
        }
        case 2: { // Carré imbriqué
            int reste = i % 4;        // Côté du carré en cours (0 à 3)
            int entier = i / 4;       // Numéro du carré (combien déjà tracés)

            if (entier <= nbCarre) {
                int demiCote = coteCarre / 2;
                int r = entier * ecartCarre;

                // Coordonnées des coins du carré courant
                int x1 = x0 - demiCote + r;  // gauche
                int x2 = x0 + demiCote - r;  // droite
                int y1 = y0 - demiCote + r;  // bas
                int y2 = y0 + demiCote - r;  // haut

                switch (reste) {
                    case 0: // Aller à droite (bas)
                        consigne_pos_X = x2;
                        consigne_pos_Y = y1;
                        break;

                    case 1: // Monter (droite)
                        consigne_pos_X = x2;
                        consigne_pos_Y = y2;
                        break;

                    case 2: // Aller à gauche (haut)
                        consigne_pos_X = x1;
                        consigne_pos_Y = y2;
                        break;

                    case 3: // Descendre (gauche)
                        consigne_pos_X = x1;
                        consigne_pos_Y = y1;
                        break;
                }
            } else {
                mode = 0;       // Fin de la séquence
                first_call = true;
                i = 0;
            }
            break;
        }

        // case 2:{ //Carré imbriqué
        //     int reste = i % 4; //Correspond au côté qu'on est en train de tracé
        //     int entier = i/4; //Entier est le nombre de carré(s) tracé(s)

        //     if(entier <= nbCarre){
        //         switch (reste) {
        //             case 0:{
        //                 consigne_pos_X = x0 + coteCarre - entier*ecartCarre;
        //                 consigne_pos_Y = pos_Y;
        //                 break;
        //             }
        //             case 1:{
        //                 consigne_pos_Y = y0 - coteCarre + entier*ecartCarre;
        //                 consigne_pos_X = pos_X;
        //                 break;
        //             }
        //             case 2:{
        //                 consigne_pos_X = x0 + entier*ecartCarre;
        //                 consigne_pos_Y = pos_Y;
        //                 break;
        //             }
        //             case 3:{
        //                 consigne_pos_Y = y0 - ((entier + 1) * 50);
        //                 consigne_pos_X = pos_X;
        //                 break;
        //             }

        //             default:{
        //                 break;
        //             }
        //         }
        //     } else {
        //         mode = 0; //Arrêt
        //         first_call = true;
        //         i = 0;
        //     }
        //     break;
        // }
        case 3:{  //Cercle
            if(i>nbPoint){
                mode = 0;
                i = 0;
                first_call = true;
            }
            float angle = 2*PI*(i/(float)nbPoint);
            //Pour un départ à (0, 0) :
            consigne_pos_X = radius * cos(angle); //= ou +=
            consigne_pos_Y = radius * sin(angle); //= ou +=

            // Pour n'importe quelle coordonnées :
            // consigne_pos_X = x0 + radius * cos(angle);
            // consigne_pos_Y = y0 + radius * sin(angle);

            break;
        }
        case 4:{  //Rosace
            // Paramètres de la rosace
            int nb_petales = 6;       // Nombre de pétales
            float amplitude = 30;     // Longueur de chaque pétale
            float rayon_base = 50;    // Rayon du cercle central
            float angle_step = 2 * PI / nbPoint; // Incrément d'angle entre chaque point

            if (i > nbPoint) {
                mode = 0;         // Fin du dessin
                i = 0;
                first_call = true;
            } else {
                float angle = i * angle_step;

                // Rayon variant en fonction de l’angle => rosace
                float r = rayon_base + amplitude * sin(nb_petales * angle);

                consigne_pos_X = x0 + r * cos(angle);
                consigne_pos_Y = y0 + r * sin(angle);
            }
            break;
        } 
        case 5:{  //Rose des vents
            //TODO trouver le Nord

            int taille_lignes_consigne_windrose = 12; //Sans compter le cercle
            float tab_consignes_windrose[taille_lignes_consigne_windrose][2] = {
                {0, 0},
                {100, 0},
                {50, 0},
                {50, -50},
                {50, 50},
                {50, 0},
                {25, -25},
                {75, 25},
                {50, 0},
                {75, -25},
                {25, 25},
                {50, 0}
            };
            
            //Pour un départ à (0, 0) :
            // consigne_pos_X = tab_consignes_windrose[i][0]; //Colonne 0 pour les x
            // consigne_pos_Y = tab_consignes_windrose[i][1]; //Colonne 1 pour les y
            
            // Pour n'importe quelle coordonnées :
            consigne_pos_X = x0 + tab_consignes_windrose[i][0]; //Colonne 0 pour les x
            consigne_pos_Y = y0 + tab_consignes_windrose[i][1]; //Colonne 1 pour les y


            if(i >= taille_lignes_consigne_windrose-1){
                mode = 3; // Fais un cercle
                radius = 50; //Pour que ca corresponde aux croix dessinnées
                i = 0;
                first_call = true;
            }
            break;
        } 
        case 6:{  //Flèche
            //TODO Trouver le Nord

            int taille_lignes_consigne_fleche = 5;
            float tab_consignes_fleche[taille_lignes_consigne_fleche][2] = {
                {0, 0},
                {60, 0},
                {40, -10},
                {40, 10},
                {60, 0}
            };

            //Pour un départ à (0, 0) :
            consigne_pos_X = tab_consignes_fleche[i][0]; //Colonne 0 pour les x
            consigne_pos_Y = tab_consignes_fleche[i][1]; //Colonne 1 pour les y
            
            // Pour n'importe quelle coordonnées :
            consigne_pos_X = x0 + tab_consignes_fleche[i][0]; //Colonne 0 pour les x
            consigne_pos_Y = y0 + tab_consignes_fleche[i][1]; //Colonne 1 pour les y

            if(i >= taille_lignes_consigne_fleche-1){
                mode = 0; //arret
                i = 0;
                first_call = true;
            }
            break;
        } 
        default:{
            i = 0;
            first_call = true;
            break;
        }
    }
}