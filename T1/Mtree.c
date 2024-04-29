#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define B 100 // Si B no es fijo, esto debe cambiar (aun no caxo bien como se calcula xd)

// Estructura de un punto
typedef struct {
    double x, y;
} Punto;


// Estructura de una entrada (entry)
typedef struct {
    Punto p; // punto
    double cr; // radio cobertor
    Node *a; // dir. en disco a la pagina de su hijo
} Entry;


// Estructura de un nodo. 
typedef struct {
    Entry entrys[B]; // está bien este 'B' ?
} Node;


// Estructura de una Query para busqueda de entrys en un nodo
typedef struct {
    Punto q;
    double r;
} Query;


// Probablemente para mas adelante...
typedef struct {
    ;
} Mtree;


// Función que crea un nodo.
// hay que ver bien si es correcto que parta con ese tamaño o parte de tamaño 0 y aumenta al agregarle entradas.
Node* create_node() {
    Node* node = (Node*)malloc(sizeof(Node));
    return node;
}


// Función que dice si un nodo es una hoja o no.
// Hay que revisar bien la condicion de cuándo un nodo es hoja, quizas la condicion podría relajarse.
int is_leaf(Node* node) {
    int num_entrys = sizeof(node) / sizeof(Entry); // Número de entradas del nodo
    Entry* entrys = node->entrys; // Arreglo de Entry del nodo
    // Para cada entrada, si alguna tiene 'cr' o 'a' no nulos, entonces no es una hoja
    for (int i=0; i < num_entrys; i++) {
        if (entrys[i].cr != 0.0 || entrys[i].a != NULL)
            return 0;
    }
    return 1;
}


// Función auxiliar que agrega las soluciones (puntos) que cumplen la condicion de una entrada de un nodo
void range_search(Node* node, Query Q, Punto** arreglo, int* tamaño) {
    Punto q = Q.q; // punto de la query
    double r = Q.r; // radio de la query
    int num_entrys = sizeof(node) / sizeof(Entry); // numero de entradas en el nodo
    Entry* entrys = node->entrys; // arreglo de entradas del nodo

    // Si el nodo es hoja, buscamos en cada entrada si cumplen con la condicion.
    // si la entrada la cumple, aumentamos el largo del arreglo y agregamos el punto de la entrada.
    if (is_leaf(node)) {
        for (int i=0; i<num_entrys; i++) {
            if(euclidian_distance(entrys[i].p, q) <= r) {
                *arreglo = (Punto*)realloc(*arreglo, (*tamaño) + 1 * sizeof(Punto));
                (*arreglo)[*tamaño] = q;
                (*tamaño)++;
            }
        }
    }
    // Si no es hoja, para cada entrada, si cumple la condicion de distancia bajamos a revisar en su nodo hijo 'a'
    else {
        for (int i=0; i<num_entrys; i++) {
            if(euclidian_distance(entrys[i].p, q) <= entrys[i].cr + r)
                range_search(entrys[i].a, Q, arreglo, tamaño);
        }
    }
}


// Función que busca los puntos que viven dentro de la bola especificada en la query.
Punto* search_points_in_radio(Node* node, Query Q) {
    Punto* arreglo = NULL; // inicializamos arreglo de puntos como nulo
    int tamaño = 0; // el tamaño del arreglo comienza en cero (no hay puntos inicialmente)

    range_search(node, Q, &arreglo, &tamaño); // ocupamos la funcion auxiliar que hace todo el trabajo
    return arreglo; // retornamos el arreglo con los puntos encontrados que cumplian la condicion
    
}


// Funcioón que calcula la distancia euclidiana entre dos puntos
double euclidian_distance(Punto p1, Punto p2) {
    return sqrt(pow(p2.x - p1.x, 2) + pow(p2.y - p1.y, 2));
}





void main() {
    printf("%s\n", "hola");
}