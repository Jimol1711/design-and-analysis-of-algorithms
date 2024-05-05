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

// Function that delete an element from an array
void deleteElemenFromArray(Point** array, int index, int array_size) {
    for (int i=index; i < array_size - 1; i++) {
        (*array)[i] = (*array)[i+1];
    }
}

// Function that adds an element to an array
void addNodeToArray(Node** array, Node node, int* array_size) {
    *array = (Node*)realloc(*array, (*array_size + 1) * sizeof(Node));
    (*array)[*array_size] = node;
    (*array_size)++;
}

// Function that insert an entry into the entries array of a node
void insertEntry(Node* node, Entry entry) {
    node->entries[node->num_entries] = entry;
    (node->num_entries)++;
}

// Function that calculates the height of a tree
int treeHeight(Node* node) {
    if (node == NULL) {
        return 0; // If the tree is empty, height is 0
    } else {
        int max_subtree_height = 0;
        // Iterate through all entries in the node
        for (int i = 0; i < node->num_entries; i++) {
            Node* child = node->entries[i].a;
            int subtree_height = treeHeight(child); // Recursively find height of each child subtree
            if (subtree_height > max_subtree_height) {
                max_subtree_height = subtree_height; // Update the maximum subtree height
            }
        }
        return 1 + max_subtree_height; // Height of the current node is 1 plus the maximum height of its child subtrees
    }
}

Node* cpBulkLoading(Point* point_set, int set_size) {
    // STEP 1
    // If the number of points in the point set is less or equal to B.
    if (set_size <= B) {
        // Then create a new Node structure (it will be a leaf)
        Node* newNode = create_node();
        // For each point in the point set:
        for (int i=0; i<set_size; i++) {
            // Create a new Entry structure with the format on a leaf 
            Entry newEntry;
            newEntry.p = point_set[i];
            newEntry.cr = 0.0; 
            newEntry.a = NULL;
            // Add the Entry structure into the 'entries' array of the created node
            insertEntry(newNode, newEntry);
        }
        return newNode;
    }

    else {
        int sample_size; // declaration of sample size variable
        Point* F;

        sample_size = min(B, set_size/B);
        F = getSamples(point_set, set_size, sample_size);

        int iteracion = 0;

        SubsetStructure samples_subsets[sample_size]; // array that contains the samples subsets structure for each sample point in F. STEP 3
        // for each point in the sample subset, create the sample structure 
        // maybe it goes inside the do while. if true, it would be easier
        for (int i=0; i<sample_size; i++) {
            SubsetStructure newSubsetStructure;
            newSubsetStructure.point = F[i];
            newSubsetStructure.subset_size = 0;
            newSubsetStructure.sample_subset = NULL;
            samples_subsets[i] = newSubsetStructure;
        }

        do {
            // STEP 2
            if (iteracion == 0)
                iteracion++;
            else {
                sample_size = min(B, set_size/B); // size of the set of samples.
                F = getSamples(point_set, set_size, sample_size); // Get the samples array
            }

            // For each point in the point set, assign to the nearest sample
            for (int i=0; i<set_size; i++) {
                double nearest_distance = euclidian_distance(point_set[i], F[0]);
                int nearest_sample_index = 0;
                // evaluate for each sample point in F
                for (int k=1; k<sample_size; k++) {
                    if (euclidian_distance(point_set[i], F[k]) < nearest_distance) {
                        nearest_distance = euclidian_distance(point_set[i], F[k]);
                        nearest_sample_index = k;
                    }
                }
                // Add the point of the set point to the subset of the nearest sample
                SubsetStructure* subset_structure = &(samples_subsets[nearest_sample_index]); // Obtain a pointer to the structure that references the subset of the nearest sample found
                int subset_size = subset_structure->subset_size;
                subset_structure->sample_subset = (Point*)realloc(subset_structure->sample_subset, (subset_size+1) * sizeof(Point));
                (subset_structure->sample_subset)[subset_size] = point_set[i];
                subset_structure->subset_size++;
            }

            // Redistribution. STEP 4
            for (int j=0; j<sample_size; j++) { // for each F_j
                SubsetStructure current_substet = samples_subsets[j];
                if (current_substet.subset_size < b) { // if |F_j| < b
                    deleteElemenFromArray(&F, j, sample_size); // delete p_j from F (element j from F)
                    deleteElemenFromArray(&samples_subsets, j, sample_size); // delete subset structure j from sample_subset
                    sample_size--; // the sample size is reduced by 1

                    // for each p in F_j
                    for (int i=0; i < current_substet.subset_size; i++) {
                        Point p = (current_substet.sample_subset)[i]; // get the point p
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
                    free((current_substet.sample_subset));
                }
            }
        } while (sample_size == 1); // if the sample size |F| = 1, return to step 2

        // STEP 6
        Node* T = create_node();

        for (int j = 0; j < sample_size; j++) {
            // Recursively call cpBulkLoading for each subset F_j
            if (samples_subsets[j].subset_size > 0) {
                Node* root = CP(samples_subsets[j].sample_subset, samples_subsets[j].subset_size);

                // STEP 7
                // if root size is less than b
                if (root->num_entries < b) {
                    deleteElemenFromArray(&F, j, sample_size); // delete p_fj from F
                    eleteElemenFromArray(&samples_subsets, j, sample_size); // delete the sample point subset too?
                    sample_size--; // F size is reduced

                    // esta parte esta muy dudosa, pero imagino esto segun el enunciado:
                    Entry *root_entries = root->entries; // Entry array of the root
                    // for each entry of the root, add the entry to the main node T
                    for (int p=0; p < root->num_entries; p++) {
                        Entry root_entry = root_entries[p];
                        insertEntry(T, root_entry);
                    }
                }

                // if root size is greater than or equal to b: Add the root to the main node T
                else {
                    Entry entry = {F[j], 0.0, root};
                    insertEntry(T, entry);
                }

            }
        }

        // STEP 8
        // found h
        int h;
        Entry *T_root_entries = T->entries;
        int T_num_entries = T->num_entries;

        h = treeHeight(T_root_entries[0].a); // set the first subtree height as the minimum
        for (int j=1; j < T_num_entries; j++) {
            Node *subtree = T_root_entries[j].a;
            // if another subtree has smaller height, set that height on h
            int subtree_height = treeHeight(subtree);
            if (subtree_height < h)
                h = subtree_height;
        }

        // Define T' as empty set
        Node *T_prime = NULL;
        int T_prime_size = 0;

        // STEP 9

        // for each Tj 
        for (int j=0; j < T_num_entries; j++) {
            Node *subtree = T_root_entries[j].a;
            int subtree_height = treeHeight(subtree);
            // if the subtree height is equal to h, add the subtree to T_prime
            if (subtree_height == h)
                addNodeToArray(&T_prime, *subtree, &T_prime_size);
                
            else {

            }
        }

    }


}