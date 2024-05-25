#include "fibonacci-dijkstra.cpp"
#include <bits/stdc++.h>

using namespace std;

struct Edge {
    int src, dest;
    double weight;
};

// Función para generar grafos sintéticos
vector<Edge> generateGraph(int v, int e) {
    vector<Edge> graph;

    // Generar árbol cobertor
    for (int i = 1; i < v; ++i) {
        Edge edge;
        edge.src = i;
        edge.dest = rand() % i; // Nodo aleatorio en [0, i-1]
        edge.weight = (double)(rand()) / RAND_MAX; // Peso aleatorio en (0, 1]
        graph.push_back(edge);
    }

    // Añadir aristas restantes
    for (int i = v - 1; i < e; ++i) {
        Edge edge;
        edge.src = rand() % v;
        edge.dest = rand() % v;
        edge.weight = (double)(rand()) / RAND_MAX; // Peso aleatorio en (0, 1]
        graph.push_back(edge);
    }

    return graph;
}

int main() {
    srand(time(0)); // Semilla aleatoria

    int v = 4; // Número de nodos
    int e = 7; // Número de aristas

    vector<Edge> graph = generateGraph(v, e);

    // Imprimir el grafo generado
    cout << "Grafo Generado:" << endl;
    for (const auto& edge : graph) {
        cout << edge.src << " -> " << edge.dest << " : " << edge.weight << endl;
    }

    return 0;
}