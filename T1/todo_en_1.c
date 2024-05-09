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
typedef struct subsetstructure SubsetStructure;

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
struct subsetstructure {
    Point point;
    Point* sample_subset;
    int subset_size;
};

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

/*






DESDE AQUÍ CP






*/

// Function that returns a sample array of random unique points from a point set
Point* getSamples(Point* point_set, int point_size, int sample_size) {
    int indices[sample_size]; // array that contains the selected indices of the points in the sample
    Point *samples = malloc(sample_size * sizeof(Point)); // array with the selected points 
    srand(time(NULL)); // generate a seed given the current time

    // Add a point in the sample array
    for (int i=0; i<sample_size; i++) {
        int random_indices = -1;
        int iter = 0;
        while (random_indices == -1) {
            random_indices = rand() % point_size; // get a random index

            // for each index already chosen, if that index is equal to the candidate, reject that index and try with another one
            for (int j=0; j<i; j++) {
                if (random_indices == indices[j]) {
                    random_indices = -1;
                    break;
                }
            }
            if (iter >= 100) {
                printf("fail 1\n");
                exit(1);
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
    for (int i=index; i < (*array_size) - 1; i++) {
        (*array)[i] = (*array)[i+1];
    }
    (*array_size)--;
}

// Function that delete an  from an array
void deleteSubsetStructureFromArray(SubsetStructure* array, int index, int array_size) {
    for (int i=index; i < array_size - 1; i++) {
        array[i] = array[i+1];
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

// Function that set the covering radius of each subtree of a node
void setCoveringRadius(Node *node) {
    Entry* node_entries = node->entries; // entries from the node
    int entries_size = node->num_entries; // number of entries of the node

    // if the node is a leaf, the covering radius of his entries are equal to 0.0
    if (is_leaf(node)) {
        // for each entry, set the covering radio as 0.0
        for (int i=0; i < entries_size; i++) {
            node_entries[i].cr = 0.0;
        }
    }

    else {
        // for each entry of the node, set his covering radius
        for (int i=0; i < entries_size; i++) {
            Point p = node_entries[i].p; // point p of the entry
            Node *a = node_entries[i].a; // subtree of the node

            Entry* a_entries = a->entries; // entries of the subtree
            int a_size = a->num_entries; // size of the entries of the subtree

            double maxDistance = 0; // set the initial max distance as 0

            // for each entry in the subtree, search the max distance between the entry point and the parent point
            for (int j=0; j < a_size; j++) {
                double distance = euclidean_distance(p, a_entries[j].p); // distance between p and the point of the j entry of the subtree
                if (distance > maxDistance)
                    maxDistance = distance;
            }

            node_entries[i].cr = maxDistance; // set the covering radius for the entry

            setCoveringRadius(a); // recursion on subtrees
        }
    }
}

// Función que encuentra el mínimo entre dos ints
int intMin(int i, int j) {
    return i < j ? i : j;
}

Node* cpBulkLoading(Point* P, int P_size) {
    // STEP 1
    // printf("Begin Step 1\n");
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
    // printf("End Step 1\n");
    
    int F_size = intMin(B, P_size / B); // sample size of F
    // printf("%i", F_size);
    Point *F; // array F containing samples chosen at random from P
    SubsetStructure samples_subsets[F_size]; // array that contains, for each element, the Fk array and its size;

    // printf("Begin Step 2\n");
    int iter = 0;

    do {
        // STEP 2
        if (iter >= 1000) {
            printf("fail 2 because F_size = %i\n", F_size);
            exit(1);
        }
        // F_size = intMin(B, P_size / B); 
        F = getSamples(P, P_size, F_size); 
        // Initialize every sample subset structure belonging to the sample points in F and add to samples subsets array
        for (int i=0; i < F_size; i++) {

            // printf("Size of F: %i\n", F_size);
            
            SubsetStructure newSubsetStructure = {F[i], NULL, 0};
            samples_subsets[i] = newSubsetStructure;
        }
        // printf("Step 3\n");
        // STEP 3

        // For each point in the point set, assign to the nearest sample
        for (int i=0; i<P_size; i++) {
            Point p = P[i]; // get the point P[i]

            double nearest_distance = euclidean_distance(p, F[0]);
            int nearest_sample_index = 0;

            // evaluate for each sample point in F
            for (int j=1; j<F_size; j++) {
                int distance = euclidean_distance(p, F[j]);
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

        // printf("Step 4\n");
        // STEP 4. Redistribution
        // STEP 4.1
        for (int j=0; j<F_size; j++) { // for each Fj
            SubsetStructure subset = samples_subsets[j]; // subset structure of Fj
            Point *Fj_array = subset.sample_subset; // array Fj
            int Fj_size = subset.subset_size; // size of the array Fj

            if (Fj_size < b) { // if |Fj| < b
                deletePointFromArray(&F, j, F_size); // delete point Pj from F (element j from F)
                deleteSubsetStructureFromArray(samples_subsets, j, F_size); // delete subset structure j from samples_subsets
                F_size--; // the sample size is reduced by 1


                // STEP 4.2

                // for each point in Fj
                for (int i=0; i < Fj_size; i++) {
                    Point p = Fj_array[i]; // get the point Fj[i]

                    double nearest_distance = euclidean_distance(p, F[0]);
                    int nearest_sample_index = 0;

                    // Found the nearest subset Fl for the point p (Fj[i])
                    for (int l = 1; l < F_size; l++) {
                        double distance = euclidean_distance(p, F[l]);
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

        iter++;

    } while (F_size == 1); // STEP 5: if the sample size |F| = 1, return to step 2
    // printf("Passed Step 5\n");

    // STEP 6
    // printf("Step 6\n");
    Node* T = NULL; // array where we will save every Tj obtained from F
    int T_size = 0;

    for (int j = 0; j < F_size; j++) {
        SubsetStructure subset = samples_subsets[j]; // subset structure of Fj
        Point *Fj_array = subset.sample_subset; // array Fj
        int Fj_size = subset.subset_size; // size of the array Fj

        if (Fj_size > 0) { // if |Fj| > 0

            // Recursively call cpBulkLoading for each subset Fj
            Node* Tj = cpBulkLoading(Fj_array, Fj_size);


            // STEP 7
            // printf("Step 7\n");
            int Tj_size = Tj->num_entries; // size of the Tj array of entries

            // if Tj size is less than b
            if (Tj_size < b) {
                deletePointFromArray(&F, j, F_size); // delete p_fj from F
                F_size--; // F size is reduced

                Entry *Tj_entries = Tj->entries; // Entry array of the root

                // for each entry of Tj
                for (int p=0; p < Tj_size; p++) {

                    // add his subtrees to the T array
                    Entry Tj_entry = Tj_entries[p];
                    Node *Tj_subtree = Tj_entry.a;
                    addNodeToArray(&T, *Tj_subtree, &T_size);

                    // "the relevant point is added to F" 
                    Point subtree_point = Tj_entry.p; // point of the Tj entry
                    addPointToArray(&F, subtree_point, &F_size); // add the point to F
                }
            }

            // if root size is greater than or equal to b: Add Tj to the node array T
            else
                addNodeToArray(&T, *Tj, &T_size);
        }
    }

    // STEP 8
    // printf("Step 8\n");
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
    // printf("Step 9\n");
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
                }
            }
        }
    }

    // STEP 10
    // printf("Step 10\n");
    Node *T_sup = cpBulkLoading(F, F_size); // apply cp algorithm to F (sample array)


    //STEP 11
    // printf("Step 11\n");
    int Tj_inserted; // variable that indicates if the Tj was inserted. It is useful to evite unnecesary iterations in joinTj recursive function

    // for each Tj in T_prime, insert Tj into the corresponding leaf in T_sup
    for (int j=0; j < T_prime_size; j++) {
        Tj_inserted = 0; // begin with the Tj is not inserted still
        Node *Tj = &T_prime[j]; // Obtain Tj from T_prime
        joinTj(T_sup, Tj, F, j, &Tj_inserted); // insert Tj into a T_sup leaf
    }

    // STEP 12
    // printf("Step 12\n");
    // set the covering radius for each entry of T_sup
    setCoveringRadius(T_sup);

    // STEP 13
    // printf("Step 13\n");
    // return T_sup
    return T_sup;
}

/*






DESDE AQUÍ MTREE TEST






*/

// #include "mtree.c"
// #include "cp.c"
// #include "ss.c"

// Function that returns a random double value between 0 and 1
double random_double() {
    return (double)rand() / RAND_MAX;
}

// Function that returns two to the exponent
int power_of_two(int exponent) {
    int result = 1;
    for (int i = 0; i < exponent; i++) {
        result *= 2;
    }
    return result;
}

int main() {

    // Para compilar: gcc mtree-test.c -o mtree-test -lm
    // Para ejecutar: ./mtree

    // Test para la búsqueda (crear árbol y query, ver que devuelva los puntos en la query)

    // Test de cp (contar accesos a disco en árbol construido con cp para n puntos, con n
    // entre 2**10 y 2**25)

    // Test de ss (contar accesos a disco en árbol construido con ss para n puntos, con n
    // entre 2**10 y 2**25)

    // ======================
    // Determinar tamano de B
    // ======================
    printf("tamano de entrada: %i\n", sizeof(Entry));
    printf("tamano de B sería: %d\n", 4096 / sizeof(Entry));

    // =====================================================================================================
    // Creación set de puntos aleatorios para cada n entre 2**10 y 2**25 y set de consultas Q con 100 puntos
    // =====================================================================================================

    // Seed the random number generator
    srand(time(NULL));

    // Arreglo de queries
    Query Q[100];

    // Asignación de 100 puntos aleatorios y radio 0.02
    for (int i = 0; i < 100; i++) {
        Point p;
        p.x = random_double();
        p.y = random_double();

        Q[i].q = p;
        Q[i].r = 0.02;
    }

    // Imprimimos para probar que funcionó
    for (int i = 0; i < 100; i++) {
        printf("Query %d - Point: (%lf, %lf)\n", i + 1, Q[i].q.x, Q[i].q.y);
    }

    // Puntos para testing de los métodos
    int point_nums[16];
    for (int i = 0; i < 16; i++) {
        point_nums[i] = power_of_two(i + 10);
    }

    // Imprimimos para probar que funcionó
    for (int i = 0; i < 16; i++) {
        printf("Array %i size: %i\n", i+1, point_nums[i]);
    }

    // Arreglo con punteros a cada arreglo de puntos
    Point *P[16];

    // Creamos puntos aleatorios para cada arreglo
    for (int i = 0; i < 16; i++) {
        P[i] = (Point*)malloc(point_nums[i] * sizeof(Point));
        for (int j = 0; j < point_nums[i]; j++) {
            P[i][j].x = random_double();
            P[i][j].y = random_double();
        }
    }

    // Imprimimos para probar que funcionó
    for (int i = 0; i < point_nums[0]; i++) {
        printf("Point %i: (%lf, %lf)\n", i + 1, P[0][i].x, P[0][i].y);
        if (i == 5) {
            break;
        }
    }

    // =======
    // Testing
    // =======

    // 1. Ciaccia Patella
    int cp_disk_acceses[16];

    // Iteramos en cada conjunto con las 100 consultas y almacenamos accesos
    for (int i = 0; i < 1; i++) {
        printf("Begin experiment\n");
        Node *cp_tree = cpBulkLoading(P[i], power_of_two(i + 10));
        printf("Passed algorithm\n");
        int acceses = 0;
        for (int j = 0; j < 100; j++) {
            Point *search = search_points_in_radio(cp_tree, Q[j], &acceses);
        }
        cp_disk_acceses[i] = acceses;
        printf("End experiment\n");
    }
    
    // 2. Sexton Swinbank
    int ss_disk_acceses[16];

    // Iteramos en cada conjunto con las 100 consultas y almacenamos accesos
    for (int i = 0; i < 1; i++) {
        // Node *ss_tree = sextonSwinbank(&P[i], point_nums[i]);
        int acceses = 0;
        for (int j = 0; j < 100; j++) {
        //    Point *search = search_points_in_radio(ss_tree, Q[j], &acceses);
        }
        ss_disk_acceses[i] = acceses;
    }

    // Imprimimos para probar que funcionó
    for (int i = 0; i < 1; i++) {
        printf("CP acceses for set %i: %i", i + 1, cp_disk_acceses[i]);
        // printf("SS acceses for set %i: %i\n", i + 1, ss_disk_acceses[i]);
    }

    // Liberamos memoria de cada arreglo
    for (int i = 0; i < 16; i++) {
        free(P[i]);
    }
    
    return 0;
}
