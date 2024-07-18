#include "dijkstra.hpp"
#include "heap-dijkstra.cpp"
#include "fibonacci-dijkstra.cpp"
#include <fstream>

int main() {
    
    // Inicializar la semilla para rand()
    srand(static_cast<unsigned int>(time(nullptr)));

    std::map<int, std::map<int, vector<double>>> heap_times, fibonacci_times;

    for (int i = 10; i <= 14; i += 2) {
        for (int j = 16; j <= 22; ++j) {
            heap_times[i][j].reserve(50);
            fibonacci_times[i][j].reserve(50);
        }
    }

    // Para ejecutar el algoritmo en cada par (i,j)
    for (int i = 10; i <= 14; i+=2) {
        for (int j = 16; j <= 22; j++) {
            cout << "--------------- Caso i=" << i << ", j=" << j << ": ---------------"<< std::endl;
            for (int k = 0; k < 50; k++) { // k < 1 para testear como funciona, debería ser k < 50
                cout << "----- Iteración " << k+1 << std::endl;
                int v = pow(2, i);
                int e = pow(2, j);
                int raiz = getRandomInt(v);
                Graph graph = generateRandomGraph(v, e);
                // graph.printGraph(); // Por si es necesario ver el grafo
                auto start1 = chrono::high_resolution_clock::now(); // Registro de tiempo de inicio heap
                dijkstraHeap(graph, raiz);
                auto end1 = chrono::high_resolution_clock::now();   // Registro de tiempo de finalización heap
                auto start2 = chrono::high_resolution_clock::now(); // Registro de tiempo de inicio fibonacci
                dijkstraFibonacci(graph, raiz);
                auto end2 = chrono::high_resolution_clock::now();   // Registro de tiempo de finalización fibonacci
                chrono::duration<double> duration1 = end1 - start1;   // Cálculo del tiempo transcurrido heap
                chrono::duration<double> duration2 = end2 - start2;   // Cálculo del tiempo transcurrido fibonacci
                cout << "Tiempo del algoritmo con heap: " << duration1.count() << " segundos" << std::endl;
                cout << "Tiempo del algoritmo con cola de fibonacci: " << duration2.count() << " segundos" << std::endl;
                heap_times[i][j].push_back(duration1.count());
                fibonacci_times[i][j].push_back(duration2.count());
            }
        }
    }

    // Crear y abrir un archivo de salida para heap-dijkstra
    std::ofstream outputFile("resultados.txt");

    if (!outputFile) {
        std::cerr << "Error al abrir el archivo para escritura" << std::endl;
        return 1;
    }

    // Escribir los resultados del mapa heap_times en el archivo
    outputFile << "Resultados del algoritmo con heap:" << std::endl;
    for (const auto& outerPair : heap_times) {
        outputFile << "i = " << outerPair.first << std::endl;
        for (const auto& innerPair : outerPair.second) {
            outputFile << "  j = " << innerPair.first << ": [ ";
            for (const auto& value : innerPair.second) {
                outputFile << value << " ";
            }
            outputFile << "]" << std::endl;
        }
    }

    // Escribir los resultados del mapa fibonacci_times en el archivo
    outputFile << "Resultados del algoritmo con cola de fibonacci:" << std::endl;
    for (const auto& outerPair : fibonacci_times) {
        outputFile << "i = " << outerPair.first << std::endl;
        for (const auto& innerPair : outerPair.second) {
            outputFile << "  j = " << innerPair.first << ": [ ";
            for (const auto& value : innerPair.second) {
                outputFile << value << " ";
            }
            outputFile << "]" << std::endl;
        }
    }

    // Cerrar el archivo
    outputFile.close();

    return 0;
}
