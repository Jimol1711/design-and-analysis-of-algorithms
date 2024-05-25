#include "fibonacci-dijkstra.cpp"
#include <bits/stdc++.h>

using namespace std;

int getRandomInt(int v) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, v - 1);
    return dis(gen);
}

class Graph {
private:
    vector<vector<pair<int, double>>> adjList;
    int numVertices;

public:
    // Constructor
    Graph(int numVertices) : numVertices(numVertices) {
        adjList.resize(numVertices);
    }

    // Añadir una arista con un peso
    void addEdge(int u, int v, double weight) {
        adjList[u].emplace_back(v, weight);
        adjList[v].emplace_back(u, weight);  // No dirigido
    }

    // Mostrar el grafo
    void printGraph() const {
        for (int i = 0; i < 5; ++i) {   // i < 5 para que solo imprima los primeros 5 nodos
            cout << i << ": ";
            for (const auto& neighbor : adjList[i]) {
                cout << "(" << neighbor.first << ", " << neighbor.second << ") ";
            }
            cout << endl;
        }
    }
};

// Función para generar un peso aleatorio en el rango (0, 1]
double getRandomWeight() {
    static mt19937 gen(random_device{}());
    static uniform_real_distribution<> dis(0.0001, 1.0);
    return dis(gen);
}

// Función para generar un grafo aleatorio conexo
Graph generateRandomGraph(int v, int e) {
    Graph graph(v);
    set<pair<int, int>> existingEdges; // Para no repetir aristas

    // Crear un árbol cobertor mínimo para garantizar la conectividad
    for (int i = 1; i < v; ++i) {
        int randomNode = rand() % i;
        double weight = getRandomWeight();
        graph.addEdge(i, randomNode, weight);
        existingEdges.emplace(min(i, randomNode), max(i, randomNode));
    }

    // Crear una lista de todas las posibles aristas adicionales
    vector<pair<int, int>> possibleEdges;
    for (int i = 0; i < v; ++i) {
        for (int j = i + 1; j < v; ++j) {
            if (existingEdges.find({i, j}) == existingEdges.end()) {
                possibleEdges.emplace_back(i, j);
            }
        }
    }

    // Mezclar las aristas posibles para extraerlas luego aleatoriamente
    shuffle(possibleEdges.begin(), possibleEdges.end(), mt19937(random_device()()));

    // Añadir las aristas restantes hasta alcanzar e aristas en total
    int additionalEdges = e - (v - 1);
    for (int i = 0; i < additionalEdges && i < possibleEdges.size(); ++i) {
        int u = possibleEdges[i].first;
        int v = possibleEdges[i].second;
        double weight = getRandomWeight();
        graph.addEdge(u, v, weight);
        existingEdges.emplace(min(u, v), max(u, v));
    }

    return graph;
}

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
