#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "mtree.h"

#define B 128 // Tamaño de B calculado

// Structure of a point
struct point {
    double x, y;
};

// Structure of an entry
struct entry {
    Point p; // point
    double cr; // cover radio
    Node *a; // disk address to the child node
};

struct node {
    Entry entries[B];
    int size;
};

// Structure of a query to search points in a Mtree
struct query {
    Point q;
    double r;
};

// Function that calculates the Euclidean distance between two points
double euclidean_distance(Point p1, Point p2) {
    return sqrt(pow(p2.x - p1.x, 2) + pow(p2.y - p1.y, 2));
}

// Function that tells whether a node is a leaf or not.
// Hay que revisar bien la condicion de cuándo un nodo es hoja, quizas la condicion podría relajarse.
int is_leaf(Node* node) {
    int num_entries = sizeof(node) / sizeof(Entry); // number of entries in the node
    Entry* entries = node->entries; // node Entry array
    // For each entry, if any has 'cr' or 'a' non-nulls, so the node is not a leaf
    for (int i=0; i < num_entries; i++) {
        if (entries[i].cr != 0.0 || entries[i].a != NULL)
            return 0;
    }
    return 1;
}

void covering_radius(Node *node) {

    if (is_leaf(node)) {
        // If the node is a leaf, set cr to NULL for simplicity
        for (int i = 0; i < node->size; i++) {
            node->entries[i].cr = 0.0;
        }
    } else {
        // If the node is not a leaf, calculate the covering radius for each entry
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
// hay que ver bien si es correcto que parta con ese tamano o parte de tamano 0 y aumenta al agregarle entradas.
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

int main() {

    // Para compilar: gcc mtree.c -o mtree -lm
    // Para ejecutar: ./mtree

    // Determinar tamano de B
    printf("tamano de entrada: %i\n", sizeof(Entry));
    printf("tamano de B sería: %d\n", 4096 / sizeof(Entry));

    Point *P = malloc(25);

    // Seed the random number generator
    srand(time(NULL));

    // Create a root node for the M-tree
    Node* root = create_node();

    // Create some points for testing
    Point p1 = {random_double(), random_double()};
    Point p2 = {random_double(), random_double()};
    Point p3 = {random_double(), random_double()};
    Point p4 = {random_double(), random_double()};

    // Create a query
    Query query;
    query.q = p1;  // Query point
    query.r = 1.5; // Query radius

    // Search points within the specified radius
    Point* result = search_points_in_radio(root, query);

    // Print the points found
    printf("puntos en el radio:\n");
    for (int i = 0; result[i].x != 0 || result[i].y != 0; ++i) {
        printf("Point %d: (%.2f, %.2f)\n", i + 1, result[i].x, result[i].y);
    }

    // Free the memory allocated for the result array
    free(result);

    return 0;
}
