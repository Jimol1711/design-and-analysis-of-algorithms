#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "mtree.c"
#include "cp.c"
#include "ss.c"

// De aquí en adelante es mtree-test.c
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

    // Para compilar: gcc mtree.c -o mtree -lm
    // Para ejecutar: ./mtree

    // Test para la búsqueda (crear árbol y query, ver que devuelva los puntos en la query)

    // Test de cp (contar accesos a disco en árbol construido con cp para n puntos, con n
    // entre 2**10 y 2**25)

    // Test de ss (contar accesos a disco en árbol construido con ss para n puntos, con n
    // entre 2**10 y 2**25)

    // ======================
    // Determinar tamano de B
    // ======================
    printf("tamano de entrada: %i\n", sizeof(Entry));
    printf("tamano de B sería: %d\n", 4096 / sizeof(Entry));

    // =====================================================================================================
    // Creación set de puntos aleatorios para cada n entre 2**10 y 2**25 y set de consultas Q con 100 puntos
    // =====================================================================================================

    // Seed the random number generator
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

    // Imprimimos para probar que funcionó
    for (int i = 0; i < 100; i++) {
        printf("Query %d - Point: (%lf, %lf)\n", i + 1, Q[i].q.x, Q[i].q.y);
    }

    // Puntos para testing de los métodos
    int point_nums[16];
    for (int i = 0; i < 16; i++) {
        point_nums[i] = power_of_two(i + 10);
    }

    // Imprimimos para probar que funcionó
    for (int i = 0; i < 16; i++) {
        printf("Array %i size: %i\n", i+1, point_nums[i]);
    }

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

    // Imprimimos para probar que funcionó
    for (int i = 0; i < point_nums[0]; i++) {
        printf("Point %i: (%lf, %lf)\n", i + 1, P[0][i].x, P[0][i].y);
    }

    // Liberamos memoria de cada arreglo
    for (int i = 0; i < 16; i++) {
        free(P[i]);
    }
    
    return 0;
}

