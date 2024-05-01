#include <stdio.h>
#include <stdlib.h>
#include "mtree.h"


Node cpBulkLoading(Point* point_set, int set_size) {
    // If the number of points in the point set is less or equal to B
    if (set_size <= B) {
        // Then create a new Node structure (it will be a leaf)
        Node newNode;
        newNode.num_entries = 0;
        // For each point in the point set:
        for (int i=0; i<set_size; i++) {
            // Create a new Entry structure with the format on a leaf 
            Entry newEntry;
            newEntry.p = point_set[i];
            newEntry.cr = 0.0; 
            newEntry.a = NULL;
            // Add the Entry structure into the 'entries' array of the created node
            newNode.entries[i] = newEntry;
        }
        return newNode;
    }
    
    else {

    }
}