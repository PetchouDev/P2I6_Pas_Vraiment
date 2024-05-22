#ifndef PUPPETUTILS_H
#define PUPPETUTILS_H

#include <Encoder.h> 


class SlavedEngine {
    protected:
        int destination = 0; // angle d'asservissement
        int position = 0; // position angulaire
        bool running = false; // état de l'asservissement

    
    private:
        // constantes
        const float kp = 0.1; // coefficient proportionnel
        const float ki = 0.1; // coefficient intégral
        const float kd = 0.1; // coefficient dérivé

        // variables
        long unsigned int t0 = 0; // temps précédent
        int previous_error = 0; // erreur précédente
        int integral = 0; // somme des erreurs

        // paramètres du moteur
        int power_pin; // pin de commande du moteur
        int encoder_pin_1; // pin 1 du codeur
        int encoder_pin_2; // pin 2 du codeur
        Encoder* encoder; // pointeur vers le codeur

    public:
        SlavedEngine(int power_pin, int encoder_pin_1, int encoder_pin_2) {
            // initialisation du codeur
            this->encoder = new Encoder(encoder_pin_1, encoder_pin_2);

            // initialisation du moteur
            pinMode(power_pin, OUTPUT);
            digitalWrite(power_pin, LOW);

            this->power_pin = power_pin;
            this->encoder_pin_1 = encoder_pin_1;
            this->encoder_pin_2 = encoder_pin_2;

            this->reset();
        }

        void set_destination(int dest) {
            this->destination = dest;
        }

        void set_position(int pos) {
            this->position = pos;
        }

        void set_running(bool run) {
            this->running = run;
        }

        void reset() {
            this->destination = 0;
            this->position = 0;
            this->running = false;
            this->t0 = 0;
            this->integral = 0;
            this->encoder->write(0); // reset la position accumulée du codeur
        }

        void run() {
            if (this->running) {
                // obtenir la position via le codeur du moteur
                this->position = this->encoder->read();

                // calcul de l'erreur
                int error = this->destination - this->position;

                // calcul de la dérivée
                float derivative = (error - this->previous_error) / (millis() - this->t0);

                // calcul de l'intégrale
                this->integral += error;

                // calcul de la commande
                float command = this->compute_command(error, derivative);

                // obentir la direction
                bool clockwise = command > 0;
                command = abs(command);

                // appliquer la commande
                this->set_direction(clockwise);
                this->set_power(command);
                
            }
        }

    protected:
        void set_power(int power) {
            analogWrite(this->power_pin, power);
        }

        void set_direction(bool clockwise) {
            digitalWrite(this->encoder_pin_1, clockwise ? HIGH : LOW);
            digitalWrite(this->encoder_pin_2, clockwise ? LOW : HIGH);
        }

        float compute_command(int error, float derivative) {
            return this->kp * error + this->ki * this->integral + this->kd * derivative;
        }
};

#endif