#include "fibonacci-dijkstra.cpp"
#include <bits/stdc++.h>

using namespace std;

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
        for (int i = 0; i < numVertices; ++i) {
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
    // Definir parámetros
    int i = 12;  // Puedes cambiar a 12 o 14
    int j = 16;  // Puedes cambiar a un valor entre 16 y 22

    // Número de nodos y aristas
    int v = pow(2, i);
    int e = pow(2, j);

    // Inicializar la semilla para rand()
    srand(static_cast<unsigned int>(time(nullptr)));

    // Generar el grafo
    Graph graph = generateRandomGraph(v, e);

    // Imprimir el grafo
    graph.printGraph();

    return 0;
}
