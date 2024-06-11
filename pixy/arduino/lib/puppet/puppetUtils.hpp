// Path: lib/puppet/puppetUtils.h
#ifndef PUPPETUTILS_H_INCLUDED // si la librairie n'est pas incluse
#define PUPPETUTILS_H_INCLUDED  // inclure la librairie (_INCLUDED permet d'importer les variables et fonctions de la librairie)

// importer la librairie math pour uitliser les fonctions trigonométriques et les puissances et PI
#include <math.h>

#if defined(__INTELLISENSE__) // si l'IDE est en mode Intellisense (imports pris en compte par l'aide à la saisie)
#include <..\engines\puppetMover.hpp> // importer les classes des moteurs
#include <..\utils\coordinates.hpp> // importer les classes de gestion des coordonnées
#include <Arduino.h> // importer la librairie Arduino pour la classe String et la fonction constrain
#endif

// variables de position
int x; // position x du torse
int right_x; // position x de la main droite
int left_x; // position x de la main gauche

int y; // position y du torse
int right_y; // position y de la main droite
int left_y; // position y de la main gauche


// multiplicateur de position
const static int coef_position = 100; // facteur de multiplication pour la position de la marionette

// Numéros des signatures pour le motion tracking (les signatures des références sont déclarées dans coordinates.hpp)

// torse
static int torso = 5;

// main droite
static int right_hand = 1;

// main gauche
static int left_hand = 4;

// constantes physiques
const static int scene_width = 215; //mm
const int hole_shoulder = 80;
const int arm_length = 61;
const float radius = 40.0;

// variables de mouvement
int arm_angle = 0; // angle du bras en degrés
int last_position = 0; // dernière position de l'utilisateur (en pixels)
int position_on_scene = scene_width / 2; // position de la marionette sur la scène (en mm)
float lc_init= pow(pow(hole_shoulder, 2) + pow(arm_length, 2), .5); // angle initial du bras

// DC motor pins
static int encoder_pin_1 = 2;
static int encoder_pin_2 = 4;
static int direction_pin = 12;
static int power_pin = 3;

// servo pins
static int center_servo_pin = 5;
static int right_servo_pin = 6;
static int left_servo_pin = 7;

// moteurs
SlavedEngine move_engine = SlavedEngine(power_pin, direction_pin, encoder_pin_1, encoder_pin_2); // moteur pour déplacer la marionette sur la scène
ServoMotor left_servo = ServoMotor(left_servo_pin);        // moteur pour déplacer le bras gauche
ServoMotor right_servo = ServoMotor(right_servo_pin);     // moteur pour déplacer le bras droit 
ServoMotor center_servo = ServoMotor(center_servo_pin);  // moteur pour déplacer le torse

// initialisation des moteurs
void init_engine() {
    delay(500); // attendre 5 secondes pour laisser le temps à l'utilisateur de se positionner

    Serial.println("Initializing engine...");

    // stocker la position actuelle
    int previous_position = move_engine.encoder->read();

    // lancer lentement le moteur vers x=x_max (la droite)
    move_engine.power(-255);

    // attendre que le moteur tape la butée (le codeur se fige)
    delay(1000);
    while (move_engine.encoder->read() != previous_position) {
        move_engine.run();
        Serial.print(move_engine.encoder->read());  
        Serial.print(";");
        Serial.println(previous_position);
        delay(1000);
        previous_position = move_engine.encoder->read();
    }

    // arrêter le moteur
    move_engine.power(0);

    // réinitialiser le moteur
    move_engine.reset();

    // envoyer la marionette au centre de la scène
    move_engine.set_destination(105);

    // attendre que le moteur atteigne la destination
    while (move_engine.encoder->read() != move_engine.mm_to_degrees(105)) {
        move_engine.run();
        Serial.print(move_engine.encoder->read());
        Serial.print(";");
        Serial.println(move_engine.mm_to_degrees(105));
        delay(100);
    }

    Serial.println("Engine initialized");

    // attendre 3 secondes pour laisser le temps à l'utilisateur de se positionner
    delay(3000);
}

void process_coords(Coordinates coords){
    // On récupère les coordonnées des repères de référence
    Signature* torso_sig = coords.get_by_id(torso);
    Signature* left_hand_sig = coords.get_by_id(left_hand);
    Signature* right_hand_sig = coords.get_by_id(right_hand);

    // mise à jour des coordonnées
    if (torso_sig != nullptr) {
        x = torso_sig->x;
        y = torso_sig->y;
    }
    if (left_hand_sig != nullptr) {
        left_x = left_hand_sig->x;
        left_y = left_hand_sig->y;
    }
    if (right_hand_sig != nullptr) {
        right_x = right_hand_sig->x;
        right_y = right_hand_sig->y;
    }

    // déplacement de l'utilisateur
    int move = x - last_position; // déplacement de l'utilisateur

    // mise à jour de la position de la marionette
    last_position = x;
    

    // en déduire la position de la marionette
    position_on_scene = position_on_scene + move * coef_position; // position = x% de la scène * largeur de la scène

    // contraindre la position de la marionette à la scène
    position_on_scene = constrain(position_on_scene, 0, scene_width);

    // envoyer la position de la marionette au moteur
    //move_engine.set_destination(position_on_scene);

    // calculer le signal de commande pour asservir le moteur
    //move_engine.run();

    // Mouvement des bras
    // calcul de la longueur de corde nécessaire pour atteindre la main droite
    float hr_right = y - right_y; // hauteur relative de la main droite par rapport à l'épaule droite
    float lc_right = pow(pow(arm_length, 2) + pow(hole_shoulder, 2) - 2*hole_shoulder*hr_right, .5); // longueur de corde nécessaire pour atteindre la main droite

    // calcul de la longueur de corde nécessaire pour atteindre la main gauche
    float hr_left = y - left_y;
    float lc_left = pow(pow(arm_length, 2) + pow(hole_shoulder, 2) - 2*hole_shoulder*hr_left, .5); //

    // calcul des angles nécessaires pour les servos
    float right_angle = (lc_right-lc_init / radius)*180 / PI;
    float left_angle= -(lc_left-lc_init / radius)*180 / PI;

    // envoyer les angles aux servos
    Serial.println("Right angle: " + (String)right_angle);
    Serial.println("Left angle: " + (String)left_angle);
    left_servo.set_angle(left_angle);
    right_servo.set_angle(right_angle);
}

#endif
