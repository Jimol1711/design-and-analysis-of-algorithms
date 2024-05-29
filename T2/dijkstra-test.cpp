#include "dijkstra.hpp"
#include "heap-dijkstra.cpp"
#include "fibonacci-dijkstra.cpp"

int main() {

    // Inicializar la semilla para rand()
    srand(static_cast<unsigned int>(time(nullptr)));

    // Para ejecutar el algoritmo en cada par (i,j)
    for (int i = 10; i <= 14; i+=2) {
        for (int j = 16; j <= 22; j++) {
            cout << "--------------- Caso i=" << i << ", j=" << j << ": ---------------"<< std::endl;
            for (int k = 0; k < 1; k++) { // k < 1 para testear como funciona, debería ser k < 50
                cout << "----- Iteración " << k+1 << std::endl;
                int v = pow(2, i);
                int e = pow(2, j);
                int raiz = getRandomInt(v);
                Graph graph = generateRandomGraph(v, e);
                // graph.printGraph(); // Por si es necesario ver el grafo
                auto start1 = chrono::high_resolution_clock::now(); // Registro de tiempo de inicio heap
                //dijkstraHeap(graph, raiz);
                auto end1 = chrono::high_resolution_clock::now();   // Registro de tiempo de finalización heap
                auto start2 = chrono::high_resolution_clock::now(); // Registro de tiempo de inicio fibonacci
                //dijkstraFibonacci(graph, raiz);
                auto end2 = chrono::high_resolution_clock::now();   // Registro de tiempo de finalización fibonacci
                chrono::duration<double> duration1 = end1 - start1;   // Cálculo del tiempo transcurrido heap
                chrono::duration<double> duration2 = end2 - start2;   // Cálculo del tiempo transcurrido fibonacci
                cout << "Tiempo del algoritmo con heap: " << duration1.count() << " segundos" << std::endl;
                cout << "Tiempo del algoritmo con cola de fibonacci: " << duration2.count() << " segundos" << std::endl;
            }
        }
    }

    return 0;
}