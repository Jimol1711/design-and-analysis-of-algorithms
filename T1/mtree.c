#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <limits.h>
#include <float.h>

#define B 128
#define b 64

typedef struct node Node;
typedef struct entry Entry;
typedef struct point Point;
typedef struct query Query;

// Estructura que representa un punto
struct point {
    double x, y;
};

// Estructura que representa una entrada
struct entry {
    Point p;
    double cr;
    Node *a;
};


// Estructura que representa un nodo
struct node {
    Entry *entries;
    int num_entries;
};

// Estructura que representa una consulta
struct query {
    Point q;
    double r;
};

// Función que calcula la distancia euclidiana entre p1 y p2
double euclidean_distance(Point p1, Point p2) {
    return sqrt(pow(p2.x - p1.x, 2) + pow(p2.y - p1.y, 2));
}

// Función que encuentra el mínimo entre dos ints
int intMin(int i, int j) {
    return i < j ? i : j;
}

// Función que determina si un nodo es hoja o no
int is_leaf(Node* node) {
    int num_entries = sizeof(node) / sizeof(Entry);
    Entry* entries = node->entries;
    for (int i=0; i < num_entries; i++) {
        if (entries[i].cr != 0.0 || entries[i].a != NULL)
            return 0;
    }
    return 1;
}

// Función que crea un nodo
Node* create_node() {
    Node* node = (Node*)malloc(sizeof(Node));
    node->num_entries = 0;
    return node;
}

// Función que realiza la query Q en el árbol node, guardando los puntos en sol_array y calculando los accesos a disco en la dirección disk_accesses
void range_search(Node* node, Query Q, Point** sol_array, int* array_size, int* disk_accesses) {
    Point q = Q.q; 
    double r = Q.r;
    int num_entries = sizeof(node) / sizeof(Entry); // number of entries in the node
    Entry* entries = node->entries; // node Entry array

    // If the node is a leaf, search each entry that satisfy the condition of distance.
    // if the entry satisfies it, then increase the sol_array length and add the point to sol_array
    if (is_leaf(node)) {
        (*disk_accesses)++;
        for (int i=0; i<num_entries; i++) {
            Point p = entries[i].p;
            if(euclidian_distance(p, q) <= r) {
                *sol_array = (Point*)realloc(*sol_array, (*array_size) + 1 * sizeof(Point));
                (*sol_array)[*array_size] = p;
                (*array_size)++;
            }
        }
    }
    // if is not a leaf, for each entry, if satisfy this distance condition go down to check its child node 'a'
    else {
        (*disk_accesses)++;
        for (int i=0; i<num_entries; i++) {
            if(euclidean_distance(entries[i].p, q) <= entries[i].cr + r){
                range_search(entries[i].a, Q, sol_array, array_size, disk_accesses);
            }
        }
    }
}

// Función que busca los puntos en la query Q del árbol node y guarda accesos a disco en la dirección disk_accesses
Point* search_points_in_radio(Node* node, Query Q, int* disk_accesses) {
    Point* sol_array = NULL;
    int array_size = 0;

    range_search(node, Q, &sol_array, &array_size, disk_accesses);
    return sol_array;   
}
