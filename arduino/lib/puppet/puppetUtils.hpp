// Path: lib/puppet/puppetUtils.h
#ifndef PUPPETUTILS_H_INCLUDED // si la librairie n'est pas incluse
#define PUPPETUTILS_H_INCLUDED  // inclure la librairie (_INCLUDED permet d'importer les variables et fonctions de la librairie)

// importer la librairie math pour uitliser les fonctions trigonométriques et les puissances et PI
#include <math.h>

#if defined(__INTELLISENSE__) // si l'IDE est en mode Intellisense (imports pris en compte par l'aide à la saisie)
#include <..\engines\puppetMover.hpp> // importer les classes des moteurs
#include <..\utils\coordinates.hpp> // importer les classes de gestion des coordonnées
#endif


// Numéros des signatures pour le motion tracking (les signatures des références sont déclarées dans coordinates.hpp)

// épaules
static int shoulder_left = 2;
static int shoulder_right = 3;

// main droite
static int hand_right = 4;

// constantes physiques
const static int scene_width = 320; //mm
const int hole_shoulder = 50;
const int arm_length = 61;
const float radius = 26.25;

// variables de mouvement
int arm_angle = 0; // angle du bras en degrés
int position = 0; // position de la marionette sur l'image
int position_on_scene = scene_width / 2; // position de la marionette sur la scène
float lc_init= pow(pow(hole_shoulder, 2) + pow(arm_length, 2), .5); // angle initial du bras

// coordonnées des repères de référence
int left_x = 0;
int right_x = 0;

// DC motor pins
static int encoder_pin_1 = 2;
static int encoder_pin_2 = 3;
static int direction_pin = 4;
static int power_pin = 5;

// servo pins
static int right_servo_pin = 9;
static int left_servo_pin = 10;

// moteurs
SlavedEngine* move_engine = new SlavedEngine(power_pin, direction_pin, encoder_pin_1, encoder_pin_2); // moteur pour déplacer la marionette sur la scène
ServoMotor* left_servo = new ServoMotor(left_servo_pin);        // moteur pour déplacer le bras gauche
ServoMotor* right_servo = new ServoMotor(right_servo_pin);    // moteur pour déplacer le bras droit

void get_initial_references(Coordinates coords) {

    // On récupère les coordonnées des repères de référence
    while (left_x == 0 || right_x == 0) {

        // vérifier si les références sont disponibles
        if (coords.left_ref_available) {

            // on récupère les coordonnées de la référence gauche
            left_x = coords.get_by_id(reference_left)->x;
        }
        if (coords.right_ref_available) {

            // on récupère les coordonnées de la référence droite
            right_x = coords.get_by_id(reference_right)->x;
        }
    }
}

void process_coords(Coordinates coords){
    // On récupère les coordonnées des repères de référence
    if (coords.left_ref_available) {
        left_x = coords.get_by_id(reference_left)->x;
    }
    if (coords.right_ref_available) {
        right_x = coords.get_by_id(reference_right)->x;
    }

    // On récupère les coordonnées des épaules
    Signature* left_shoulder = coords.get_by_id(shoulder_left);
    Signature* right_shoulder = coords.get_by_id(shoulder_right);

    // On récupère les coordonnées de la main droite
    Signature* right_hand = coords.get_by_id(hand_right);

    // calcul de la position de la marionette si c'est possible
    if (left_shoulder != nullptr && right_shoulder != nullptr) {

        // Si les deux épaules sont détectées, on place la marionette au milieu (moyenne)
        position = (left_shoulder->x + right_shoulder->x) / 2;

    // Si une seule épaule est détectée, on place la marionette à la position de l'épaule
    } else if (left_shoulder != nullptr) {

        position = left_shoulder->x;

    } else if (right_shoulder != nullptr) {

        position = right_shoulder->x;

    }

    // On met à jour la position de la marionette sur la scène
    position_on_scene = (position - left_x) * (left_x - right_x) * scene_width; // position = x% de la scène * largeur de la scène

    // envoyer la position de la marionette au moteur
    move_engine->set_destination(position_on_scene);

    // asservir le moteur pour déplacer la marionette
    move_engine->run();

    // Mouvement des bras
    // calcul de la longueur de corde nécessaire pour atteindre la main droite
    float hr_right = (right_shoulder->y - right_hand->y); // hauteur relative de la main droite par rapport à l'épaule droite
    float lc_right = pow(pow(arm_length, 2) + pow(hole_shoulder, 2) - 2*hole_shoulder*hr_right, .5); // longueur de corde nécessaire pour atteindre la main droite

    // calcul de la longueur de corde nécessaire pour atteindre la main gauche
    float hr_left = right_shoulder->y - right_hand->y;
    float lc_left = pow(pow(arm_length, 2) + pow(hole_shoulder, 2) - 2*hole_shoulder*hr_left, .5); //

    // calcul des angles nécessaires pour les servos
    float right_angle = (lc_right-lc_init / radius)*180 / PI;
    float left_angle= -(lc_left-lc_init / radius)*180 / PI;

    // envoyer les angles aux servos
    left_servo->set_angle(left_angle);
    right_servo->set_angle(right_angle);
}

#endif
