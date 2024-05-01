#include <stdio.h>
#include <stdlib.h>

#include "mtree.h"

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

// Auxiliary function that adds the solutions (points) that satisfy the condition in a node entry
void range_search(Node* node, Query Q, Point** sol_array, int* array_size, int* disk_accesses) {
    Point q = Q.q; // query point
    double r = Q.r; // query radio
    int num_entries = node->num_entries; // number of entries in the node
    Entry* entries = node->entries; // Array of entries of the node

    // If the node is a leaf, search each entry that satisfy the condition of distance.
    // if the entry satisfies it, then increase the sol_array length and add the point to sol_array
    if (is_leaf(node)) {
        for (int i=0; i<num_entries; i++) {
            if(euclidian_distance(entries[i].p, q) <= r) {
                *sol_array = (Point*)realloc(*sol_array, (*array_size) + 1 * sizeof(Point));
                (*sol_array)[*array_size] = q;
                (*array_size)++;
            }
        }
    }
    // if is not a leaf, for each entry, if satisfy this distance condition go down to check its child node 'a'
    else {
        for (int i=0; i<num_entries; i++) {
            if(euclidian_distance(entries[i].p, q) <= entries[i].cr + r){
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


void main() {
    // Para compilar: gcc mtree.c -o mtree -lm
    // Para ejecutar: ./mtree

    // Determinar tamano de B
    printf("tamano de entrada: %i\n", sizeof(Entry));
    printf("tamano de B sería: %d\n", 4096 / sizeof(Entry));

}