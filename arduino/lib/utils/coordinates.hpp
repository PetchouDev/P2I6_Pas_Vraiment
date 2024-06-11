#ifndef COORDINATES_H // si la librairie n'est pas incluse
#define COORDINATES_H // inclure la librairie

#if defined(__INTELLISENSE__) // si l'IDE est en mode Intellisense (imports pris en compte par l'aide à la saisie)
#include <Arduino.h> // importer la librairie Arduino pour la classe String
#endif

// Classe pour stocker les coordonnées des objets détectés
class Signature {

    // Attributs publiques
    public:
        int x;    // abscisse
        int y;   // ordonnée
        int id; // id de la signature

        // Constructeur depuis des coordonnées x, y et l'id de signature
        Signature(int x, int y, int id) {
            this->x = x;
            this->y = y;
            this->id = id;
        }

        // Construction depuis un tableau de coordonnées int[3] (utilisé pendant le développement, plus forcément nécessaire)
        Signature(int coord[3]) {
            this->x = coord[0];
            this->y = coord[1];
            this->id = coord[2];
        }
};

// Classe pour les stocker les signatures
class Coordinates {
    public:
        // Taille du tableau de coordonnées
        int size = 0;

        // Tableau de pointeurs vers les coordonnées
        Signature** coords = nullptr;

        // booléens pour indiquer si les références sont disponibles
        bool left_ref_available = false;
        bool right_ref_available = false;

    private:
        // booléen pour indiquer si les références ont été trouvées (influe sur l'ajout des coordonnées)
        bool left_ref_found = false;

    public:
        // Méthode pour ajouter des coordonnées
        void append(Signature coord, int index = -1) {

            // Si l'index est -1, ajouter les coordonnées à la fin du tableau
            if (index == -1) {
                // Déclarer un nouveau pointeur vers  l'objet Signature
                Signature* coordPtr = &coord;
                
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

        // Méthode pour supprimer des coordonnées à l'index spécifié
        void remove(int index) {

            // supprimer la coordonnée dont la référence correspond à l'index spécifié et réduire la taille du tableau
            if (index >= 0 && index < size) {

                // Déclarer un nouveau tableau de pointeurs de coordonnées de taille - 1
                Signature** newCoords = new Signature*[size - 1];

                // Copier les coordonnées existantes dans le nouveau tableau
                for (int i = 0; i < size; ++i) {
                    if (i < index) {
                        newCoords[i] = coords[i];
                    } else if (i > index) {
                        newCoords[i - 1] = coords[i];
                    }
                }

                // Supprimer les anciennes coordonnées
                delete coords[index];
                delete[] coords;

                // Mettre à jour les coordonnées
                coords = newCoords;

                // Mettre à jour la taille
                --size;
            }
        }

        // surcharge de la méthode remove pour supprimer les coordonnées par signature
        void remove(Signature signature) {

            // parcourir les coordonnées
            for (int i = 0; i < size; ++i) {

                // si l'id de la coordonnée correspond à l'id de la signature, supprimer la coordonnée
                if (coords[i]->id == signature.id) {
                    remove(i);
                    break;
                }
            }
        }

        // Méthode pour obtenir les coordonnées à l'index spécifié
        Signature* get(int index) {

            // retourner les coordonnées à l'index spécifié
            if (index >= 0 && index < size) {
                return coords[index];
            } else {
                return nullptr;
            }
        }

        Signature* get_by_id(int signature) {

            // parcourir les coordonnées
            for (int i = 0; i < size; ++i) {

                // si l'id de la coordonnée correspond à l'id de la signature, retourner les coordonnées
                if (coords[i]->id == signature) {
                    return get(i);
                }
            }
            return nullptr;
        }


        // Méthode pour obtenir les coordonnées à l'index spécifié sous forme de chaîne de caractères
        String get_str(int index) {

            // obtenir l'objet Signature à l'index spécifié
            Signature* coord = get(index);

            // retourner les coordonnées sous forme de chaîne de caractères
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

            // supprimer les coordonnées de la mémoire
            if (coords != nullptr) {
                if (flushValues) {
                    for (int i = 0; i < size; ++i) {
                        delete coords[i];
                    }
                }
                delete[] coords;
                coords = nullptr;
            }

            // réinitialiser la taille et les booléens de référence
            size = 0;
        }
};

#endif

