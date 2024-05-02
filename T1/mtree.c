#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "mtree.h"

#define B 128

// Structure of a point
struct point {
    double x, y;
};

// Structure of an entry
struct entry {
    Point p; // point
    double cr; // covering radius
    Node *a; // disk address to the root of the covering tree
};

// Structure of a Node
struct node {
    Entry entries[B];
    int size;
};

// Structure of a query to search points in an Mtree
struct query {
    Point q;
    double r;
};

// Function that calculates the Euclidean distance between two points p1 and p2
double euclidean_distance(Point p1, Point p2) {
    return sqrt(pow(p2.x - p1.x, 2) + pow(p2.y - p1.y, 2));
}

// Function that tells whether a node is a leaf or not.
int is_leaf(Node* node) {
    int num_entries = sizeof(node) / sizeof(Entry);
    Entry* entries = node->entries;
    for (int i=0; i < num_entries; i++) {
        if (entries[i].cr != 0.0 || entries[i].a != NULL)
            return 0;
    }
    return 1;
}

void covering_radius(Node *node) {
    if (is_leaf(node)) {
        for (int i = 0; i < node->size; i++) {
            node->entries[i].cr = 0.0;
        }
    } else {
        for (int i = 0; i < node->size; i++) {
            double max_distance = 0.0;
            for (int j = 0; j < node->size; j++) {
                double distance = euclidean_distance(node->entries[i].p, node->entries[j].p);
                if (distance > max_distance) {
                    max_distance = distance;
                }
            }
            node->entries[i].cr = max_distance;
        }
    }
}

// Function that creates a node
Node* create_node() {
    Node* node = (Node*)malloc(B * sizeof(Entry));
    covering_radius(node);
    return node;
}

// Auxiliary function that adds the solutions (points) that satisfy the condition in a node entry
void range_search(Node* node, Query Q, Point** sol_array, int* array_size, int* disk_accesses) {
    Point q = Q.q; // query point
    double r = Q.r; // query radio
    int num_entries = sizeof(node) / sizeof(Entry); // number of entries in the node
    Entry* entries = node->entries; // node Entry array

    // If the node is a leaf, search each entry that satisfy the condition of distance.
    // if the entry satisfies it, then increase the sol_array length and add the point to sol_array
    if (is_leaf(node)) {
        for (int i=0; i<num_entries; i++) {
            if(euclidean_distance(entries[i].p, q) <= r) {
                *sol_array = (Point*)realloc(*sol_array, (*array_size) + 1 * sizeof(Point));
                (*sol_array)[*array_size] = q;
                (*array_size)++;
            }
        }
    }
    // if is not a leaf, for each entry, if satisfy this distance condition go down to check its child node 'a'
    else {
        for (int i=0; i<num_entries; i++) {
            if(euclidean_distance(entries[i].p, q) <= entries[i].cr + r){
                (*disk_accesses)++;
                range_search(entries[i].a, Q, sol_array, array_size, disk_accesses);
            }
        }
    }
}

// Function that search the points that lives inside the ball specified in the query
Point* search_points_in_radio(Node* node, Query Q) {
    Point* sol_array = NULL; // Initialize the solutions array as null
    int array_size = 0; // the array_size of sol_array starts in zero (doesnt have solutions initially)
    int disk_accesses = 1; // Disk accesses count. Starts in 1 because we start to search in the node inmediately

    range_search(node, Q, &sol_array, &array_size, &disk_accesses); // We occupy the range_search auxiliar function
    return sol_array; // return the array solutions with the points found
    
}

// Function that returns a random double value between 0 and 1
double random_double() {
    return (double)rand() / RAND_MAX;
}

// Function to generate a set of 2^10 random points in C
void generate_points(struct point P[], int totalPoints) {
    for (int i = 0; i < totalPoints; i++) {
        P[i].x = random_double();
        P[i].y = random_double();
    }
}

int main() {

    // Para compilar: gcc mtree.c -o mtree -lm
    // Para ejecutar: ./mtree

    // Test para la búsqueda (crear árbol y query, ver que devuelva los puntos en la query)

    // Crear set de puntos P para cada n entre 2**10 y 2**25 y set de consultas Q con 100 puntos

    // Test de cp (contar accesos a disco en árbol construido con cp para n puntos, con n
    // entre 2**10 y 2**25)

    // Test de ss (contar accesos a disco en árbol construido con ss para n puntos, con n
    // entre 2**10 y 2**25)

    // Determinar tamano de B
    printf("tamano de entrada: %i\n", sizeof(Entry));
    printf("tamano de B sería: %d\n", 4096 / sizeof(Entry));

    // Seed the random number generator
    srand(time(NULL));

    for (int i = 10; i < 25; i++) {
        int totalPoints = pow(2, i);
        struct point P[totalPoints];
        generate_points(P, totalPoints);
    }

    return 0;
}
