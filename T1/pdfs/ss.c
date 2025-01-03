#include <stddef.h>
#include <float.h>

#include "cp.c"

typedef struct clusterstruct ClusterStruct;
typedef struct clustersarray ClustersArray;
typedef struct entryarray EntryArray;
typedef struct entryarrayarray EntryArrayArray;

// Estructura que representa un Cluster
struct clusterstruct {
    Point *self;
    int size;
    Point primary_medoid;
};

// Estructura que representa un conjunto de clusters
struct clustersarray {
    ClusterStruct *self;
    int size;
};

// Estructura que representa un conjunto de entradas
struct entryarray {
    Entry *self;
    int size;
};

// Estructura que representa un conjunto de un conjunto de entradas
struct entryarrayarray {
    EntryArray *entries_array;
    int size;
};

// Función que encuentra el máximo entre dos doubles
double max(double i, double j) {
    return (i > j) ? i : j;
}

// Función que encuentra el mínimo entre dos doubles
double min(double i, double j) {
    return i < j ? i : j;
}


// Función que añade un cluster c a un conjunto de clusters C
void addCluster(ClustersArray C, ClusterStruct c) {
    if (C.size == 0) {
        C.self = (ClusterStruct *)malloc(sizeof(ClusterStruct));
        C.self[C.size] = c;
        C.size++;
    } else {
        C.self = (ClusterStruct *)realloc(C.self, (C.size + 1) * sizeof(ClusterStruct));
        C.self[C.size] = c;
        C.size++;
    }
}

// Función que quita un cluster c de un conjunto de clusters C
void removeCluster(ClustersArray* C, ClusterStruct* c) {
    /* buscamos la posición del cluster a eliminar */
    int i;
    for (i = 0; i < C->size; i++) {
        ClusterStruct *cluster = &C->self[i];
        if (cluster == c) {
            break;
        }
    }
    /* movemos los clusters de la derecha */
    for (int j = i; j < C->size -1; j++) {
        C->self[j] = C->self[j+1];
    }
    /* redimensionamos el arreglo */
    C->self = realloc(C->self, (C->size - 1) * sizeof(ClusterStruct));
    /* actualizamos el tamaño */
    C->size--;
}

// Función que retorna un arreglo de entradas con las entradas en C que contienen puntos en el cluster c
EntryArray entriesWithPointInCluster(EntryArray C, ClusterStruct c) {
    EntryArray entries;
    entries.self = (Entry *)malloc(c.size * sizeof(Entry));
    entries.size = c.size;
    for (int i = 0; i < c.size; i++) {
        if (c.self[i].x == C.self[i].p.x && c.self[i].y == C.self[i].p.y) {
            entries.self[i] = C.self[i];
        } else {
            continue;
        }
    }
    return entries;
}

// Función que retorna el medoide primario de un cluster
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

// Función que retorna distancia entre dos clusters (distancia entre sus medoides primarios)
double clusterDist(ClusterStruct c1, ClusterStruct c2) {
    Point pm_c1 = primary_medoid(&c1);
    Point pm_c2 = primary_medoid(&c2);
    double dist = euclidean_distance(pm_c1, pm_c2);
    return dist;
}

// Función que retorna el vecino más cercano de un cluster en un conjunto de clusters
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

// Función que realiza min max split policy de un cluster, retorna arreglo con los dos clusters
/* ARREGLAR */
ClustersArray MinMaxSplitPolicy(ClusterStruct cluster) {

    ClustersArray divided_clusters;
    divided_clusters.size = 2;
    divided_clusters.self = (ClusterStruct*)malloc(2 * sizeof(ClusterStruct));
    ClusterStruct c1;
    ClusterStruct c2;
    double min_max_radius = DBL_MAX;

    for (int i = 0; i < cluster.size; i++) {
        for (int j = i + 1; j < cluster.size; j++) {
            if (i == j) {
                continue;
            }
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

// Función que añade una entrada a un nodo
void addEntryInNode(Node *N, Entry e) {
    
    if (N->num_entries == 0) {
        N->entries = (Entry *)malloc(sizeof(Entry));
        N->entries[N->num_entries] = e;
        N->num_entries++;
    }
    else {
        N->entries = (Entry *)realloc(N->entries, (N->num_entries + 1) * sizeof(Entry));
        N->entries[N->num_entries] = e;
        N->num_entries++;
    }
    
}

// Función que añade un punto a un cluster
void addPointInCluster(ClusterStruct C, Point p) {
    if (C.size == 0) {
        C.self = (Point *)malloc(sizeof(Point));
        C.self[C.size] = p;
        C.size++;
    }
    else {
        C.self = (Point *)realloc(C.self, (C.size + 1) * sizeof(Point));
        C.self[C.size] = p;
        C.size++;
    }
}

// Función que retorna un Cluster con los puntos dentrode un arreglo de entradas
ClusterStruct pointsInEntryArray(EntryArray E) {
    Point *C_points;
    ClusterStruct C = {C_points, 0};
    for (int i = 0; i < E.size; i++) {
        Entry e = E.self[i];
        Point e_point = e.p;
        addPointInCluster(C, e_point);
    }
    return C;
}

// Función que añade una entrada a un arreglo de entradas
void addEntryInEntryArray(EntryArray E, Entry e) {
    if (E.size == 0) {
        E.self = (Entry *)malloc(sizeof(Entry));
        E.self[E.size] = e;
        E.size++;
    }
    else {
        E.self = (Entry *)realloc(E.self, (E.size + 1) * sizeof(Entry));
        E.self[E.size] = e;
        E.size++;
    }
}

// Función que añade un arreglo de entradas a un arreglo de arreglos de entradas
void addEntryArrayInEntryArrayArray(EntryArrayArray EE, EntryArray E) {
    if (EE.size == 0) {
        EE.entries_array = (EntryArray *)malloc(sizeof(EntryArray));
        EE.entries_array[EE.size] = E;
        EE.size++;
    }
    else {
        EE.entries_array = (EntryArray *)realloc(EE.entries_array, (EE.size + 1) * sizeof(EntryArray));
        EE.entries_array[EE.size] = E;
        EE.size++;
    }
}

// Función que clusteriza un conjunto de puntos, retorna conjunto de clusters
ClustersArray Cluster(ClusterStruct C_in) {

    if (C_in.size < b) {
        printf("El tamaño del set de puntos es menor a b.\n");
        exit(1);
    }

    // Parte 1.
    ClusterStruct *C_out_clusters;
    ClustersArray C_out = {C_out_clusters, 0};
    ClusterStruct *C_clusters;
    ClustersArray C = {C_clusters, 0};

    // Parte 2.
    for (int i = 0; i < C_in.size; i++) {
        /* añadir {p} a C */
        Point p = C_in.self[i];
        Point *pp = (Point *)malloc(sizeof(Point));
        pp[0] = p;
        ClusterStruct C_p = {pp, 1};
        addCluster(C, C_p);
    }

    // Parte 3.
    while(C.size > 1) {
        // Parte 3.1
        ClustersArray C_closest = closest_pair(C);
        ClusterStruct c1, c2;
        // Parte 3.2
        if (C_closest.self[0].size >= C_closest.self[1].size){
            ClusterStruct c1 = C_closest.self[0];
            ClusterStruct c2 = C_closest.self[1];
        }
        else {
            ClusterStruct c1 = C_closest.self[1];
            ClusterStruct c2 = C_closest.self[0];
        }
        if (c1.size + c2.size <= B) {
            removeCluster(&C, &c1);
            removeCluster(&C, &c2);
            ClusterStruct c_union = merge_clusters(c1, c2);
            addCluster(C, c_union);
        // Parte 3.3
        } else if (c1.size >= c2.size) {
            removeCluster(&C, &c1);
            addCluster(C_out, c1);
        }
    }

    // Parte 4.
    ClusterStruct c = C.self[0];

    // Parte 5.
    ClusterStruct c_prime;
    if (C_out.size > 0) {
        c_prime = closest_neighbor(c, C_out);
        removeCluster(&C_out, &c_prime);
    } else {
        Point *c_prima_points;
        ClusterStruct c_prima = {c_prima_points, 0};
    }

    // Parte 6.
    ClusterStruct c_U_c_prime = merge_clusters(c, c_prime);
    if (c_U_c_prime.size <= B) {
        addCluster(C_out, c_U_c_prime);
    } else {
        ClustersArray divided = MinMaxSplitPolicy(c_U_c_prime);
        ClusterStruct div1 = divided.self[0];
        ClusterStruct div2 = divided.self[1];
        addCluster(C_out, div1);
        addCluster(C_out, div2);      
    }

    // Parte 7.
    return C_out;
}

// Función que retorna entrada para hoja del mtree
Entry OutputHoja(ClusterStruct C_in) {

    Point g = primary_medoid(&C_in);
    double r = 0;
    Entry *C_entries;
    Node C = {C_entries, 0};

    for (int i = 0; i < C_in.size; i++) {
        Point p = C_in.self[i];
        Entry new_entry = {p, 0.0, NULL};
        addEntryInNode(&C, new_entry);
        r = max(r, euclidean_distance(g,p));
    }

    Node *a = &C;

    Entry out = {g, r, a};
    return out;

}

// Función que retorna entrada para nodo interno del mtree
Entry OutputInterno(EntryArray C_mra) {

    ClusterStruct C_in = pointsInEntryArray(C_mra);
    Point G = primary_medoid(&C_in);
    double R = 0.0;
    Entry *C_entries;
    Node C = {C_entries, 0};
    
    for (int i = 0; i < C_mra.size; i++) {
        Entry new_entry = C_mra.self[i];
        addEntryInNode(&C, new_entry);
        R = max(R, euclidean_distance(G,new_entry.p) + new_entry.cr);
    }
    
    Node *A = &C;
    
    Entry out = {G, R, A};
    return out;
}

// Función que realiza el algoritmo ss sobre un conjunto de puntos, retorna el árbol construido
Node *sextonSwinbank(Point *set, int set_size) {

    ClusterStruct C_in;
    C_in.size = set_size;
    C_in.self = (Point *)malloc(set_size * sizeof(Point));
    for (int i = 0; i < set_size; i++) {
        C_in.self[i] = set[i];
    }

    // Parte 1.
    if (C_in.size <= B) {
        Entry leaf = OutputHoja(C_in);
        Node *tree = leaf.a;
        return tree;
    }

    // Parte 2.
    ClustersArray C_out = Cluster(C_in);
    EntryArray C;
    C.self = (Entry *)malloc(C_out.size * sizeof(Entry));
    C.size = 0;

    // Parte 3.
    for (int i = 0; i < C_out.size; i++) {
        C.self[i] = OutputHoja(C_out.self[i]);
        C.size++;
    }

    // Parte 4.
    while(C.size > B) {
        ClusterStruct _C_in;
        _C_in.self = (Point *)malloc(C.size * sizeof(Point));
        _C_in.size = C.size;

        for (int i = 0; i < C.size; i++) {
            _C_in.self[i] = C.self[i].p;
        }

        ClustersArray _C_out = Cluster(_C_in);

        EntryArray C_mra;

        for (int i = 0; i < _C_out.size; i++) {
            EntryArray s = entriesWithPointInCluster(C, _C_out.self[i]);

        } 
        free(_C_in.self);
    }
}
