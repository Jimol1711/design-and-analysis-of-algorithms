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

        SampleSubset samples_subsets[sample_size]; // array that contains the samples subsets for each sample point. STEP 3
        // for each point in the sample subset, create the sample structure 
        for (int i=0; i<sample_size; i++) {
            SampleSubset newSampleSubset;
            newSampleSubset.point = F[i];
            newSampleSubset.subset_size = 0;
            newSampleSubset.sample_subset = NULL;
            samples_subsets[i] = newSampleSubset;
        }

        // For each point in the point set, assign the nearest sample
        for (int i=0; i<set_size; i++) {
            Point nearest_sample = F[0];
            double nearest_distance = euclidian_distance(point_set[i], F[0]);
            int nearest_sample_index = 0;
            for (int k=1; k<sample_size; k++) {
                if (euclidian_distance(point_set[i], F[k]) < nearest_distance) {
                    nearest_sample = F[k];
                    nearest_distance = euclidian_distance(point_set[i], F[k]);
                    nearest_sample_index = k;
                }
            }
            SampleSubset* sample_subset = &(samples_subsets[nearest_sample_index]); // Obtain a pointer to the structure that references the subset of the nearest sample found
            int subset_size = sample_subset->subset_size;
            sample_subset->sample_subset = (Point*)realloc(sample_subset->sample_subset, (subset_size+1) * sizeof(Point));
            (sample_subset->sample_subset)[subset_size] = nearest_sample;
            sample_subset->subset_size++;
        }

        // Redistribution. STEP 4
        for (int k=0; k<sample_size; k++) {
            if (samples_subsets[k].subset_size < b) {
                // QUITAR EL PUNTO DEL ARREGLO .........
            }
        }
    }
}