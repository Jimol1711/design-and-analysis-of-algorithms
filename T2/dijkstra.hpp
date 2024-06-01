#ifndef DIJKSTRA_HPP
#define DIJKSTRA_HPP

#include <iostream>
#include <vector>
#include <string>
#include <limits>
#include <random>
#include <set>
#include <algorithm>
#include <cmath>
#include <utility>
#include <chrono>
#include <stack>
#include <bits/stdc++.h>

using namespace std;

// Función para obtener un entero aleatorio en el rango [0, v-1]
int getRandomInt(int v) {
    auto seed = std::chrono::system_clock::now().time_since_epoch().count();
    static std::mt19937 gen(seed);
    std::uniform_int_distribution<> dis(0, v - 1);
    return dis(gen);
}

// Función para generar un peso aleatorio en el rango (0, 1]
double getRandomWeight() {
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    static thread_local std::mt19937 gen(seed); 
    std::uniform_real_distribution<> dis(std::nextafter(0.0, 1.0), 1.0); 
    return dis(gen);
}

struct HeapNode {
    double distance;
    int node;

    HeapNode(double d, int n) : distance(d), node(n) {} // constructor
};

struct Vertex {
    vector<pair<int, double>> neighbors;
    void* nodeInHeap; // Puntero al nodo en el heap
};

class Graph {

private:

    int numVertices;

public:
    vector<Vertex> adjList;

    // Constructor
    Graph(int numVertices) : numVertices(numVertices) {
        adjList.resize(numVertices);
    }

    // Añadir una arista con un peso
    void addEdge(int u, int v, double weight) {
        adjList[u].neighbors.emplace_back(v, weight);
        adjList[v].neighbors.emplace_back(u, weight);  // No dirigido
    }

    // Mostrar el grafo
    void printGraph() const {
        for (int i = 0; i < 5; ++i) {   // i < 5 para que solo imprima los primeros 5 nodos
            cout << i + 1 << ": ";
            for (const auto& neighbor : adjList[i].neighbors) {
                cout << "(" << neighbor.first + 1 << ", " << neighbor.second << ") ";
            }
            cout << endl;
        }
    }

    int getNumVertices() {
        return numVertices;
    }
};

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
    std::shuffle(possibleEdges.begin(), possibleEdges.end(), mt19937(random_device()()));

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

#endif