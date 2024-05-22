#include <coordinates.hpp>
#include <puppetMover.hpp>

Coordinates* coords;

void setup() {
    Serial.begin(19200);
    Serial.println("Starting...");
    coords->init();
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
        coords->init();
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
        while (coords->pixy.ccc.getBlocks() == 0 && attempts < 3) {
            delay(100);
            attempts++;
        }

        if (attempts >= 3) {
            Serial.println("No objects detected, reinitializing...");
            coords->pixy.init();
        }

        if (coords->pixy.ccc.numBlocks > 0) {
            int x1 = coords->pixy.ccc.blocks[0].m_x;
            bool reference_left_found = false;

            for (int i = 1; i < coords->pixy.ccc.numBlocks; i++) {
                if (coords->pixy.ccc.blocks[i].m_signature == reference_left) {
                    int x2 = coords->pixy.ccc.blocks[i].m_x;
                    int y2 = coords->pixy.ccc.blocks[i].m_y;
                    int id = coords->pixy.ccc.blocks[i].m_signature;
                    int width2 = coords->pixy.ccc.blocks[i].m_width;
                    int distance = x2 - x1;
                    Signature coord(x2, y2, id);
                    coords->append(coord, -1, true);
                    reference_left_found = true;
                } else if (coords->pixy.ccc.blocks[i].m_signature == reference_right) {
                    if (reference_left_found) {
                        int x2 = coords->pixy.ccc.blocks[i].m_x;
                        int y2 = coords->pixy.ccc.blocks[i].m_y;
                        int width2 = coords->pixy.ccc.blocks[i].m_width;
                        int distance = x2 - x1;
                        int coord[3] = {x2, y2, width2};
                        coords->append(coord, -1, false, true);
                    }
                } else {
                    int x = coords->pixy.ccc.blocks[i].m_x;
                    int y = coords->pixy.ccc.blocks[i].m_y;
                    int signature = coords->pixy.ccc.blocks[i].m_signature;
                    int coord[3] = {x, y, signature};
                    coords->append(coord);
                }
            }

            for (int i = 0; i < coords->size; i++) {
                Serial.print("Object ");
                Serial.print(i);
                Serial.print(": ");
                Serial.println(coords->get_str(i));
            }

            if (coords->size == 0) {
                Serial.println("No objects found");
            }
        }
        delay(100);
    }
}
