#include "mtree.h"


// Function that returns a sample array of random unique points from a point set
Point* getSamples(Point* point_set, int point_size, int sample_size) {
    int indices[sample_size]; // array that contains the selected indices of the points in the sample
    Point samples[sample_size]; // array with the selected points 
    srand(time(NULL)); // generate a seed given the current time

    // Add a point in the sample array
    for (int i=0; i<sample_size; i++) {
        int random_indices = -1;
        while (random_indices == -1) {
            random_indices = rand() % point_size; // get a random index

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

    return samples; // return the sample array
}

// Function that delete a point from an array
void deletePointFromArray(Point** array, int index, int array_size) {
    for (int i=index; i < array_size - 1; i++) {
        (*array)[i] = (*array)[i+1];
    }
}

// Function that delete a node from an array
void deleteNodeFromArray(Node** array, int index, int* array_size) {
    for (int i=index; i < array_size - 1; i++) {
        (*array)[i] = (*array)[i+1];
    }
    (*array_size)--;
}

// Function that delete an  from an array
void deleteSubsetStructureFromArray(SubsetStructure** array, int index, int array_size) {
    for (int i=index; i < array_size - 1; i++) {
        (*array)[i] = (*array)[i+1];
    }
}

// Function that adds a node to an array
void addNodeToArray(Node** array, Node node, int* array_size) {
    *array = (Node*)realloc(*array, (*array_size + 1) * sizeof(Node));
    (*array)[*array_size] = node;
    (*array_size)++;
}

// Function that adds a point to an array
void addPointToArray(Point** array, Point point, int* array_size) {
    *array = (Point*)realloc(*array, (*array_size + 1) * sizeof(Point));
    (*array)[*array_size] = point;
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

// Function that insert a 'Tj' node into the leaf of a 'node', where the leaf point is the same as the point of F[j].
void joinTj(Node* node,  Node* Tj, Point* F, int j, int* already_inserted) {
    Entry *node_entries = node->entries; // entries of the node
    int entries_size = node->num_entries; // size of entries of the node

    // if the node is a leaf
    if (is_leaf(node)) {
        Point p_j = F[j]; // corresponding point to p_j in F

        // for each entry, verify if his point is equal to p_j
        for (int i=0; i<entries_size; i++) {
            Entry* entry = &node_entries[i];
            Point p = entry->p;
            if (p.x == p_j.x && p.y == p_j.y) { // if the points have the same coordinates, add Tj to that leaf
                entry->a = Tj;
                *already_inserted = 1; // Tj was inserted
                return;
            }
        }
    }

    else {
        // Go to each the node children ánd apply the function recursively.
        for (int i=0; i<entries_size; i++) {
            // if Tj was inserted, break the iteration
            if (*already_inserted) 
                break;

            // if the Tj is not inserted yet
            else {
                Entry* entry = &node_entries[i];
                Node* child = entry->a;
                joinTj(child, Tj, F, j, already_inserted);
            }
        }
    }
}

Node* cpBulkLoading(Point* P, int P_size) {
    // STEP 1

    // If the number of points in the point set is less or equal to B.
    if (P_size <= B) {
        // Then create a new Node structure (it will be a leaf)
        Node* newNode = create_node();

        // For each point in the point set:
        for (int i=0; i<P_size; i++) {
            // Create a new Entry structure with the format on a leaf 
            Entry newEntry = {P[i], 0.0, NULL};
    
            // Add the Entry structure into the 'entries' array of the created node
            insertEntry(newNode, newEntry);
        }

        return newNode;
    }

    
    int F_size; // sample size of F
    Point *F; // array F containing samples chosen at random from P
    SubsetStructure samples_subsets[F_size]; // array that contains, for each element, the Fk array and its size

    do {
        // STEP 2

        F_size = min(B, P_size/B); 
        F = getSamples(P, P_size, F_size); 

        // Initialize every sample subset structure belonging to the sample points in F and add to samples subsets array
        for (int i=0; i<F_size; i++) {
            SubsetStructure newSubsetStructure = {F[i], 0, NULL};
            samples_subsets[i] = newSubsetStructure;
        }


        // STEP 3

        // For each point in the point set, assign to the nearest sample
        for (int i=0; i<P_size; i++) {
            Point p = P[i]; // get the point P[i]

            double nearest_distance = euclidian_distance(p, F[0]);
            int nearest_sample_index = 0;

            // evaluate for each sample point in F
            for (int j=1; j<F_size; j++) {
                int distance = euclidian_distance(p, F[j]);
                if (distance < nearest_distance) {
                    nearest_distance = distance;
                    nearest_sample_index = j;
                }
            }

            // Add the point of P to the subset of the nearest sample
            SubsetStructure* nearest_subset = &(samples_subsets[nearest_sample_index]); // Obtain a pointer to the structure that references the subset of the nearest sample found
            Point** Fj_array = &(nearest_subset->sample_subset); // obtain a pointer to the Fj
            int* Fj_size = &(nearest_subset->subset_size); // obtain a pointer to the Fj size
            addPointToArray(Fj_array, p, Fj_size); // add P[i] to Fj
        }


        // STEP 4. Redistribution
        // STEP 4.1
        for (int j=0; j<F_size; j++) { // for each Fj
            SubsetStructure subset = samples_subsets[j]; // subset structure of Fj
            Point *Fj_array = subset.sample_subset; // array Fj
            int Fj_size = subset.subset_size; // size of the array Fj

            if (Fj_size < b) { // if |Fj| < b
                deletePointFromArray(&F, j, F_size); // delete point Pj from F (element j from F)
                deleteSubsetStructureFromArray(&samples_subsets, j, F_size); // delete subset structure j from samples_subsets
                F_size--; // the sample size is reduced by 1


                // STEP 4.2

                // for each point in Fj
                for (int i=0; i < Fj_size; i++) {
                    Point p = Fj_array[i]; // get the point Fj[i]

                    double nearest_distance = euclidian_distance(p, F[0]);
                    int nearest_sample_index = 0;

                    // Found the nearest subset Fl for the point p (Fj[i])
                    for (int l = 1; l < F_size; l++) {
                        double distance = euclidian_distance(p, F[l]);
                        if (distance < nearest_distance) {
                            nearest_distance = distance;
                            nearest_sample_index = l;
                        }
                    }

                    // Move the point from Fj point to the nearest subset (F_l)
                    SubsetStructure *nearest_subset = &(samples_subsets[nearest_sample_index]); // subset of Fl
                    Point** Fl_array = &(nearest_subset->sample_subset); // array Fl
                    int* Fl_size = &(nearest_subset->subset_size); // size of the array Fl
                    addPointToArray(Fl_array, p, Fl_size);
                }

                // Free memory from the sample array Fj being redistributed
                free(Fj_array);
            }
        }

    } while (F_size == 1); // STEP 5: if the sample size |F| = 1, return to step 2


    // STEP 6

    Node* T = NULL; // array where we will save every Tj obtained from F
    int T_size = 0;

    for (int j = 0; j < F_size; j++) {
        SubsetStructure subset = samples_subsets[j]; // subset structure of Fj
        Point *Fj_array = subset.sample_subset; // array Fj
        int Fj_size = subset.subset_size; // size of the array Fj

        if (Fj_size > 0) { // if |Fj| > 0

            // Recursively call cpBulkLoading for each subset Fj
            Node* Tj = CP(Fj_array, Fj_size);


            // STEP 7
            int Tj_size = Tj->num_entries; // size of the Tj array of entries

            // if Tj size is less than b
            if (Tj_size < b) {
                deletePointFromArray(&F, j, F_size); // delete p_fj from F
                // deleteSubsetStructureFromArray(&samples_subsets, j, F_size); // delete if F_j should be deleted too
                F_size--; // F size is reduced

                // esta parte esta muy dudosa, pero imagino esto segun el enunciado:
                Entry *Tj_entries = Tj->entries; // Entry array of the root

                // for each entry of Tj
                for (int p=0; p < Tj_size; p++) {

                    // add his subtrees to the T array
                    Entry Tj_entry = Tj_entries[p];
                    Node *Tj_subtree = Tj_entry.a;
                    addNodeToArray(&T, *Tj_subtree, &T_size);

                    // "the relevant point is added to F" 
                    Point subtree_point = Tj_entry.p; // point of the Tj entry
                    addPointToArray(&F, subtree_point, F_size); // add the point to F
                    // ADD STRUCTURE TO ARRAY?
                }
            }

            // if root size is greater than or equal to b: Add Tj to the node array T
            else
                addNodeToArray(&T, *Tj, &T_size);
        }
    }


    // STEP 8

    // found h
    int h; // min height of the Tj trees
    h = treeHeight(&T[0]); // set the first subtree height as the minimum

    for (int j=1; j < T_size; j++) {
        Node *Tj = &T[j];

        // if another Tj has smaller height, set that height on h
        int subtree_height = treeHeight(Tj);
        if (subtree_height < h)
            h = subtree_height;
    }

    // Define T' as empty set
    Node *T_prime = NULL;
    int T_prime_size = 0;


    // STEP 9

    // for each Tj 
    for (int j=0; j < T_size; j++) {
        Node *Tj = &T[j]; // Tj
        int Tj_height = treeHeight(Tj); // Hieght of Tj

        // if the Tj height is equal to h, add the Tj to T_prime
        if (Tj_height == h)
            addNodeToArray(&T_prime, *Tj, &T_prime_size);

        else {
            // delete the respective point j in F
            deletePointFromArray(&F, j, F_size);
            // deleteSubsetStructureFromArray(&samples_subsets, j, F_size); // delete if F_j should be deleted too
            F_size--;

            Entry* Tj_entries = Tj->entries;
            int Tj_size = Tj->num_entries;

            // for each subtree in Tj, insert into T_prime if the subtree has height equal to h
            for (int p=0; p < Tj_size; p++) {
                Entry Tj_entry = Tj_entries[p]; // entry of Tj
                Node *subtree = Tj_entry.a; // subtree of Tj
                int subtree_height = treeHeight(subtree); // subtree height

                // if the subtree height is equal to h:
                if (subtree_height == h) {
                    addNodeToArray(&T_prime, *subtree, &T_prime_size); // add this node to T_prime
                    Point root_point = Tj_entry.p; // root point of the subtree of height h
                    addPointToArray(&F, root_point, &F_size); // add the root point to F
                    // ADD STRUCTURE TO ARRAY if when a point p_i is added to F, then a subsetstructure is added to samples subsets corresponding to that point
                }
            }
        }
    }

    // STEP 10
    Node *T_sup = cpBulkLoading(F, F_size); // apply cp algorithm to F (sample array)


    //STEP 11
    
    int Tj_inserted; // variable that indicates if the Tj was inserted. It is useful to evite unnecesary iterations in joinTj recursive function

    // for each Tj in T_prime, insert Tj into the corresponding leaf in T_sup
    for (int j=0; j < T_prime_size; j++) {
        Tj_inserted = 0; // begin with the Tj is not inserted still
        Node *Tj = &T_prime[j]; // Obtain Tj from T_prime
        joinTj(T_sup, Tj, F, j, &Tj_inserted); // insert Tj into a T_sup leaf
    }


    // STEP 12

}