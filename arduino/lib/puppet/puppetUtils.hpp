// Path: lib/puppet/puppetUtils.h
#ifndef PUPPETUTILS_H
#define PUPPETUTILS_H

#include <math.h>

#include <coordinates.hpp>
# include <puppetMover.hpp>


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
const int hole_shoulder = 50;
const int arm_length = 61;


// variables de mouvement
int arm_angle = 0; // angle du bras en degrés
int position = 0; // position de la marionette sur l'image
int position_on_scene = scene_width / 2; // position de la marionette sur la scène
float lc_init= pow(pow(hole_shoulder, 2) + pow(arm_length, 2), .5); // angle initial du bras


int left_x = 0;
int right_x = 0;

// DC motor pins
static int power_pin = 6;
static int encoder_pin_1 = 2;
static int encoder_pin_2 = 3;

// moteurs
SlavedEngine* move_engine = new SlavedEngine(power_pin, encoder_pin_1, encoder_pin_2);

void get_initial_references(Coordinates coords) {
    // On récupère les coordonnées des repères de référence
    while (left_x == 0 || right_x == 0) {
        Signature* left_ref = coords.get_by_id(reference_left);
        Signature* right_ref = coords.get_by_id(reference_right);
        if (left_ref != nullptr) {
            left_x = left_ref->x;
        }
        if (right_ref != nullptr) {
            right_x = right_ref->x;
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
        // On calcule la position de la marionet
        position = (left_shoulder->x + right_shoulder->x) / 2;
    } else if (left_shoulder != nullptr) {
        position = left_shoulder->x;
    } else if (right_shoulder != nullptr) {
        position = right_shoulder->x;
    }

    // On met à jour la position de la marionette sur la scène
    position_on_scene = (position - left_x) * (left_x - right_x) * scene_width;

    // TODO: asservir le moteur pour déplacer la marionette
    move_engine->set_destination(position_on_scene); // on déplace la marionette à la position calculée
    move_engine->run(); // on fait tourner le moteur


    // calculer l'angle du bras
    // si le bonhomme est de face
    if (left_shoulder != nullptr && right_shoulder != nullptr && right_hand != nullptr) {
        // on calcule l'angle du bras
        int dx = abs(left_shoulder->x - right_shoulder->x) * 1.2; // on approxime la longueur du bras comme étant 1.2 fois la distance entre les épaules
        int dy = left_shoulder->y - right_hand->y;

        // on calcule l'angle (en degrés)
        arm_angle = atan(dy / dx) * 180 / PI;

    } else if (left_shoulder != nullptr && right_hand != nullptr) {

        // on calcule l'angle du bras
        int dx = abs(left_shoulder->x - right_hand->x);
        int dy = abs(left_shoulder->y - right_hand->y);

        // on calcule l'angle (en degrés)
        arm_angle = atan(dy / dx) * 180 / PI;

    } else if (right_shoulder != nullptr && right_hand != nullptr) {
        
        // on calcule l'angle du bras
        int dx = abs(right_shoulder->x - right_hand->x);
        int dy = abs(right_shoulder->y - right_hand->y);

        // on calcule l'angle (en degrés)
        arm_angle = atan(dy / dx) * 180 / PI;
    }
    
    
    float hr_right = (right_shoulder->y - right_hand->y); // hauteur relative de la main droite par rapport à l'épaule droite
    float lc_right = pow(pow(arm_length, 2) + pow(hole_shoulder, 2) - 2*hole_shoulder*hr_right, .5); // longueur de corde nécessaire pour atteindre la main droite

    float hr_left = right_shoulder->y - right_hand->y;
    float lc_left = pow(pow(arm_length, 2) + pow(hole_shoulder, 2) - 2*hole_shoulder*hr_left, .5); //

    float right_angle= (lc_right-lc_init / radius)*180/PI;
    float left_angle= -(lc_left-lc_init / radius)*180/PI;

    // TODO: asservir le moteur pour déplacer le bras
}

#endif
