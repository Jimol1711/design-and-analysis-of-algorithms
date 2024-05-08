#ifndef MTREE_H
#define MTREE_H

#include <stdio.h>
#include <stdlib.h>
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
} SubsetStructure;

// Function that calculates the Euclidean distance between two points
double euclidian_distance(Point p1, Point p2) {
    return sqrt(pow(p2.x - p1.x, 2) + pow(p2.y - p1.y, 2));
}

// Function to create a new node
Node* create_node() {
    Node* node = (Node*)malloc(sizeof(Node));
    if (node) {
        node->num_entries = 0;
    }
    return node;
}

// Function that tells whether a node is a leaf or not.
// Hay que revisar bien la condicion de cuándo un nodo es hoja, quizas la condicion podría relajarse.
int is_leaf(Node* node) {
    int num_entries = node->num_entries; // number of entries in the node
    Entry* entries = node->entries; // node Entry array
    // For each entry, if any has 'cr' or 'a' non-nulls, so the node is not a leaf
    for (int i=0; i < num_entries; i++) {
        if (entries[i].cr != 0.0 || entries[i].a != NULL)
            return 0;
    }
    return 1;
}

#endif // MTREE_H
