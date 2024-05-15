#include <coordinates.hpp>

Coordinates coords;


void setup() {
    Serial.begin(19200);
    Serial.println("Starting...");
    coords.init();
}

void loop() {
    // read data over the Serial port
    String c = "";

    // if data is available
    if (Serial.available() > 0) {
        // read the data
        c = Serial.readString();
        c.trim();
        Serial.println(">>> " + c);
    }

    if (c == "reload") {
        // reset the pixy
        coords.init();
    } else if (c == "stop") {
        // trigger idle mode
        coords.set_idle(true);
    } else if (c == "start") {
        // trigger normal mode
        coords.set_idle(false);
    } else if (c == "state") {
        // exit the program
        Serial.println("Idle: " + (String)coords.is_idle());
    }

    //Serial.println(idle);
    
    if (!coords.is_idle()) {
        coords.acquire_signatures();
        // TODO: process the coordinates
    }
    delay(100); 
}