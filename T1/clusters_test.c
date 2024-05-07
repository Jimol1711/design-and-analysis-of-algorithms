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

// Función que encuentra el mínimo entre dos valores
double min(double a, double b) {
    return a < b ? a : b;
}

// Structure to store size of point arrays
typedef struct clusterstruct {
    Point *self;
    int size;
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

    return merged_cluster;
}

// Función que encuentra el par más cercano en un set de clusters, retorna arreglo con los dos clusters
ClustersArray closest_pair(ClustersArray clustersSet) {

    ClustersArray closest_pair;
    closest_pair.size = 2;
    closest_pair.self = (ClusterStruct*)malloc(2 * sizeof(ClusterStruct));
    
    ClusterStruct c1;
    ClusterStruct c2;
    double min_dist = DBL_MAX;

    for (int i = 0; i < clustersSet.size; i++) {
        for (int j = 0; j < clustersSet.size; j++) {
            if (i == j) {
                continue;
            }
            double dist = clusterDist(clustersSet.self[i], clustersSet.self[j]);
            if (dist < min_dist && dist != 0) {
                min_dist = dist;
                closest_pair.self[0] = clustersSet.self[i];
                closest_pair.self[1] = clustersSet.self[j];
            }
        }
    }
    return closest_pair;
}

// Función que realiza min max split policy de un cluster, devuelve arreglo con los 2 clusters obtenidos
ClustersArray MinMaxSplitPolicy(ClusterStruct cluster) {

    ClustersArray divided_clusters;
    divided_clusters.size = 2;
    divided_clusters.self = (ClusterStruct*)malloc(2 * sizeof(ClusterStruct));
    ClusterStruct c1;
    ClusterStruct c2;
    double min_max_radius = DBL_MAX;

    for (int i = 0; i < cluster.size; i++) {
        for (int j = i + 1; j < cluster.size; j++) {
            // if (i == j) {
            //    continue;
            // }
            // Initialize clusters c1 and c2
            c1.size = 0;
            c1.self = (Point*)malloc(cluster.size * sizeof(Point));
            c2.size = 0;
            c2.self = (Point*)malloc(cluster.size * sizeof(Point));

            for (int k = 0; k < cluster.size; k++) {
                if (k == i || k==j) {
                    continue;
                }
                
                double i_dist = euclidean_distance(cluster.self[i], cluster.self[k]);
                double j_dist = euclidean_distance(cluster.self[j], cluster.self[k]);

                if (i_dist < j_dist) {
                    c1.size++;
                    Point *added_c1 = (Point *)realloc(c1.self, c1.size * sizeof(Point));
                    c1.self = added_c1;
                    c1.self[c1.size - 1] = cluster.self[k];
                } else if (i_dist > j_dist) {
                    c2.size++;
                    Point *added_c2 = (Point *)realloc(c2.self, c2.size * sizeof(Point));
                    c2.self = added_c2;
                    c2.self[c2.size - 1] = cluster.self[k];
                }

            }

            double max_covering_radius = 0.0;

            for (int n = 0; n < c1.size; n++) {
                if (n == i || n == j || n >= c1.size) {
                    continue;
                }
                if (euclidean_distance(c1.self[n], cluster.self[i]) > max_covering_radius) {
                    max_covering_radius = euclidean_distance(c1.self[n], cluster.self[i]);
                }
            }
            for (int m = 0; m < c2.size; m++) {
                if (m == i || m == j || m >= c2.size) {
                    continue;
                    }
                if (euclidean_distance(c2.self[m], cluster.self[i]) > max_covering_radius) {
                    max_covering_radius = euclidean_distance(c2.self[m], cluster.self[i]);
                }
            }

            if (max_covering_radius < min_max_radius) {
                min_max_radius = max_covering_radius;
                divided_clusters.self[0] = c1;
                divided_clusters.self[1] = c2;
            } else {
                // Reinitialize clusters c1 and c2
                c1.size = 0;
                Point *new_c1 = (Point*)realloc(c1.self, cluster.size * sizeof(Point));
                c1.self = new_c1;
                c2.size = 0;
                Point *new_c2 = (Point*)realloc(c2.self, cluster.size * sizeof(Point));
                c2.self = new_c2;               
            }
        }
    }
    free(c1.self);
    free(c2.self);

    return divided_clusters;
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
    Point p7 = {0.09, 0.03};
    Point p8 = {1.0, 0.02};

    Point points1[3] = {p1, p2, p3};
    Point points2[3] = {p4, p5, p6};
    Point points3[2] = {p7, p8};

    ClusterStruct c1 = {points1, 3};
    ClusterStruct c2 = {points2, 3};
    ClusterStruct c3 = {points3, 2};

    Point p_medoid1 = primary_medoid(&c1);

    Point p_medoid2 = primary_medoid(&c2);

    Point p_medoid3 = primary_medoid(&c3);

    // primary_medoid funciona
    printf("primary_medoid funciona\n");
    for (int i = 0; i < c1.size; i++) {
        printf("p%i of c1: (%lf, %lf)\n", i+1, c1.self[i].x, c1.self[i].y);
    }

    for (int i = 0; i < c2.size; i++) {
        printf("p%i of c2: (%lf, %lf)\n", i+1, c2.self[i].x, c2.self[i].y);
    }

    printf("Primary medoid of c1: (%lf,%lf)\n", p_medoid1);
    printf("Primary medoid of c2: (%lf,%lf)\n", p_medoid2);
    printf("Primary medoid of c3: (%lf,%lf)\n", p_medoid3);

    // clusterDist funciona
    printf("\nclusterDist funciona\n");
    printf("Distance between c1 and c2: %lf\n", clusterDist(c1,c2));

    // closest_neighbor funciona
    printf("\nclosest_neighbor funciona\n");
    ClusterStruct clusters_array[3] = {c1, c2, c3};
    ClustersArray clusters = {clusters_array, 3};
    ClusterStruct cl_to_c2 = closest_neighbor(c2, clusters);

    for (int i = 0; i < cl_to_c2.size; i++) {
        printf("Point %i of cl_to_c2: (%lf, %lf)\n", i+1, cl_to_c2.self[i].x, cl_to_c2.self[i].y);
    }

    // merge_clusters funciona
    printf("\nmerge_clusters funciona\n");
    ClusterStruct merged_cluster = merge_clusters(c1,c3);
    for (int i = 0; i < merged_cluster.size; i++) {
        printf("Point %i of merged cluster: (%lf, %lf)\n", i+1, merged_cluster.self[i].x, merged_cluster.self[i].y);
    }

    // closest_pair funciona
    printf("\nclosest_pair funciona\n");
    ClustersArray the_closest_pair = closest_pair(clusters);

    for (int i = 0; i < the_closest_pair.size; i++) {
        for (int j = 0; j < the_closest_pair.self[i].size; j++) {
            printf("Point %i of cluster %i: (%lf,%lf)\n", j + 1, i + 1, the_closest_pair.self[i].self[j].x, the_closest_pair.self[i].self[j].y);
        }
    }

    // Borrar esto
    // Sample points
    Point points[] = {{0.1, 0.2}, {0.4, 0.5}, {0.3, 0.4}, {0.7, 0.6}, {0.9, 0.8}};
    int num_points = 5;

    // Create a cluster with the sample points
    ClusterStruct cluster;
    cluster.size = num_points;
    cluster.self = points;

    // Test the MinMaxSplitPolicy function
    ClustersArray divided_clusters = MinMaxSplitPolicy(cluster);

    // Print the clusters
    printf("\nCluster 1:\n");
    for (int i = 0; i < divided_clusters.self[0].size; i++) {
        printf("(%.2f, %.2f)\n", divided_clusters.self[0].self[i].x, divided_clusters.self[0].self[i].y);
    }
    printf("Cluster 1 size: %d\n", divided_clusters.self[0].size);

    printf("\nCluster 2:\n");
    for (int i = 0; i < divided_clusters.self[1].size; i++) {
        printf("(%.2f, %.2f)\n", divided_clusters.self[1].self[i].x, divided_clusters.self[1].self[i].y);
    }
    printf("Cluster 2 size: %d\n", divided_clusters.self[1].size);

    // Free allocated memory
    // free(divided_clusters.self[0].self);
    // free(divided_clusters.self[1].self);
    // free(divided_clusters.self);

    return 0;
}