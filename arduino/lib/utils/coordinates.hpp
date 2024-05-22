#ifndef COORDINATES_H
#define COORDINATES_H

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

const int reference_left = 0;
const int reference_right = 1;


class Signature {

    // Attributs publiques
    public:
        int x;
        int y;
        int id;

        // Constructeur depuis des coordonnées x, y et l'id de signature
        Signature(int x, int y, int id) {
            this->x = x;
            this->y = y;
            this->id = id;
        }

        // Construction depuis un tableau de coordonnées int[3]
        Signature(int coord[3]) {
            this->x = coord[0];
            this->y = coord[1];
            this->id = coord[2];
        }
};

class Coordinates {
    public:
        int size = 0;
        // Tableau de pointeurs vers les coordonnées
        Signature** coords = nullptr;
        bool left_ref_available = false;
        bool right_ref_available = false;
        Pixy2 pixy;


    private:
        bool left_ref_found = false;
        bool idle = false;

    public:
        // Méthode pour ajouter des coordonnées
        void append(Signature coord, int index = -1, bool left_reference = false, bool right_reference = false) {
            if (left_reference) {
                this->append(coord, 0);
                left_ref_found = true;
                left_ref_available = true;
            } else if (right_reference) {
                if (left_ref_found) {
                    this->append(coord, 1);
                    right_ref_available = true;
                } else {
                    this->append(coord, 0);
                }
            } else if (index == -1) {
                 // Allouer un nouvel objet Signature
                Signature* coordPtr = new Signature(coord);
                
                // Déclarer un nouveau tableau de pointeurs de coordonnées de taille + 1
                Signature** newCoords = new Signature*[size + 1];

                // Copier les coordonnées existantes dans le nouveau tableau
                for (int i = 0; i < size; ++i) {
                    newCoords[i] = coords[i];
                }

                // Ajouter les nouvelles coordonnées à la fin du tableau
                newCoords[size] = coordPtr;

                // Supprimer les anciennes coordonnées
                deleteCoords();

                // Mettre à jour les coordonnées
                coords = newCoords;

                // Mettre à jour la taille
                ++size;

            } else {
                // Allouer un nouvel objet Signature
                Signature* coordPtr = new Signature(coord);

                // Déclarer un nouveau tableau de pointeurs de coordonnées de taille + 1
                Signature** newCoords = new Signature*[size + 1];

                // Copier les coordonnées existantes dans le nouveau tableau
                // jusqu'à l'index spécifié
                for (int i = 0; i < index; ++i) {
                    newCoords[i] = coords[i];
                }

                // Ajouter la nouvelle coordonnée à l'index spécifié
                newCoords[index] = coordPtr;
                
                // Terminer de copier les coordonnées existantes dans le nouveau tableau
                for (int i = index; i < size; ++i) {
                    newCoords[i + 1] = coords[i];
                }

                // Supprimer les anciennes coordonnées
                deleteCoords();

                // Mettre à jour les coordonnées
                coords = newCoords;

                // Mettre à jour la taille
                ++size;
            }
        }

        // obtenir l'état de l'acquisition des coordonnées
        bool is_idle() {
            return idle;
        }

        // Changer l'état de l'acquisition des coordonnées
        void set_idle(bool state) {
            idle = state;
        }

        // acquérir les coordonnées depuis la pixy
        void acquire_signatures() {
            return;
        }

        // Méthode pour supprimer des coordonnées à l'index spécifié
        void remove(int index) {
            // supprimer la coordonnée dont la référence correspond à l'index spécifié et réduire la taille du tableau
            if (index >= 0 && index < size) {
                Signature** newCoords = new Signature*[size - 1];
                for (int i = 0; i < size; ++i) {
                    if (i < index) {
                        newCoords[i] = coords[i];
                    } else if (i > index) {
                        newCoords[i - 1] = coords[i];
                    }
                }
                delete coords[index];
                delete[] coords;
                coords = newCoords;
                --size;
            }
        }

        // (re)initialiser la pixy
        void init() {
            pixy.init();
        }

        // surcharge de la méthode remove pour supprimer les coordonnées par signature
        void remove(Signature signature) {
            for (int i = 0; i < size; ++i) {
                if (coords[i]->id == signature.id) {
                    remove(i);
                    break;
                }
            }
        }

        // Méthode pour obtenir les coordonnées à l'index spécifié
        Signature* get(int index) {
            if (index >= 0 && index < size) {
                return coords[index];
            } else {
                return nullptr;
            }
        }

        Signature* get_by_id(int signature) {
            for (int i = 0; i < size; ++i) {
                if (coords[i]->id == signature) {
                    return get(i);
                }
            }
            return nullptr;
        }


        // Méthode pour obtenir les coordonnées à l'index spécifié sous forme de chaîne de caractères
        String get_str(int index) {
            Signature* coord = get(index);
            if (coord != nullptr) {
                return "[" + (String)coord->x + ", " + (String)coord->y + ", " + (String)coord->id + "]";
            } else {
                return "null";
            }
        }

        // Méthode pour vider les coordonnées
        void clear() {
            // supprime les coordonnées de la mémoire, supprime le tableau de références et réinitialise la taille
            for (int i = 0; i < size; ++i) {
                delete coords[i];
            }
            delete[] coords;
            size = 0;
        }

    private:
        // Méthode pour supprimer toutes les coordonnées
        void deleteCoords(bool flushValues = false) {
            if (coords != nullptr) {
                if (flushValues) {
                    for (int i = 0; i < size; ++i) {
                        delete coords[i];
                    }
                }
                delete[] coords;
                coords = nullptr;
            }
            left_ref_available = false;
            right_ref_available = false;
        }
};

#endif

