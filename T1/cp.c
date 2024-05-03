#include <stdio.h>
#include <stdlib.h>
#include "mtree.h"


// Function that returns a sample array of random unique points from a point set
Point* getSamples(Point* point_set, int set_size, int sample_size) {
    int indices[sample_size]; // array that contains the selected indices of the points in the sample
    Point samples[sample_size]; // array with the selected points 
    srand(time(NULL)); // generate a seed given the current time
    // Add a point in the sample array
    for (int i=0; i<sample_size; i++) {
        int random_indices = -1;
        while (random_indices == -1) {
            random_indices = rand() % set_size; // get a random index
            // for each index already chosen, if that index is equal to the candidate, reject that index and try with another one
            for (int j=0; j<i; j++) {
                if (random_indices == indices[j]) {
                    random_indices = -1;
                    break;
                }
            }
        }
        indices[i] = random_indices; // add the index to the array of selected indices
        samples[i] = point_set[i]; // add the point to the unique sample points
    }
    return samples; // return the samples array
}

Point* deleteElemenFromArray(Point** array, int index, int* array_size) {
    for (int i=index; i < *array_size - 1; i++) {
        (*array)[i] = (*array)[i+1];
    }
    // reduce the array size
    (*array_size)--;
}

Node cpBulkLoading(Point* point_set, int set_size) {
    // If the number of points in the point set is less or equal to B. STEP 1
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
        int sample_size = min(B, set_size/B); // size of the set of samples. STEP 2
        Point* F = getSamples(point_set, set_size, sample_size); // Get the samples array

        SubsetStructure samples_subsets[sample_size]; // array that contains the samples subsets structure for each sample point in F. STEP 3
        // for each point in the sample subset, create the sample structure 
        for (int i=0; i<sample_size; i++) {
            SubsetStructure newSubsetStructure;
            newSubsetStructure.point = F[i];
            newSubsetStructure.subset_size = 0;
            newSubsetStructure.sample_subset = NULL;
            samples_subsets[i] = newSubsetStructure;
        }

        // For each point in the point set, assign to the nearest sample
        for (int i=0; i<set_size; i++) {
            double nearest_distance = euclidian_distance(point_set[i], F[0]);
            int nearest_sample_index = 0;
            for (int k=1; k<sample_size; k++) {
                if (euclidian_distance(point_set[i], F[k]) < nearest_distance) {
                    nearest_distance = euclidian_distance(point_set[i], F[k]);
                    nearest_sample_index = k;
                }
            }
            SubsetStructure* subset_structure = &(samples_subsets[nearest_sample_index]); // Obtain a pointer to the structure that references the subset of the nearest sample found
            int subset_size = subset_structure->subset_size;
            subset_structure->sample_subset = (Point*)realloc(subset_structure->sample_subset, (subset_size+1) * sizeof(Point));
            (subset_structure->sample_subset)[subset_size] = point_set[i];
            subset_structure->subset_size++;
        }

        // Redistribution. STEP 4
        for (int j=0; j<sample_size; j++) { // for each F_j
            if (samples_subsets[j].subset_size < b) { // if |F_j| < b
                deleteElemenFromArray(&F, j, &sample_size); // delete p_j from F (element j from F)

                // for each p in F_j
                for (int i=0; i < samples_subsets[j].subset_size; i++) {
                    Point p = (samples_subsets[j].sample_subset)[i]; // get the point p
                    double nearest_distance = euclidian_distance(p, F[0]);
                    int nearest_sample_index = 0;

                    // Found the nearest subset for the point p
                    for (int k = 1; k < sample_size; k++) {
                        double distance = euclidian_distance(p, F[k]);
                        if (distance < nearest_distance) {
                            nearest_distance = distance;
                            nearest_sample_index = k;
                        }
                    }

                    // Move p point to the nearest subset (F_l)
                    SubsetStructure *subset_structure = &(samples_subsets[nearest_sample_index]);
                    int subset_size = subset_structure->subset_size;
                    subset_structure->sample_subset = (Point *)realloc(subset_structure->sample_subset, (subset_size + 1) * sizeof(Point));
                    (subset_structure->sample_subset)[subset_size] = p;
                    subset_structure->subset_size++;
                }

                // Free memory from the sample set being redistributed
                free((samples_subsets[j].sample_subset));
            }
        }

        // Hacer un do while...
    }
}