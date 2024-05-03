#include <stddef.h>
#include <float.h>

#include "mtree.c"

// Función que encuentra el medoide primario en un cluster
Point primary_medoid(Point *cluster, int cluster_size) {
    double min_radius = DBL_MAX;
    Point primary_medoid;
    for (int i = 0; i < cluster_size; i++) {
        double max_distance = 0.0;

        // Calculate the maximum distance to other points in the cluster
        for (int j = 0; j < cluster_size; j++) {
            if (i == j) {
                continue; // Skip comparing a point to itself
            }
            double dist = euclidean_distance(cluster[i], cluster[j]);
            if (dist > max_distance) {
                max_distance = dist;
            }
        }

        // Update the minimum radius and primary medoid if needed
        if (max_distance < min_radius) {
            min_radius = max_distance;
            primary_medoid = cluster[i];
        }
    }
    return primary_medoid;
}

// Función que devuelve un vecino más cercano de un cluster
Point *closest_neighbor(Point *cluster, Point **Cluster_set) {

}

// Función que une dos clusters
Point *merge_clusters(Point *c1, Point *c2) {

}

// Función que realiza min max split policy de un cluster, devuelve arreglo con punteros a los 2 clusters obtenidos
Point **MinMaxSplitPolicy(Point *cluster) {

}

// Función que encuentra el máximo entre dos valores
double max(double a, double b) {
    return (a > b) ? a : b;
}

Point *Cluster(Point *C_in, int C_max, 
                // Parámetros que no están en el pseudo código
                // Número de puntos en C_in
                int num_points) {

    // Partes 1. y 2.
    Point *C_out = NULL;
    Point **C = (struct Point*)malloc(num_points * sizeof(Point));

    for (int i = 0; i < num_points; i++) {
        Point *ptr;
        ptr = &C_in[i];
        C[i] = ptr; 
    }

    // Parte 3.
    int C_size = num_points;
    int Cout_size = 0;
    while(C_size != 1) {
        for (int i = 0; i < C_size; i++) {
            for (int j = 0; j < C_size; j++) {
                if (i == j) {
                    continue;
                }

            }
        }
        // WIP
    }

    // Parte 4. y 5.
    Point *c = C[0];
    Point *c_prime;
    if (Cout_size > 0) {
        Point *c_prime = closest_neighbor(c, &C_out);
        // Remove c_prime from C_out
    } else {
        Point *c_prime = NULL;
    }

    // Parte 6.
    Point *c_U_c_prime = merge_clusters(c, c_prime);
    int c_U_c_prime_size; 
    // Determine c_U_c_prime size
    if (c_U_c_prime_size <= B) {
        // Add c_U_c_prime to C_out
    } else {
        Point **c1_and_c2 = MinMaxSplitPolicy(c_U_c_prime);
        Point *c1 = c1_and_c2[0];
        Point *c2 = c1_and_c2[1];
        // Add c1 and c2 to C_out
    }

    // Parte 7.
    return C_out;
}

Entry *OutputHoja(Point *C_in,
                // Parámetros que no están en el pseudo código
                // Cardinalidad de C_in
                int C_in_size) {

    // Parte 1.
    Point g = primary_medoid(C_in, C_in_size);
    double r = 0.0;
    Entry *C = malloc(C_in_size * sizeof(Entry));

    // Parte 2.
    for (int i = 0; i < C_in_size; i++) {
        C[i].p = C_in[i];
        C[i].cr = 0.0;
        C[i].a = NULL;
        r = max(r, euclidean_distance(g, C[i].p));
    }

    // Parte 3.
    Point *a = &C;

    // Parte 4.
    Entry *leaf;
    leaf->p = g;
    leaf->cr = r;
    leaf->a = a;
    return leaf;

}

Entry *OutputInterno(Entry *C_mra) {

}

Node *sextonSwinbank(Point *P) {

}