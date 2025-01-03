#include "ss.c"

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

    // ======================
    // Determinar tamano de B
    // ======================
    printf("Entry size: %i\n", sizeof(Entry));
    printf("B: %d\n\n", 4096 / sizeof(Entry));

    // =====================================================================================================
    // Creación set de puntos aleatorios para cada n entre 2**10 y 2**25 y set de consultas Q con 100 puntos
    // =====================================================================================================

    // Seteo de semilla para el generador de números aleatorios
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

    // Imprimimos 5 puntos para probar que funcionó
    for (int i = 0; i < 5; i++) {
        printf("Query %d - Point: (%lf, %lf)\n", i + 1, Q[i].q.x, Q[i].q.y);
    }

    // Arreglo de cantidades de puntos para testing de los métodos
    int point_nums[16];
    for (int i = 0; i < 16; i++) {
        point_nums[i] = power_of_two(i + 10);
    }

    // Imprimimos primera cantidad para probar que funcionó
    printf("Array %i size: %i\n", 1, point_nums[0]);

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

    // Imprimimos 5 puntos del primer arreglo para probar que funcionó
    for (int i = 0; i < 5; i++) {
        printf("Point %i: (%lf, %lf)\n", i + 1, P[0][i].x, P[0][i].y);
    }

    // =======
    // Testing
    // =======

    // 1. Sexton Swinbank
    // Arreglo con accesos a disco de cada número de puntos
    int ss_disk_acceses[16];

    // Iteramos en cada conjunto con las 100 consultas y almacenamos accesos
    printf("Begin experiment\n");
    printf("Begin ss algorithm experiments\n");
    for (int i = 0; i < 1; i++) { // Este ciclo for se debe modificar si se quieren realizar experimentos con más puntos
        Node *ss_tree = sextonSwinbank(P[i], point_nums[i]);
        int acceses = 0;
        for (int j = 0; j < 100; j++) {
            Point *search = search_points_in_radio(ss_tree, Q[j], &acceses);
        }
        ss_disk_acceses[i] = acceses;
    }
    printf("Passed ss algorithm\n\n");
    
    // 2. Ciaccia Patella
    // Arreglo con accesos a disco de cada número de puntos
    int cp_disk_acceses[16];

    // Iteramos en cada conjunto con las 100 consultas y almacenamos accesos
    
    printf("Begin cp algorithm experiments\n");
    for (int i = 0; i < 1; i++) { // Este ciclo for se debe modificar si se quieren realizar experimentos con más puntos
        Node *cp_tree = ciacciaPatella(P[i], point_nums[i]);
        int acceses = 0;
        for (int j = 0; j < 100; j++) {
            Point *search = search_points_in_radio(cp_tree, Q[j], &acceses);
        }
        cp_disk_acceses[i] = acceses;
    }
    printf("Passed cp algorithm\n\n");
    
    printf("End experiment\n\n");

    // Imprimimos accesos de cada conjunto de puntos
    for (int i = 0; i < 1; i++) {
        printf("CP acceses for set %i: %i\n", i + 1, cp_disk_acceses[i]);
        printf("SS acceses for set %i: %i\n", i + 1, ss_disk_acceses[i]);
    }

    // Liberamos memoria de cada arreglo
    for (int i = 0; i < 16; i++) {
        free(P[i]);
    }
    
    return 0;
}

