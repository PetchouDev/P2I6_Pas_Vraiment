// imports pour la Pixy
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

// librairies du projet (coordinates pour stocker les signatures, puppetUtils pour asservir les moteurs et piloter les servos)
#include <coordinates.hpp>
# include <puppetMover.hpp>
#include <puppetUtils.hpp>

// déclaration des variables
Coordinates* coords; // stocker les coordonnées des objets détectés
Pixy2 pixy;         // déclaration de la caméra Pixy
bool idle = false; // état de l'acquisition des coordonnées
bool initialized = false; // état de l'initialisation des références


// Au démarrage, on initialise la communication série et la caméra Pixy
void setup() {
    Serial.begin(19200);
    pixy.init();
}

void loop() {
    // Lecture des commandes depuis le port série
    String c = ""; // déclaration d'une chaine de caractères pour stocker les commandes

    // Si des données sont disponibles sur le port série
    if (Serial.available() > 0) {
        // Lire les données
        c = Serial.readString();
        // Supprimer les espaces et les retours à la ligne
        c.trim();
        // Afficher les commandes reçues
        Serial.println(">>> " + c);
    }

    if (c == "reload") {
        // reinitialize la pixy
        pixy.init();
    } else if (c == "stop") {
        // activer le mode idle (mise en pause de l'acquisition des coordonnées)
        idle = true;
    } else if (c == "start") {
        // désactiver le mode idle (reprise de l'acquisition des coordonnées)
        idle = false;
    } else if (c == "state") {
        // afficher l'état de l'acquisition des coordonnées
        Serial.println("Idle: " + (String)idle);
    }

    // Si le mode idle est désactivé, on acquiert les coordonnées des objets détectés
    if (!idle) {

        // créer une instance de la classe Coordinates pour stocker les coordonnées des objets détectés et l'assigner au pointeur coords
        coords = new Coordinates();

        if (!initialized) {
            // récupérer les coordonnées des repères de référence
            get_initial_references(*coords);
            initialized = true;
        }

        // nombre de tentatives pour détecter des objets
        int attempts = 0;

        // tant qu'aucun objet n'est détecté et que le nombre de tentatives est inférieur à 3, on réessaie de détecter des objets
        while (pixy.ccc.getBlocks() == 0 && attempts < 3) {
            // attendre 100ms avant de réessayer
            delay(100);

            // incrémenter le nombre de tentatives
            attempts++;
        }

        // si le nombre de tentatives est supérieur ou égal à 3, réinitialiser la caméra Pixy
        if (attempts >= 3) {
            Serial.println("No objects detected, reinitializing...");
            pixy.init();
        }

        // si des objets sont détectés, les traiter
        if (pixy.ccc.numBlocks > 0) {

            // parcourir les objets détectés
            for (int i = 1; i < pixy.ccc.numBlocks; i++) {

                // récupérer les coordonnées de l'objet
                int x = pixy.ccc.blocks[i].m_x;
                int y = pixy.ccc.blocks[i].m_y;
                int id = pixy.ccc.blocks[i].m_signature;

                // créer une instance de la classe Signature pour stocker les coordonnées de l'objet
                Signature coord(x, y, id);

                // ajouter les coordonnées de l'objet à l'instance de la classe Coordinates et indiquer qu'il ne s'agit pas d'une référence
                coords->append(coord, -1, id == reference_left, id == reference_right); // si une des références est trouvée, elle est prioritaire sur l'index spécifiée
            }

            // afficher les coordonnées des objets détectés
            for (int i = 0; i < coords->size; i++) {
                Serial.print("Object ");
                Serial.print(i);
                Serial.print(": ");
                Serial.println(coords->get_str(i));
            }

            // si aucun objet n'est détecté, afficher un message
            if (coords->size == 0) {
                Serial.println("No objects found");
            }

            // traiter les coordonnées des objets détectés
            process_coords(*coords);
        }
        // supprimer les signatures de la mémoire pour éviter les fuites de mémoire
        coords->clear();

        // supprimer la référence de l'objet coords (à nouveau pour éviter les fuites de mémoire)
        delete coords;

        // attendre 100ms avant de réessayer de détecter des objets
        delay(100);
    }
}
