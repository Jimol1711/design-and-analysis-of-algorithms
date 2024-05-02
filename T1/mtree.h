#ifndef MTREE_H
#define MTREE_H

#include <math.h>

#define B 128 // Tamaño de B calculado
#define b 64 // b = 0.5*B

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
    Entry entries[B];
    int num_entries;
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

// Structure that represents a sample subset (F_j)
// Esta estructura se llama igual que su campo que contiene un arreglo. Esto puede traer confusiones y podria escogerse nombres mas apropiados
typedef struct {
    Point point;
    Point* sample_subset;
    int subset_size;
} SampleSubset;

// Function that calculates the Euclidean distance between two points
double euclidian_distance(Point p1, Point p2) {
    return sqrt(pow(p2.x - p1.x, 2) + pow(p2.y - p1.y, 2));
}

#endif // MTREE_H
