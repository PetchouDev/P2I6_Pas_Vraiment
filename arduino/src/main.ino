#include <PIDLoop.h>
#include <Pixy2.h>
#include <Pixy2CCC.h>
#include <Pixy2I2C.h>
#include <Pixy2Line.h>
#include <Pixy2SPI_SS.h>
#include <Pixy2Video.h>
#include <TPixy2.h>
#include <ZumoBuzzer.h>
#include <ZumoMotors.h>
#include <SPI.h>

#include <coordinates.hpp>
#include <puppetMover.hpp>

Coordinates* coords;
Pixy2 pixy;

void setup() {
    Serial.begin(19200);
    Serial.println("Starting...");
    pixy.init();
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
        pixy.init();
    } else if (c == "stop") {
        // trigger idle mode
        coords->set_idle(true);
    } else if (c == "start") {
        // trigger normal mode
        coords->set_idle(false);
    } else if (c == "state") {
        // exit the program
        Serial.println("Idle: " + (String)coords->is_idle());
    }

    if (!coords->is_idle()) {


        // acquire objects positions from the pixy
        int attempts = 0;
        while (pixy.ccc.getBlocks() == 0 && attempts < 3) {
            delay(100);
            attempts++;
        }

        // if no objects are detected, reinitialize the pixy
        if (attempts >= 3) {
            Serial.println("No objects detected, reinitializing...");
            pixy.init();
        }

        // if objects are detected, process them
        if (pixy.ccc.numBlocks > 0) {
            // for each object detected, process it
            for (int i = 1; i < pixy.ccc.numBlocks; i++) {
                // if the object is the left reference, store its position
                if (pixy.ccc.blocks[i].m_signature == reference_left) {
                    // store the position of the left reference (x, y, id)
                    int x2 = pixy.ccc.blocks[i].m_x;
                    int y2 = pixy.ccc.blocks[i].m_y;
                    int id = pixy.ccc.blocks[i].m_signature;

                    // create a new Signature object
                    Signature coord(x2, y2, id);

                    // append the object to the list of coordinates
                    coords->append(coord, -1, true);

                // if the object is the right reference, store its position
                } else if (pixy.ccc.blocks[i].m_signature == reference_right) {
                    // store the position of the right reference (x, y, id)
                    int x2 = pixy.ccc.blocks[i].m_x;
                    int y2 = pixy.ccc.blocks[i].m_y;
                    int id = pixy.ccc.blocks[i].m_signature;

                    // create a new Signature object
                    Signature coord(x2, y2, id);

                    // append the object to the list of coordinates
                    coords->append(coord, -1, false, true);

                // if the object is not a reference, store its position
                } else {
                    // store the position of the object (x, y, id)
                    int x = pixy.ccc.blocks[i].m_x;
                    int y = pixy.ccc.blocks[i].m_y;
                    int signature = pixy.ccc.blocks[i].m_signature;

                    // create a new Signature object
                    Signature coord(x, y, signature);

                    // append the object to the list of coordinates
                    coords->append(coord);
                }
            }

            // send the coordinates to the serial port
            for (int i = 0; i < coords->size; i++) {
                Serial.print("Object ");
                Serial.print(i);
                Serial.print(": ");
                Serial.println(coords->get_str(i));
            }

            // if no objects are detected, print a message
            if (coords->size == 0) {
                Serial.println("No objects found");
            }
        }
        // delete the signatures stored to avoid memory leaks, then delete the coords object and create a new one
        coords->clear();
        delete coords;
        coords = new Coordinates();
        // wait for 100ms
        delay(100);
    }
}
