# 1 "C:\\Users\\mathe\\AppData\\Local\\Temp\\tmp1nfr5mzz"
#include <Arduino.h>
# 1 "C:/Users/mathe/Desktop/JenAiMarre/src/main.ino"
#include <Arduino.h>
#include <Servo.h>


const int left_x_pin = A4;
const int left_y_pin = A3;


const int right_x_pin = A2;
const int right_y_pin = A1;


const int left_servo_pin = 5;
const int right_servo_pin = 6;
const int middle_servo_pin = 7;


float left_servo_position = 90;
float right_servo_position = 90;
float middle_servo_position = 90;


Servo left_servo;
Servo right_servo;
Servo middle_servo;


const int main_engine_power_pin = 3;
const int main_engine_direction_pin = 12;
void setup();
void loop();
#line 31 "C:/Users/mathe/Desktop/JenAiMarre/src/main.ino"
void setup() {

    Serial.begin(19200);


    left_servo.attach(left_servo_pin);
    right_servo.attach(right_servo_pin);
    middle_servo.attach(middle_servo_pin);


    left_servo.write(left_servo_position);
    right_servo.write(right_servo_position);
    middle_servo.write(middle_servo_position);
}

void loop() {

    int left_x = 0;
    int left_y = 0;
    int right_x = 0;
    int right_y = 0;


    left_x = (analogRead(left_x_pin) > 612) ? .5 : (analogRead(left_x_pin) < 412) ? -.5 : 0;
    left_y = (analogRead(left_y_pin) > 612) ? -.5 : (analogRead(left_y_pin) < 412) ? .5 : 0;
    right_x = (analogRead(right_x_pin) > 612) ? .5 : (analogRead(right_x_pin) < 412) ? -.5 : 0;
    right_y = (analogRead(right_y_pin) > 612) ? .5 : (analogRead(right_y_pin) < 412) ? .5 : 0;


    if (Serial.available() > 0) {

        char data[5];
        Serial.readBytesUntil('\n', data, 5);
        Serial.println(data);
        left_x += (data[0] == '1') ? 1 : (data[0] == '2') ? -1 : 0;
        left_y += (data[1] == '1') ? 1 : (data[1] == '2') ? -1 : 0;
        right_x += (data[2] == '1') ? 1 : (data[2] == '2') ? -1 : 0;
        right_y += (data[3] == '1') ? 1 : (data[3] == '2') ? -1 : 0;
    }
# 95 "C:/Users/mathe/Desktop/JenAiMarre/src/main.ino"
    left_servo_position = constrain(left_servo_position - left_y, 5, 175);
    right_servo_position = constrain(right_servo_position + right_y, 5, 175);
    middle_servo_position = constrain(middle_servo_position + left_x, 5, 175);


    left_servo.write(left_servo_position);
    right_servo.write(right_servo_position);
    middle_servo.write(middle_servo_position);


    digitalWrite(main_engine_direction_pin, (right_x == 1) ? LOW : HIGH);
    analogWrite(main_engine_power_pin, right_x == 0 ? 0 : 255);

}