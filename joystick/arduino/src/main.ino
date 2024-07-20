#include <Arduino.h>
#include <Servo.h>

// pins du joystick gauche
const int left_x_pin = A4;
const int left_y_pin = A3;

// pins du joystick droit
const int right_x_pin = A2;
const int right_y_pin = A1;

// pins des servomoteurs
const int left_servo_pin = 5;
const int right_servo_pin = 6;
const int middle_servo_pin = 7;

// positions des servomoteurs
float left_servo_position = 90;
float right_servo_position = 90;
float middle_servo_position = 90;

// Initialisation des servomoteurs
Servo left_servo;
Servo right_servo;
Servo middle_servo;

// initialisation du moteur principal
const int main_engine_power_pin = 3;
const int main_engine_direction_pin = 12;

void setup() {
    // Initialisation de la communication série pour le débogage
    Serial.begin(19200);

    // Initialisation des servomoteurs
    left_servo.attach(left_servo_pin);
    right_servo.attach(right_servo_pin);
    middle_servo.attach(middle_servo_pin);

    // servos en position neutre
    left_servo.write(left_servo_position);
    right_servo.write(right_servo_position);
    middle_servo.write(middle_servo_position);
}

void loop() {
    // Déclarer des variables pour stocker des sens de mouvements (-1, 0, 1)
    float left_x = 0;
    float left_y = 0;
    float right_x = 0;
    float right_y = 0;

    // Lire les valeurs des joysticks (avec une tolérance de 100 autour de la valeur neutre de 512) - axes Y inversés
    left_x = (analogRead(left_x_pin) > 612) ? 1 : (analogRead(left_x_pin) < 412) ? -1 : 0;
    left_y = (analogRead(left_y_pin) > 612) ? -1 : (analogRead(left_y_pin) < 412) ? 1 : 0;
    right_x = (analogRead(right_x_pin) > 612) ? 1 : (analogRead(right_x_pin) < 412) ? -1 : 0;
    right_y = (analogRead(right_y_pin) > 612) ? 1 : (analogRead(right_y_pin) < 412) ? -1 : 0;

    // lecture du moniteur série
    if (Serial.available() > 0) {
        // Lire les valeurs des servomoteurs
        char data[5];
        Serial.readBytesUntil('\n', data, 5);
        Serial.println(data);
        left_x += (data[0] == '1') ? 1 : (data[0] == '2') ? -1 : 0;
        left_y += (data[1] == '1') ? 1 : (data[1] == '2') ? -1 : 0;
        right_x += (data[2] == '1') ? 1 : (data[2] == '2') ? -1 : 0;
        right_y += (data[3] == '1') ? 1 : (data[3] == '2') ? -1 : 0;
    }

    /* Serial.print(left_x);
    Serial.print(";");
    Serial.print(left_y);
    Serial.print(";");
    Serial.print(right_x);
    Serial.print(";");
    Serial.print(right_y);
    Serial.print(";");
    Serial.print(left_servo_position);
    Serial.print(";");
    Serial.print(right_servo_position);
    Serial.print(";");
    Serial.println(middle_servo_position); */


    /*
    Liens axes => moteurs:
        - Axe X gauche => servomoteur central
        - Axe Y gauche => servomoteur gauche
        - Axe X droit => moteur principal
        - Axe Y droit => servomoteur droit
    */
    
    // incrémentation des valeurs des servomoteurs
    left_servo_position = constrain(left_servo_position - left_y, 5, 175);
    right_servo_position = constrain(right_servo_position + right_y, 5, 175);
    middle_servo_position = constrain(middle_servo_position + left_x, 5, 175);

    // Mise à jour des servomoteurs
    left_servo.write(left_servo_position);
    right_servo.write(right_servo_position);
    middle_servo.write(middle_servo_position);

    // Mise à jour du moteur principal
    digitalWrite(main_engine_direction_pin, (right_x == 1) ? LOW : HIGH);
    analogWrite(main_engine_power_pin, right_x == 0 ? 0 : 255);

}
