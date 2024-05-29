#ifndef COORDINATES_H
#define COORDINATES_H 

// numéro de signature pour les références
const int reference_left = 0;
const int reference_right = 1;

// Classe pour les coordonnées de signature
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
        void append(Signature coord, int index = -1, bool left_reference = false, bool right_reference = false) {

            // si une référence est trouvée, elle est prioritaire sur l'index spécifié
            // référence gauche trouvée
            if (left_reference) {

                // si la référence gauche est trouvée, ajouter les coordonnées au début du tableau
                this->append(coord, 0);

                // indiquer que la référence gauche est disponible
                left_ref_found = true;
                left_ref_available = true;

            // référence droite trouvée
            } else if (right_reference) {

                // si la référence droite est trouvée, l'ajouter juste après la référence gauche si elle a été trouvée
                if (left_ref_found) {

                    // ajouter les coordonnées à l'index 1 (2ème position, soit après la référence gauche)
                    this->append(coord, 1);

                    // indiquer que la référence droite est disponible
                    right_ref_available = true;

                // si la référence gauche n'a pas été trouvée, ajouter les coordonnées au début du tableau
                } else {
                    this->append(coord, 0);
                }

            // si l'objet n'est pas une référence et l'index est -1 (défault), ajouter les coordonnées à la fin du tableau
            } else if (index == -1) {
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

        // acquérir les coordonnées depuis la pixy
        void acquire_signatures() {
            return;
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
            left_ref_available = false;
            right_ref_available = false;
        }
};

#endif

