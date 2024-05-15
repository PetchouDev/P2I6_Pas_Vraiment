// Path: lib/puppet/puppetUtils.h
#ifndef PUPPETUTILS_H
#define PUPPETUTILS_H

#include <math.h>
#include <coordinates.hpp>


// Numéros des signatures pour le motion tracking
// repères de référence
static int reference_left = 0;
static int reference_right = 1;

// épaules
static int shoulder_left = 2;
static int shoulder_right = 3;

// main droite
static int hand_right = 4;

// variables physiques
static int scene_width = 320; //mm
int position = 0; // position de la marionette sur l'image
int position_on_scene = scene_width / 2; // position de la marionette sur la scène

void processCoords(Coordinates* coords){
    // On récupère les coordonnées des repères de référence
    Signature* left_ref = coords->get_by_id(reference_left);
    Signature* right_ref = coords->get_by_id(reference_right);

    // On récupère les coordonnées des épaules
    Signature* left_shoulder = coords->get_by_id(shoulder_left);
    Signature* right_shoulder = coords->get_by_id(shoulder_right);

    // On récupère les coordonnées de la main droite
    Signature* right_hand = coords->get_by_id(hand_right);

    int pos;
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
    position_on_scene = (position - left_ref->x) * (right_ref->x - left_ref->x) * scene_width;

    // TODO: asservir le moteur pour déplacer la marionette
}

#endif
