#ifndef PUPPETUTILS_H
#define PUPPETUTILS_H

#include <Encoder.h> 
#include <Servo.h> 


// Classe pour asservir un moteur EMG30
// Classe pour asservir un moteur EMG30
class SlavedEngine {
    public:
        int destination = 0; // angle d'asservissement
        int position = 0; // position angulaire

        Encoder* encoder; // pointeur vers le codeur

    private:
        // constantes
        const float kp = 1.25; // coefficient proportionnel

        // paramètres physiques
        const float pas = 40; // en mm/tour

        // paramètres du moteur
        int power_pin; // pin de commande du moteur
        int direction_pin; // pin de direction du moteur


    public:
        SlavedEngine(int power_pin, int direction_pin, int encoder_pin_1, int encoder_pin_2, float kp=4.0) {
            // initialisation du codeur
            this->encoder = new Encoder(encoder_pin_1, encoder_pin_2);

            // initialisation du moteur
            pinMode(power_pin, OUTPUT);
            digitalWrite(power_pin, LOW);

            pinMode(direction_pin, OUTPUT);

            // initialisation des paramètres
            this->power_pin = power_pin;
            this->direction_pin = direction_pin;

            // initialisation des variables
            this->reset();
        }

        void set_destination(int dest) {
            // conversion de la destination de mm en degrés
            this->destination = this->mm_to_degrees(dest); // régler la destination sur scene en mm
        }

        void reset() {
            this->destination = 0;
            this->position = 0;
            this->encoder->write(0); // reset la position accumulée du codeur
        }

        void run() {
            // obtenir la position via le codeur du moteur
            this->position = this->encoder->read();
            long erreur = this->position - this->destination;
            int commande = constrain(this->kp * erreur, -255, 255);

            this->power(commande);

            Serial.print(this->position);
            Serial.print(";");
            Serial.print(this->destination);
            Serial.print(";");
            Serial.print(erreur);
            Serial.print(";");
            Serial.println(commande);
    
        }

        // Méthode pour convertir la commande pour le moteur
        void power(int vitesse) {
            digitalWrite(12, vitesse < 0 ? HIGH : LOW);
            analogWrite(3, abs(vitesse));
        }

        // Méthode pour convertir les disnatances en degrés
        int mm_to_degrees(int mm) {
            return mm * 360 / this->pas;
        }

        // Méthode pour convertir les degrés en distances
        int degrees_to_mm(int degrees) {
            return degrees * this->pas / 360;
        }
};

// Classe pour contrôler un servo moteur
class ServoMotor {
    public:
        int pin; // pin du servo

    protected:
        Servo motor; // instance du servo
    
    public:
        // Constructeur
        ServoMotor(int angle_pin) {
            this->pin = pin; // pin du servo
            this->motor.attach(pin); // attacher le servo au pin
            pinMode(pin, OUTPUT); // définir le pin en sortie
        }

        // Méthode pour régler l'angle du servo
        void set_angle(int angle) {
            this->motor.write(angle);
        }

        // Méthode pour obtenir l'angle du servo
        int get_angle() {
            return this->motor.read();
        }

        // Méthode pour réinitialiser le servo
        void reset() {
            // remettre le servo à 90 (position neutre)
            this->motor.write(90);
        }
};

#endif