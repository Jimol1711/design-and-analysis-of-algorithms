#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define B 100

// Estructura de un punto
typedef struct {
    double x, y;
} Punto;

// Estructura de una entrada (entry)
typedef struct {
    Punto p; // punto
    double cr; // radio cobertor
    Node *a; // dir. en disco a la pagina de su hijo
} Entry;

// Estructura de un nodo.
typedef struct {
    int is_leaf;
    int num_entrys;
    Entry entrys[B];
} Node;

// Estructura de una Query para busqueda de entrys en un nodo.
typedef struct {
    Punto q;
    double r;
} Query;

// Estructura de un Mtree.
typedef struct {
    ;
} Mtree;

// Función que crea un nodo 
Node* create_node() {
    Node* node = (Node*)malloc(sizeof(Node));
    return node;
}

Punto* search_entrys_in_node(Node* node, Query Q) {
    Punto q = Q.q;
    double r = Q.r;
    int coincidences = 0;
    if (node -> is_leaf) {
        for (int i=0; i < node->num_entrys; i++) {
            Punto p = node->entrys[i].p;
            if (euclidian_distance(p,q) <= r)
                coincidences++;
        }
    }
    else {
        for (int i=0; i < node->num_entrys; i++) {
            Entry entry = node->entrys[i];
            Punto p = entry.p;
            double cr = entry.cr;
            if (euclidian_distance(p,q) <= cr + r)
                search_entrys_in_node(entry.a, Q);
        }
    }

    Punto* response = (Punto*)malloc(coincidences * sizeof(Punto));

    if (node -> is_leaf) {
        for (int i=0; i < node->num_entrys; i++) {
            Punto p = node->entrys[i].p;
            if (euclidian_distance(p,q) <= r)
                response[i] = p;
        }
    }
}

// Función que calcula la distancia euclidiana entre dos puntos
double euclidian_distance(Punto p1, Punto p2) {
    return sqrt(pow(p2.x - p1.x, 2) + pow(p2.y - p1.y, 2));
}

void main() {
    printf("%s\n", "hola");
}