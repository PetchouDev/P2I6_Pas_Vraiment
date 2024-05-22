// Path: lib/puppet/puppetUtils.h
#ifndef PUPPETUTILS_H
#define PUPPETUTILS_H

#include <math.h>
#include <coordinates.hpp>


// Numéros des signatures pour le motion tracking (les signatures des références sont déclarées dans coordinates.hpp)

// épaules
static int shoulder_left = 2;
static int shoulder_right = 3;

// main droite
static int hand_right = 4;

// variables physiques
static int scene_width = 320; //mm
int position = 0; // position de la marionette sur l'image
int position_on_scene = scene_width / 2; // position de la marionette sur la scène
int arm_angle = 0; // angle du bras en degrés

int left_x = 0;
int right_x = 0;

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
    int hole_shoulder = 50
    int arm_length = 61 
    float raduis = 26,25
    float lc_init= (hole_shoulder**2 +arm_lenght**2)**(1/2)
    float hr_right = (right_shoulder->y - right_hand->y)
    float lc_right = (arm_lenght**2 + hole_shoulder**2 - 2*hole_shoulder*hr_right)**(1/2)

    int hole_shoulder = 50
    int arm_length = 61 
    float lc_init= (hole_shoulder**2 +arm_lenght**2)**(1/2)
    float hr_left = (left_shoulder->y - left_hand->y)
    float lc_left = (arm_lenght**2 + hole_shoulder**2 - 2*hole_shoulder*hr_left)**(1/2)

    float right_angle= ((lc_right-lc_init)/radius)*180/PI
    float left_angle= -((lc_left-lc_init)/radius)*180/PI

    // TODO: asservir le moteur pour déplacer le bras
}

#endif
