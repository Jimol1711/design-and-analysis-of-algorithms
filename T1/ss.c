#include <stdio.h>
#include <stdlib.h>

#include "mtree.h"

typedef struct {
    Point *points;
    Point g; // primary medoid
    int size;
} Cluster;

typedef struct {
    Cluster *clusters;
    int size;
} ClusterArray;

typedef struct {
    Point g; // primary medoid
    double r; // cover radio
    Node *a; // child node
} Tupla;

typedef struct {
    Tupla *tuplas;
    int size;
} TuplaArray;

ClusterArray cluster(Cluster C_in) {
    ClusterArray C_out = {};
    ClusterArray C = {};
    int C_out_size = 0;
    int C_size = 0;
    for (int i = 0; i < C_in.size; i++) {
        // añadir {p} a C
    }
    while (C_size > 1) {
        /* 
        1. Buscar los pares de clusters más cercanos tal que |c1| >= |c2|.
        2. if |c1 U c2| <= B, se remueven de C y se añade la unión a C.
        3. else, se remueve c1 de C y se añade a C_out.
         */
    }
    /* nos queda solo un elemento en C al salir del while, que sería c*/
    Cluster c = C.clusters[0];
    Cluster c_prima;
    if (C_out_size > 0) {
        /*
        c_prima = vecinoMasCercano(C_out, c); 
        removemos c_prima de vecinoMasCercano
        */
    }
    else {
        Cluster c_prima = {};
    }
    if ((c.size + c_prima.size) <= B) {
        /* añadimos (c U c_prima) a C_out*/
    }
    else {
        /*
        c1, c2 = MinMaxSplitPolicy(c, c_prima)
        agregamos c1 y c2 a C_out
        */
    }
    return C_out;
}

Tupla OutputHoja(Cluster C_in) {
    Point g = C_in.g;
    double r = 0;
    Node C = {};
    for (int i = 0; i < C_in.size; i++) {
        Point p = C_in.points[i];
        Entry new_entry = {p, 0, NULL};
        C.entries[i] = new_entry;
        C.num_entries++;
        r = max(r, euclidian_distance(g,p));
    }
    Node *a = &C;
    Tupla out = {g, r, a};
    return out;
}

Tupla OutputInterno(TuplaArray C_mra) {
    Cluster C_in = pointsInTuplaArray(C_mra);
    Point G = primaryMedoid(C_in);
    double R = 0.;
    Node C = {};
    for (int i = 0; i < C_mra.size; i++) {
        Tupla t = C_mra.tuplas[i];
        // Añadir C_mra.tuplas[i] en C
        R = max(R, euclidian_distance(G,t.g) + t.r);
    }
    Node *A = &C;
    Tupla out = {G, R, A};
    return out;
}

Node *AlgoritmoSS(Cluster C_in) {
    if (C_in.size <= B) {
        Tupla res = OutputHoja(C_in);
        return res.a;
    }
    ClusterArray C_out = cluster(C_in);
    TuplaArray C = {}; // C es un conjunto de tuplas
    for (int i = 0; i < C_out.size; i++) {
        Cluster c = C_out.clusters[i];
        Tupla t = OutputHoja(c);
        // agregar OutputHoja(c) a C
    }
    while (C.size > B) {
        // 4.1
        Cluster C_in = pointsInTuplaArray(C);
        ClusterArray C_out = cluster(C_in);
        TuplaArray C_mra = {};
        //4.2
        for (int i = 0; i < C_out.size; i++) {
            Cluster c = C_out.clusters[i];
            // obtener s y añadirlo a C_mra
        }
        //4.3
        TuplaArray C = {};
        //4.4
        for (int i = 0; i < C_mra.size; i++) {
            Tupla s = C_mra.tuplas[i];
            // agregar OutputInterno(s) a C
        }
    }
    Tupla c = OutputInterno(C);
    return c.a;
}
