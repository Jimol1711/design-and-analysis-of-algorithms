#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define B 128 // Tamaño de B calculado

// Forward declaration of Node
typedef struct Node Node;

// Structure of a point
typedef struct {
    double x, y;
} Point;

// Structure of an entry
typedef struct {
    Point p; // point
    double cr; // cover radio
    Node *a; // disk address to the child node
} Entry;

struct Node {
    Entry entrys[B];
};

// Structure of a query to search points in a Mtree
typedef struct {
    Point q;
    double r;
} Query;

// Probablemente para mas adelante...
typedef struct {
    ;
} Mtree;

// Function that calculates the Euclidean distance between two points
double euclidian_distance(Point p1, Point p2) {
    return sqrt(pow(p2.x - p1.x, 2) + pow(p2.y - p1.y, 2));
}

// Function that creates a node
// hay que ver bien si es correcto que parta con ese tamano o parte de tamano 0 y aumenta al agregarle entradas.
Node* create_node() {
    Node* node = (Node*)malloc(sizeof(Node));
    return node;
}


// Function that tells whether a node is a leaf or not.
// Hay que revisar bien la condicion de cuándo un nodo es hoja, quizas la condicion podría relajarse.
int is_leaf(Node* node) {
    int num_entrys = sizeof(node) / sizeof(Entry); // number of entries in the node
    Entry* entrys = node->entrys; // node Entry array
    // For each entry, if any has 'cr' or 'a' non-nulls, so the node is not a leaf
    for (int i=0; i < num_entrys; i++) {
        if (entrys[i].cr != 0.0 || entrys[i].a != NULL)
            return 0;
    }
    return 1;
}

// Auxiliary function that adds the solutions (points) that satisfy the condition in a node entry
void range_search(Node* node, Query Q, Point** sol_array, int* array_size, int* disk_accesses) {
    Point q = Q.q; // query point
    double r = Q.r; // query radio
    int num_entrys = sizeof(node) / sizeof(Entry); // number of entries in the node
    Entry* entrys = node->entrys; // node Entry array

    // If the node is a leaf, search each entry that satisfy the condition of distance.
    // if the entry satisfies it, then increase the sol_array length and add the point to sol_array
    if (is_leaf(node)) {
        for (int i=0; i<num_entrys; i++) {
            if(euclidian_distance(entrys[i].p, q) <= r) {
                *sol_array = (Point*)realloc(*sol_array, (*array_size) + 1 * sizeof(Point));
                (*sol_array)[*array_size] = q;
                (*array_size)++;
            }
        }
    }
    // if is not a leaf, for each entry, if satisfy this distance condition go down to check its child node 'a'
    else {
        for (int i=0; i<num_entrys; i++) {
            if(euclidian_distance(entrys[i].p, q) <= entrys[i].cr + r){
                (*disk_accesses)++;
                range_search(entrys[i].a, Q, sol_array, array_size, disk_accesses);
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

void main() {
    // Para compilar: gcc mtree.c -o mtree -lm
    // Para ejecutar: ./mtree

    // Determinar tamano de B
    printf("tamano de entrada: %i\n", sizeof(Entry));
    printf("tamano de B sería: %d\n", 4096 / sizeof(Entry));

}