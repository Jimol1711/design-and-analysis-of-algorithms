#include "cp.c"

typedef struct {
    Point *points;
    int size;
} Cluster;

typedef struct {
    Cluster *clusters;
    int size;
} ClusterArray;

typedef struct {
    Entry *entries;
    int size;
} EntryArray;

typedef struct {
    EntryArray *entries_array;
    int size;
} EntryArrayArray;

// Función que encuentra el medoide primario en un cluster
Point primary_medoid(Cluster *cluster) {
    double min_radius = 0.0;
    Point primary_medoid = cluster->points[0];
    for (int i = 0; i < cluster->size; i++) {
        double max_distance = 0.0;

        // Calculate the maximum distance to other points in the cluster
        for (int j = 0; j < cluster->size; j++) {
            if (i == j) {
                continue; // Skip comparing a point to itself
            }
            double dist = euclidean_distance(cluster->points[i], cluster->points[j]);
            if (dist > max_distance) {
                max_distance = dist;
            }
        } 

        // Update the minimum radius and primary medoid if needed
        if (max_distance < min_radius || i == 0) {
            min_radius = max_distance;
            primary_medoid = cluster->points[i];
        }
    }

    return primary_medoid;
}

// Función que calcula distancia entre dos clusters
double clusterDist(Cluster c1, Cluster c2) {
    Point pm_c1 = primary_medoid(&c1);
    Point pm_c2 = primary_medoid(&c2);
    double dist = euclidean_distance(pm_c1, pm_c2);
    return dist;
}

// Función que devuelve el vecino más cercano de un cluster en clustersSet
int closest_neighbor(Cluster cluster, ClusterArray clustersSet) {

    int closest_neighbor;
    double min_cluster_dist = __DBL_MAX__;
    
    for (int i = 0; i < clustersSet.size; i++) {
        Cluster i_cluster = clustersSet.clusters[i];

        if (&i_cluster == &cluster) {
            continue;
        }

        double dist = clusterDist(cluster, i_cluster);

        if (dist < min_cluster_dist && dist != 0) {
            min_cluster_dist = dist;
            closest_neighbor = i;
        }
    }
    return closest_neighbor;
}

// Función que une dos clusters
Cluster merge_clusters(Cluster c1, Cluster c2) {
    Cluster merged_cluster;
    merged_cluster.size = c1.size + c2.size;
    merged_cluster.points = (Point*)malloc(merged_cluster.size * sizeof(Point));
    
    // Copy points from c1 and c2 into merged_cluster
    int index = 0;
    for (int i = 0; i < c1.size; i++) {
        merged_cluster.points[index++] = c1.points[i];
    }
    for (int i = 0; i < c2.size; i++) {
        merged_cluster.points[index++] = c2.points[i];
    }

    return merged_cluster;
}

// Función que encuentra el par más cercano en un set de clusters, retorna arreglo con los dos clusters
int* closest_pair(ClusterArray* clustersSet) {

    int* closest_pair = (int*)malloc(2 * sizeof(int));
    double min_dist = __DBL_MAX__;

    for (int i = 0; i < clustersSet->size; i++) {
        for (int j = 0; j < clustersSet->size; j++) {
            if (i == j) {
                continue;
            }
            double dist = clusterDist(clustersSet->clusters[i], clustersSet->clusters[j]);
            if (dist < min_dist && dist != 0) {
                min_dist = dist;
                closest_pair[0] = i;
                closest_pair[1] = j;
            }
        }
    }
    return closest_pair;
}

// Función que realiza min max split policy de un cluster, devuelve arreglo con los 2 clusters obtenidos
ClusterArray MinMaxSplitPolicy(Cluster cluster) {

    ClusterArray divided_clusters;
    divided_clusters.size = 2;
    divided_clusters.clusters = (Cluster*)malloc(2 * sizeof(Cluster));
    Cluster c1;
    Cluster c2;
    double min_max_radius = __DBL_MAX__;

    for (int i = 0; i < cluster.size; i++) {
        for (int j = i + 1; j < cluster.size; j++) {
            // if (i == j) {
            //    continue;
            // }
            // Initialize clusters c1 and c2
            c1.size = 0;
            c1.points = (Point*)malloc(cluster.size * sizeof(Point));
            c2.size = 0;
            c2.points = (Point*)malloc(cluster.size * sizeof(Point));

            for (int k = 0; k < cluster.size; k++) {
                if (k == i || k==j) {
                    continue;
                }
                
                double i_dist = euclidean_distance(cluster.points[i], cluster.points[k]);
                double j_dist = euclidean_distance(cluster.points[j], cluster.points[k]);

                if (i_dist < j_dist) {
                    c1.size++;
                    Point *added_c1 = (Point *)realloc(c1.points, c1.size * sizeof(Point));
                    c1.points = added_c1;
                    c1.points[c1.size - 1] = cluster.points[k];
                } else if (i_dist > j_dist) {
                    c2.size++;
                    Point *added_c2 = (Point *)realloc(c2.points, c2.size * sizeof(Point));
                    c2.points = added_c2;
                    c2.points[c2.size - 1] = cluster.points[k];
                }

            }

            double max_covering_radius = 0.0;

            for (int n = 0; n < c1.size; n++) {
                if (n == i || n == j || n >= c1.size) {
                    continue;
                }
                if (euclidean_distance(c1.points[n], cluster.points[i]) > max_covering_radius) {
                    max_covering_radius = euclidean_distance(c1.points[n], cluster.points[i]);
                }
            }
            for (int m = 0; m < c2.size; m++) {
                if (m == i || m == j || m >= c2.size) {
                    continue;
                    }
                if (euclidean_distance(c2.points[m], cluster.points[i]) > max_covering_radius) {
                    max_covering_radius = euclidean_distance(c2.points[m], cluster.points[i]);
                }
            }

            if (max_covering_radius < min_max_radius) {
                min_max_radius = max_covering_radius;
                divided_clusters.clusters[0] = c1;
                divided_clusters.clusters[1] = c2;
            } else {
                // Reinitialize clusters c1 and c2
                c1.size = 0;
                Point *new_c1 = (Point*)realloc(c1.points, cluster.size * sizeof(Point));
                c1.points = new_c1;
                c2.size = 0;
                Point *new_c2 = (Point*)realloc(c2.points, cluster.size * sizeof(Point));
                c2.points = new_c2;
            }
        }
    }
    free(c1.points);
    free(c2.points);

    return divided_clusters;
}

// Función que encuentra el máximo entre dos doubles
double max(double i, double j) {
    return (i > j) ? i : j;
}

// Función que encuentra el mínimo entre dos doubles
double min(double i, double j) {
    return i < j ? i : j;
}

// Función que añade un Cluster a un ClusterArray
void addCluster(ClusterArray* C, Cluster* c) {
    if (C->size == 0) {
        C->clusters = (Cluster *)malloc(sizeof(Cluster));
        C->clusters[C->size] = *c;
        C->size++;
    }
    else {
        C->clusters = (Cluster *)realloc(C->clusters, (C->size + 1) * sizeof(Cluster));
        C->clusters[C->size] = *c;
        C->size++;
    }
}

// Función que elimina un cluster de un arreglo de clusters
void removeCluster(ClusterArray* C, int pos) {
    /* movemos los clusters de la derecha */
    for (int i = pos; i < C->size-1; i++) {
        C->clusters[i] = C->clusters[i+1];
    }
    /* redimensionamos el arreglo */
    C->clusters = realloc(C->clusters, (C->size - 1) * sizeof(Cluster));
    /* actualizamos el tamaño */
    C->size--;
}

// Función que añade una entrada a un nodo
void addEntryInNode(Node* N, Entry* e) {   
    if (N->num_entries == 0) {
        N->entries = (Entry *)malloc(sizeof(Entry));
        N->entries[N->num_entries] = *e;
        N->num_entries++;
    }
    else {
        N->entries = (Entry *)realloc(N->entries, (N->num_entries + 1) * sizeof(Entry));
        N->entries[N->num_entries] = *e;
        N->num_entries++;
    }
}

// Función que añade un punto a un cluster
void addPointInCluster(Cluster* C, Point* p) {
    if (C->size == 0) {
        C->points = (Point *)malloc(sizeof(Point));
        C->points[C->size] = *p;
        C->size++;
    }
    else {
        C->points = (Point *)realloc(C->points, (C->size + 1) * sizeof(Point));
        C->points[C->size] = *p;
        C->size++;
    }
}

// Función que retorna un Cluster con los puntos dentrode un arreglo de entradas
Cluster pointsInEntryArray(EntryArray E) {
    Point *C_points;
    Cluster C = {C_points, 0};
    for (int i = 0; i < E.size; i++) {
        Entry e = E.entries[i];
        Point e_point = e.p;
        addPointInCluster(&C, &e_point);
    }
    return C;
}

// Función que añade una entrada a un arreglo de entradas
void addEntryInEntryArray(EntryArray* E, Entry* e) {
    if (E->size == 0) {
        E->entries = (Entry *)malloc(sizeof(Entry));
        E->entries[E->size] = *e;
        E->size++;
    }
    else {
        E->entries = (Entry *)realloc(E->entries, (E->size + 1) * sizeof(Entry));
        E->entries[E->size] = *e;
        E->size++;
    }
}

// Función que verifica si un punto de una entrada está en un cluster
int isInCLuster(Entry e, Cluster c) {
    Point p = e.p;
    for (int i = 0; i < c.size; i++) {
        Point cluster_p = c.points[i];
        if (p.x == cluster_p.x && p.y == cluster_p.y) {
            return 1;
        }
    }
    return 0;
}

// Función que retorna un arreglo con las entradas que tengan un punto en el cluster dado
EntryArray entriesWithPointInCluster(EntryArray E, Cluster c) {
    Entry *new_E_entries;
    EntryArray new_E = {new_E_entries, 0};
    for (int i = 0; i < E.size; i++) {
        Entry e = E.entries[i];
        if (isInCLuster(e,c)) {
            addEntryInEntryArray(&new_E, &e);
        }
    }
    return new_E;
}

// Función que añade un arreglo de entradas a un arreglo de arreglos de entradas
void addEntryArrayInEntryArrayArray(EntryArrayArray* EE, EntryArray* E) {
    if (EE->size == 0) {
        EE->entries_array = (EntryArray *)malloc(sizeof(EntryArray));
        EE->entries_array[EE->size] = *E;
        EE->size++;
    }
    else {
        EE->entries_array = (EntryArray *)realloc(EE->entries_array, (EE->size + 1) * sizeof(EntryArray));
        EE->entries_array[EE->size] = *E;
        EE->size++;
    }
}

ClusterArray cluster(Cluster C_in) {
    /*
    for (size_t i = 0; i < C_in.size; i++){
        printf("Point %i: (%lf, %lf)\n", i + 1, C_in.points[i].x, C_in.points[i].y);
    }
    */
    if (C_in.size < b) {
        printf("El tamaño del set de puntos es menor a b.\n");
        exit(1);
    }
    /* 1. */
    printf("paso 1 cluster\n");
    Cluster *C_out_clusters;
    ClusterArray C_out = {C_out_clusters, 0};
    Cluster *C_clusters;
    ClusterArray C = {C_clusters, 0};
    /* 2. */
    printf("paso 2 cluster\n");
    for (int i = 0; i < C_in.size; i++) {
        /* añadir {p} a C */
        Point p = C_in.points[i];
        Point *pp = (Point *)malloc(sizeof(Point));
        pp[0] = p;
        Cluster C_p = {pp, 1}; // {p}
        addCluster(&C, &C_p);
    }
    /* 3. */
    printf("paso 3 cluster\n");
    /* -------------------------------------------------------ClusterArray C pareciera estar guardando bien los {p}-------------------------------------------------
    for (size_t i = 0; i < C.size; i++){
        printf("Point %i: (%lf, %lf)\n", i + 1, C.clusters[i].points[0].x, C.clusters[i].points[0].y);
    }
    */
    while (C.size > 1) { /*-------------------------------pareciera estar bien ahora---------------------------*/
        printf("%d\n", C.size);
        int* pos_C_mas_cercanos = closest_pair(&C); // muy ineficiente
        int pos_c1, pos_c2;
        if (C.clusters[pos_C_mas_cercanos[0]].size >= C.clusters[pos_C_mas_cercanos[1]].size){
            pos_c1 = pos_C_mas_cercanos[0];
            pos_c2 = pos_C_mas_cercanos[1];
        }
        else {
            pos_c1 = pos_C_mas_cercanos[1];
            pos_c2 = pos_C_mas_cercanos[0];
        }
        Cluster c1, c2;
        c1 = C.clusters[pos_c1];
        c2 = C.clusters[pos_c2];
        if ((c1.size + c2.size) <= B) {
            removeCluster(&C, pos_c1);
            removeCluster(&C, pos_c2);
            Cluster c_union = merge_clusters(c1,c2);
            addCluster(&C, &c_union);
        }
        else {
            removeCluster(&C, pos_c1);
            addCluster(&C_out, &c1);
        }
    }
    /*
    printf("%d\n", C.size);
    printf("%d\n", C_out.size);
    */
    /* 4. */
    printf("paso 4 cluster\n");
    Cluster c = C.clusters[0];
    /* 5. */
    printf("paso 5 cluster\n");
    Cluster c_prima;
    int pos_c_prima;
    if (C_out.size > 0) {
        pos_c_prima = closest_neighbor(c, C_out); 
        c_prima = C_out.clusters[pos_c_prima];
        removeCluster(&C_out, pos_c_prima);
    }
    else {
        Point *c_prima_points;
        Cluster c_prima = {c_prima_points, 0};
    }
    /* 6. */
    printf("paso 6 cluster\n");
    if ((c.size + c_prima.size) <= B) {
        /* añadimos (c U c_prima) a C_out*/
        Cluster c_union_prima = merge_clusters(c, c_prima);
        addCluster(&C_out, &c_union_prima);
    }
    else {
        Cluster c_union_prima = merge_clusters(c, c_prima);
        ClusterArray minMaxCluster = MinMaxSplitPolicy(c_union_prima);
        Cluster c1 = minMaxCluster.clusters[0];
        Cluster c2 = minMaxCluster.clusters[1];
        addCluster(&C_out, &c1);
        addCluster(&C_out, &c2);
    }
    /* 7. */
    printf("paso 7 cluster\n");
    return C_out;
}

Entry OutputHoja(Cluster C_in) {
    /* 1. */
    printf("paso 1 hoja\n");
    Point g = primary_medoid(&C_in);
    double r = 0;
    Entry *C_entries;
    Node C = {C_entries, 0};
    /* 2. */
    printf("paso 2 hoja\n");
    for (int i = 0; i < C_in.size; i++) {
        Point p = C_in.points[i];
        Entry new_entry = {p, 0.0, NULL};
        addEntryInNode(&C, &new_entry);
        r = max(r, euclidean_distance(g,p));
    }
    /* 3. */
    printf("paso 3 hoja\n");
    Node *a = &C;
    /* 4. */
    printf("paso 4 hoja\n");
    Entry out = {g, r, a};
    return out;
}

Entry OutputInterno(EntryArray C_mra) {
    // printf("%d\n", C_mra.size);
    /* 1. */
    printf("paso 1 interno\n");
    Cluster C_in = pointsInEntryArray(C_mra);
    Point G = primary_medoid(&C_in);
    double R = 0.0;
    Entry *C_entries;
    Node C = {C_entries, 0};
    /* 2. */
    printf("paso 2 interno\n");
    for (int i = 0; i < C_mra.size; i++) {
        Entry new_entry = C_mra.entries[i];
        addEntryInNode(&C, &new_entry);
        R = max(R, euclidean_distance(G,new_entry.p) + new_entry.cr);
    }
    /* 3. */
    printf("paso 3 interno\n");
    Node *A = &C;
    /* 4. */
    printf("paso 4 interno\n");
    Entry out = {G, R, A};
    return out;
}

Node *sextonSwinbank(Point* P, int P_size) {
    Cluster C_in = {P, P_size};
    /* 1. */
    printf("paso 1 ss\n");
    if (C_in.size <= B) {
        Entry res = OutputHoja(C_in);
        return res.a;
    }
    /* 2. */
    printf("paso 2 ss\n");
    ClusterArray C_out = cluster(C_in);
    // printf("%d\n", C_out.size);
    /*
    for (int i=0; i < C_out.size; i++) {
        printf("%d\n", C_out.clusters[2].size);
    }
    */
    Entry *C_entries;
    EntryArray C = {C_entries, 0};
    /* 3. */
    printf("paso 3 ss\n");
    for (int i = 0; i < C_out.size; i++) {
        Cluster c = C_out.clusters[i];
        Entry hoja_c = OutputHoja(c);
        addEntryInEntryArray(&C, &hoja_c);
    }

    /* 4. */
    printf("paso 4 ss\n");
    while (C.size > B) {
        /* 4.1 */
        printf("paso 4.1 ss\n");
        Cluster C_in = pointsInEntryArray(C);
        ClusterArray C_out = cluster(C_in);
        EntryArray *C_mra_array_entries;
        EntryArrayArray C_mra = {C_mra_array_entries, 0};
        /* 4.2 */
        printf("paso 4.2 ss\n");
        for (int i = 0; i < C_out.size; i++) {
            Cluster c = C_out.clusters[i];
            EntryArray s = entriesWithPointInCluster(C, c);
            if (s.size != 0) {
                addEntryArrayInEntryArrayArray(&C_mra, &s);
            }
        }
        /* 4.3 */
        printf("paso 4.3 ss\n");
        Entry *C_entries;
        EntryArray C = {C_entries, 0};
        /* 4.4 */
        printf("paso 4.4 ss\n");
        for (int i = 0; i < C_mra.size; i++) {
            EntryArray s = C_mra.entries_array[i];
            Entry interno_s = OutputInterno(s);
            addEntryInEntryArray(&C, &interno_s);
        }
    }
    /* 5. */
    printf("paso 5 ss\n");
    Entry res = OutputInterno(C);
    /* 6. */
    printf("paso 6 ss\n");
    return res.a;
}