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

Point *Cluster(Point *C_in, int C_max, 
                // Parámetros que no están en el pseudo código
                // Número de puntos en C_in
                int num_points) {

    // Partes 1. y 2.
    Point *C_out = NULL;
    Point *C = (struct Point*)malloc(num_points * sizeof(Point));

    for (int i = 0; i < num_points; i++) {
        C[i] = C_in[i]; 
    }

    // Parte 3.
    int C_size = num_points;
    while(C_size != 1) {
        for (int i = 0; i < C_size; i++) {
            for (int j = 0; j < C_size; j++) {
                if (i == j) {
                    continue;
                }
                
            }
        }
    }
}

Entry *OutputHoja(Point *C_in) {

}

Entry *OutputInterno(Entry *C_mra) {

}

Node *sextonSwinbank(Point *P) {

}