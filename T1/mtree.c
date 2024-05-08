#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define B 128
#define b 64

typedef struct node Node;
typedef struct entry Entry;
typedef struct point Point;
typedef struct query Query;

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
    Entry *entries;
    int num_entries;
};

// Structure of a query to search points in an Mtree
struct query {
    Point q;
    double r;
};

// Structure that represents a sample subset (F_j)
// Esta estructura se llama igual que su campo que contiene un arreglo. Esto puede traer confusiones y podria escogerse nombres mas apropiados
typedef struct {
    Point point;
    Point* sample_subset;
    int subset_size;
} SubsetStructure;

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
        for (int i = 0; i < node->num_entries; i++) {
            node->entries[i].cr = 0.0;
        }
    } else {
        for (int i = 0; i < node->num_entries; i++) {
            double max_distance = 0.0;
            for (int j = 0; j < node->num_entries; j++) {
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
        (*disk_accesses)++;
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
        (*disk_accesses)++;
        for (int i=0; i<num_entries; i++) {
            if(euclidean_distance(entries[i].p, q) <= entries[i].cr + r){
                range_search(entries[i].a, Q, sol_array, array_size, disk_accesses);
            }
        }
    }
}

// Function that search the points that lives inside the ball specified in the query
Point* search_points_in_radio(Node* node, Query Q, int* disk_accesses) {
    Point* sol_array = NULL; // Initialize the solutions array as null
    int array_size = 0; // the array_size of sol_array starts in zero (doesnt have solutions initially)

    range_search(node, Q, &sol_array, &array_size, disk_accesses); // We occupy the range_search auxiliar function
    return sol_array; // return the array solutions with the points found   
}

void main() {
    
}