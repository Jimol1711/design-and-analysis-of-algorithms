#include <stdio.h>
#include <stdlib.h>

#include "mtree.h"

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

void addCluster(ClusterArray C, Cluster c) {
    if (C.size == 0) {
        C.clusters = (Cluster *)malloc(sizeof(Cluster));
        C.clusters[C.size] = c;
        C.size++;
    }
    else {
        C.clusters = (Cluster *)realloc(C.clusters, (C.size + 1) * sizeof(Cluster));
        C.clusters[C.size] = c;
        C.size++;
    }
}
/* FALTA IMPLEMENTAR ESTA FUNCIÓN */
void removeCluster(ClusterArray C, Cluster c) {
    ;
}

ClusterArray cluster(Cluster C_in) {
    if (C_in.size < b) {
        printf("El tamaño del set de puntos es menor a b.\n");
        exit(1);
    }
    /* 1. */
    Cluster *C_out_clusters;
    ClusterArray C_out = {C_out_clusters, 0};
    Cluster *C_clusters;
    ClusterArray C = {C_clusters, 0};
    /* 2. */
    for (int i = 0; i < C_in.size; i++) {
        /* añadir {p} a C */
        Point p = C_in.points[i];
        Point *pp = (Point *)malloc(sizeof(Point));
        pp[0] = p;
        Cluster C_p = {pp, 1}; // {p}
        addCluster(C, C_p);
    }
    /* 3. */
    while (C.size > 1) {
        ClusterArray C_mas_cercanos = clustersMasCercanos(C);
        Cluster c1, c2;
        if (C_mas_cercanos.clusters[0].size >= C_mas_cercanos.clusters[1].size){
            Cluster c1 = C_mas_cercanos.clusters[0];
            Cluster c2 = C_mas_cercanos.clusters[1];
        }
        else {
            Cluster c1 = C_mas_cercanos.clusters[1];
            Cluster c2 = C_mas_cercanos.clusters[0];
        }
        if ((c1.size + c2.size) <= B) {
            removeCluster(C, c1);
            removeCluster(C, c2);
            Cluster c_union = unionCluster(c1,c2);
            addCluster(C, c_union);
        }
        else {
            removeCluster(C, c1);
            addCluster(C_out, c1);
        }
    }
    /* 4. */
    Cluster c = C.clusters[0];
    /* 5. */
    Cluster c_prima;
    if (C_out.size > 0) {
        c_prima = vecinoMasCercano(C_out, c); 
        removeCluster(C_out, c_prima);
    }
    else {
        Point *c_prima_points;
        Cluster c_prima = {c_prima_points, 0};
    }
    /* 6. */
    if ((c.size + c_prima.size) <= B) {
        /* añadimos (c U c_prima) a C_out*/
        Cluster c_union_prima = mergeCluster(c, c_prima);
        addCluster(C_out, c_union_prima);
    }
    else {
        ClusterArray minMaxCluster = minMaxSplitPolicy(c, c_prima);
        Cluster c1 = minMaxCluster.clusters[0];
        Cluster c2 = minMaxCluster.clusters[1];
        addCluster(C_out, c1);
        addCluster(C_out, c2);
    }
    /* 7. */
    return C_out;
}

Entry OutputHoja(Cluster C_in) {
    /* 1. */
    Point g = primaryMedoid(C_in);
    double r = 0;
    /* FALTA DEFINIR C_entries*/
    Node C = {C_entries, 0};
    /* 2. */
    for (int i = 0; i < C_in.size; i++) {
        Point p = C_in.points[i];
        Entry new_entry = {p, 0., NULL};
        /* addEntryInNode(Node N, Entry E) */
        addEntryInNOde(C, new_entry);
        r = max(r, euclidian_distance(g,p));
    }
    /* 3. */
    Node *a = &C;
    /* 4. */
    Entry out = {g, r, a};
    return out;
}

Entry OutputInterno(EntryArray C_mra) {
    /* 1. */
    Cluster C_in = pointsInEntryArray(C_mra);
    Point G = primaryMedoid(C_in);
    double R = 0.;
    /* FALTA DEFINIR C_entries*/
    Node C = {C_entries, 0};
    /* 2. */
    for (int i = 0; i < C_mra.size; i++) {
        Entry new_entry = C_mra.entries[i];
        /* addEntryInNode(Node N, Entry E) */
        addEntryInNode(C, new_entry);
        R = max(R, euclidian_distance(G,new_entry.p) + new_entry.cr);
    }
    /* 3. */
    Node *A = &C;
    /* 4. */
    Entry out = {G, R, A};
    return out;
}

Node *AlgoritmoSS(Cluster C_in) {
    /* 1. */
    if (C_in.size <= B) {
        Entry res = OutputHoja(C_in);
        return res.a;
    }
    /* 2. */
    ClusterArray C_out = cluster(C_in);
    Entry *C_entries;
    EntryArray C = {C_entries, 0};
    /* 3. */
    for (int i = 0; i < C_out.size; i++) {
        Cluster c = C_out.clusters[i];
        Entry hoja_c = OutputHoja(c);
        /* addEntryInEntryArray(EntryArray E, Entry e) */
        addEntryInEntryArray(C, hoja_c);
    }
    /* 4. */
    while (C.size > B) {
        /* 4.1 */
        Cluster C_in = pointsInEntryArray(C);
        ClusterArray C_out = cluster(C_in);
        EntryArray *C_mra_array_entries;
        EntryArrayArray C_mra = {C_mra_array_entries, 0};
        /* 4.2 */
        for (int i = 0; i < C_out.size; i++) {
            Cluster c = C_out.clusters[i];
            /* entriesWithPointInCluster(Entry E, Cluster C) */
            EntryArray s = entriesWithPointInCluster(C, c);
            addEntryArrayInEntryArrayArray(C_mra, s);
        }
        /* 4.3 */
        Entry *C_entries;
        EntryArray C = {C_entries, 0};
        /* 4.4 */
        for (int i = 0; i < C_mra.size; i++) {
            EntryArray s = C_mra.entries_array[i];
            Entry interno_s = OutputInterno(s);
            addEntryInEntryArray(C, interno_s);
        }
    }
    /* 5. */
    Entry res = OutputInterno(C);
    /* 6. */
    return res.a;
}
