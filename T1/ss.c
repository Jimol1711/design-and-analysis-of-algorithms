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
        // revisar c1.size >= c2.size
        Cluster c1 = C_mas_cercanos.clusters[0];
        Cluster c2 = C_mas_cercanos.clusters[1];
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
    Cluster c = C.clusters[0]; // el último cluster debería quedar en la posición 0
    /* 5. */
    Cluster c_prima;
    if (C_out.size > 0) {
        c_prima = vecinoMasCercano(C_out, c); 
        removeCluster(C_out, c_prima);
    }
    else {
        Point *p;
        Cluster c_prima = {p, 0}; // revisar esto por el primer 0
    }
    /* 6. */
    if ((c.size + c_prima.size) <= B) {
        /* añadimos (c U c_prima) a C_out*/
        Cluster c_union = mergeCluster(c, c_prima);
        addCluster(C_out, c_union);
    }
    else {
        ClusterArray minMaxCluster = MinMaxSplitPolicy(c, c_prima);
        Cluster c1 = minMaxCluster.clusters[0];
        Cluster c2 = minMaxCluster.clusters[1];
        addCluster(C_out, c1);
        addCluster(C_out, c2);
    }
    return C_out;
}

Entry OutputHoja(Cluster C_in) {
    Point g = primaryMedoid(C_in);
    double r = 0;
    // falta inicializar bien el nodo
    Node C = {}; // creo que aquí hay que de todas formal pasarle un puntero y 0
    for (int i = 0; i < C_in.size; i++) {
        Point p = C_in.points[i];
        Entry new_entry = {p, 0., NULL};
        /* addEntry(Node N, Entry E) */
        C.entries[i] = new_entry;
        C.num_entries++;
        r = max(r, euclidian_distance(g,p));
    }
    Node *a = &C;
    Entry out = {g, r, a};
    return out;
}

Entry OutputInterno(EntryArray C_mra) {
    Cluster C_in = pointsInEntryArray(C_mra);
    Point G = primaryMedoid(C_in);
    double R = 0.;
    // inicializar bien el nodo
    Node C = {};
    for (int i = 0; i < C_mra.size; i++) {
        Entry t = C_mra.entries[i];
        // Añadir C_mra.entries[i] en C
        R = max(R, euclidian_distance(G,t.p) + t.cr);
    }
    Node *A = &C;
    Entry out = {G, R, A};
    return out;
}

Node *AlgoritmoSS(Cluster C_in) {
    if (C_in.size <= B) {
        Entry res = OutputHoja(C_in);
        return res.a;
    }
    ClusterArray C_out = cluster(C_in);
    EntryArray C = {}; // C es un conjunto de entries
    for (int i = 0; i < C_out.size; i++) {
        Cluster c = C_out.clusters[i];
        Entry t = OutputHoja(c);
        // agregar OutputHoja(c) a C
    }
    while (C.size > B) {
        // 4.1
        /* creo que esto no va
        Cluster C_in = pointsInEntryArray(C);
        ClusterArray C_out = cluster(C_in);
        */
        EntryArray C_mra = {};
        //4.2
        for (int i = 0; i < C_out.size; i++) {
            Cluster c = C_out.clusters[i];
            // obtener s y añadirlo a C_mra
        }
        //4.3
        EntryArray C = {};
        //4.4
        for (int i = 0; i < C_mra.size; i++) {
            Entry s = C_mra.entries[i];
            // agregar OutputInterno(s) a C
        }
    }
    Entry c = OutputInterno(C);
    return c.a;
}
