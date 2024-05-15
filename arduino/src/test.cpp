#include <stdio.h>
#include "../lib/utils/coordinates.hpp"

// test de la classe Coordinates
Coordinates coords;

int main() {
    // On crée des coordonnées
    Signature s1(10, 20, 0);
    Signature s2(30, 40, 1);
    Signature s3(50, 60, 2);

    // On les ajoute à la liste
    coords.append(s1);
    coords.append(s2);
    coords.append(s3);

    // On affiche les coordonnées
    for (int i = 0; i < coords.size; ++i) {
        Signature* c = coords.get(i);
        printf("Object %d: x=%d, y=%d, id=%d\n", i, c->x, c->y, c->id);
    }

    Signature* c = coords.get_by_id(1);
    printf("Object with id 1: x=%d, y=%d, id=%d\n", c->x, c->y, c->id);

    // afficher la taille de la liste
    printf("Size: %d\n", coords.size);

    // ajouter à l'index 1
    Signature s4(70, 80, 3);
    coords.append(s4, 1);

    // On affiche les coordonnées
    for (int i = 0; i < coords.size; ++i) {
        Signature* c = coords.get(i);
        printf("Object %d: x=%d, y=%d, id=%d\n", i, c->x, c->y, c->id);
    }

    // On supprime les coordonnées à l'index 1
    coords.remove(1);

    // On supprime s3 (id = 2) de la liste
    coords.remove(s3);

    // On affiche les coordonnées
    for (int i = 0; i < coords.size; ++i) {
        Signature* c = coords.get(i);
        printf("Object %d: x=%d, y=%d, id=%d\n", i, c->x, c->y, c->id);
    }

    // effacer les coordonnées
    coords.clear();

    // afficher la taille de la liste
    printf("Size: %d\n", coords.size);

    


    return 0;
}