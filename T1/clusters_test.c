// A file to test clusters functions
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <float.h>

// Structure of a Point
typedef struct point {
    double x, y;
} Point;

// Function that calculates the Euclidean distance between two points p1 and p2
double euclidean_distance(Point p1, Point p2) {
    double x = p2.x - p1.x;
    double y = p2.y - p1.y;
    return sqrt(x * x + y * y);
}

// Structure to store size of point arrays
typedef struct clusterstruct {
    Point *self;
    int size;
    Point primary_medoid;
} ClusterStruct;

// Structure to store size of cluster arrays
typedef struct Clustersarray {
    ClusterStruct *self;
    int size;
} ClustersArray;

// Función que encuentra el medoide primario en un cluster
Point primary_medoid(ClusterStruct *cluster) {
    double min_radius = 0.0;
    Point primary_medoid = cluster->self[0];
    for (int i = 0; i < cluster->size; i++) {
        double max_distance = 0.0;

        // Calculate the maximum distance to other points in the cluster
        for (int j = 0; j < cluster->size; j++) {
            if (i == j) {
                continue; // Skip comparing a point to itself
            }
            double dist = euclidean_distance(cluster->self[i], cluster->self[j]);
            if (dist > max_distance) {
                max_distance = dist;
            }
        } 

        // Update the minimum radius and primary medoid if needed
        if (max_distance < min_radius || i == 0) {
            min_radius = max_distance;
            primary_medoid = cluster->self[i];
        }
    }

    cluster->primary_medoid = primary_medoid;
    return primary_medoid;
}

// Función que calcula distancia entre dos clusters
double clusterDist(ClusterStruct c1, ClusterStruct c2) {
    Point pm_c1 = primary_medoid(&c1);
    Point pm_c2 = primary_medoid(&c2);
    double dist = euclidean_distance(pm_c1, pm_c2);
    return dist;
}

// Función que devuelve el vecino más cercano de un cluster en clustersSet
ClusterStruct closest_neighbor(ClusterStruct cluster, ClustersArray clustersSet) {

    ClusterStruct closest_neighbor = cluster;
    double min_cluster_dist = DBL_MAX;
    
    for (int i = 0; i < clustersSet.size; i++) {
        ClusterStruct i_cluster = clustersSet.self[i];

        if (&i_cluster == &cluster) {
            continue;
        }

        double dist = clusterDist(cluster, i_cluster);

        if (dist < min_cluster_dist && dist != 0) {
            min_cluster_dist = dist;
            closest_neighbor = i_cluster;
        }
    }
    return closest_neighbor;
}

// Función que une dos clusters
ClusterStruct merge_clusters(ClusterStruct c1, ClusterStruct c2) {
    ClusterStruct merged_cluster;
    merged_cluster.size = c1.size + c2.size;
    merged_cluster.self = (Point*)malloc(merged_cluster.size * sizeof(Point));
    
    // Copy points from c1 and c2 into merged_cluster
    int index = 0;
    for (int i = 0; i < c1.size; i++) {
        merged_cluster.self[index++] = c1.self[i];
    }
    for (int i = 0; i < c2.size; i++) {
        merged_cluster.self[index++] = c2.self[i];
    }
    
    merged_cluster.primary_medoid = primary_medoid(&merged_cluster);

    return merged_cluster;
}

int main() {

    // para compilar gcc clusters_test.c -o clusters_test -lm
    // para ejecutar ./clusters_test

    Point p1 = {0.02, 0.04};
    Point p2 = {0.05, 0.01};
    Point p3 = {0.01, 0.01};
    Point p4 = {0.06, 0.07};
    Point p5 = {0.08, 0.06};
    Point p6 = {0.07, 0.09};
    Point p7 = {0.07, 0.07};
    Point p8 = {0.09, 0.06};

    Point points1[3] = {p1, p2, p3};
    Point points2[3] = {p4, p5, p6};
    Point points3[2] = {p7, p8};

    ClusterStruct c1 = {points1, 3, p1};
    ClusterStruct c2 = {points2, 3, p4};
    ClusterStruct c3 = {points3, 2, p8};

    Point p_medoid1 = primary_medoid(&c1);
    c1.primary_medoid = p_medoid1;

    Point p_medoid2 = primary_medoid(&c2);
    c2.primary_medoid = p_medoid2;

    // primary_medoid funciona
    for (int i = 0; i < c1.size; i++) {
        printf("p%i of c1: (%lf, %lf)\n", i+1, c1.self[i].x, c1.self[i].y);
    }

    printf("Primary medoid of c1: (%lf,%lf)\n", p_medoid1);

    for (int i = 0; i < c2.size; i++) {
        printf("Point %i of c2: (%lf, %lf)\n", i+1, c2.self[i].x, c2.self[i].y);
    }

    printf("Primary medoid of c2: (%lf,%lf)\n", p_medoid2);

    // clusterDist funciona
    printf("Distance between c1 and c2: %lf\n", clusterDist(c1,c2));

    // closest_neighbor funciona
    ClusterStruct clusters_array[3] = {c1, c2, c3};
    ClustersArray clusters = {clusters_array, 3};
    ClusterStruct cl_to_c2 = closest_neighbor(c2, clusters);

    for (int i = 0; i < cl_to_c2.size; i++) {
        printf("Point %i of cl_to_c2: (%lf, %lf)\n", i+1, cl_to_c2.self[i].x, cl_to_c2.self[i].y);
    }

    // merge_clusters funciona
    ClusterStruct merged_cluster = merge_clusters(c1,c3);
    for (int i = 0; i < merged_cluster.size; i++) {
        printf("Point %i of merged cluster: (%lf, %lf)\n", i+1, merged_cluster.self[i].x, merged_cluster.self[i].y);
    }

}