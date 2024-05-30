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
#include <bits/stdc++.h>

using namespace std;

int getRandomInt(int v) {
    // Sembrar el generador de números aleatorios con el reloj del sistema
    auto seed = std::chrono::system_clock::now().time_since_epoch().count();
    static std::mt19937 gen(seed);
    std::uniform_int_distribution<> dis(0, v - 1);
    return dis(gen);
}

struct HeapNode { // esto debería tener un nombre generalizado para heap y cola de fibonacci
    double distance;
    int node;

    HeapNode(double d, int n) : distance(d), node(n) {} // constructor
};

struct Vertex {
    vector<pair<int, double>> neighbors;
    void* nodeInHeap; // Puntero al nodo en el heap
};

class Graph {
public:
    vector<Vertex> adjList;
    int numVertices;

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
            cout << i << ": ";
            for (const auto& neighbor : adjList[i].neighbors) {
                cout << "(" << neighbor.first << ", " << neighbor.second << ") ";
            }
            cout << endl;
        }
    }

    int getNumVertices() {
        return numVertices;
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

// Heap de Fibonacci
class FibHeap {

    public:
        // Nodo del Heap de Fibonacci
        class FibNode {

        public:

            bool mark;  // Booleano que indica si un nodo perdió un hijo y no es raíz (Se usa en el cut)
            FibNode* parent; // Puntero al padre
            FibNode* left;  // Puntero al nodo izquierdo en la lista enlazada
            FibNode* right; // Puntero al nodo derecho en la lista enlazada
            FibNode* child; // Puntero a un hijo
            int degree; // Grado del nodo (Cantidad de hijos)

            double key; // Clave del nodo
            std::pair<double, int> par; // Par (distancia, nodo) almacenado

            // Constructor del Nodo del Heap de Fibonacci
            FibNode(double key, int vertex) 
                : par(key, vertex) {
                this->key = key;
                this->mark = false;
                this->parent = nullptr;
                this->left = this;
                this->right = this;
                this->child = nullptr;
                this->degree = 0;
            }

            // Destructor
            ~FibNode() {

            }
        };

        int n; // Cantidad de nodos en el heap
        FibNode* min; // Puntero al nodo con la menor clave

        // Constructor del Heap de Fibonacci
        FibHeap()
            : n(0), min(nullptr) {
        }

        void insert(FibNode* x) {
            x->degree = 0;
            x->parent = nullptr;
            x->child = nullptr;
            x->mark = false;

            if (min == nullptr) {
                x->left = x;
                x->right = x;
                min = x;
            }
            else {
                min->left->right = x;
                x->left = min->left;
                x->right = min;
                min->left = x;

                if (x->key < min->key) {
                    min = x;
                }
            }
            n++;
        }

        FibNode* extractMin() {
            FibNode* z = min;
            if (z != nullptr) {
                FibNode* x = z->child;
                FibNode* tempRight;

                if (x != nullptr) {
                    FibNode* firstChild = x;
                    do {
                        tempRight = x->right;
                        min->left->right = x;
                        x->right = min;
                        x->left = min->left;
                        min->left = x;
                        x->parent = nullptr;
                        x = tempRight;
                    } while (x != firstChild);
                }

                z->left->right = z->right;
                z->right->left = z->left;

                if (z == z->right) {
                    min = nullptr;
                }
                else {
                    min = z->right;
                    consolidate();
                }
                n--;
            }
            return z;
        }

        void consolidate() {
            int max_degree = static_cast<int>(floor(log(static_cast<double>(n)) / log(static_cast<double>(1 + sqrt(static_cast<double>(5))) / 2)));
            FibNode** A = new FibNode * [max_degree + 2];
            std::fill_n(A, max_degree + 2, nullptr);

            FibNode* w = min;
            int rootSize = 0;
            FibNode* next = w;
            do {
                rootSize++;
                next = next->right;
            } while (next != w);

            FibNode** rootList = new FibNode * [rootSize];
            for (int i = 0; i < rootSize; i++) {
                rootList[i] = next;
                next = next->right;
            }

            for (int i = 0; i < rootSize; i++) {
                w = rootList[i];
                FibNode* x = w;
                int d = x->degree;

                while (A[d] != nullptr) {
                    FibNode* y = A[d];
                    if (y->key < x->key) {
                        FibNode* temp = x;
                        x = y;
                        y = temp;
                    }
                    fibHeapLink(y, x);
                    A[d] = nullptr;
                    d++;
                }
                A[d] = x;
            }
            delete[] rootList;

            min = nullptr;

            for (int i = 0; i < max_degree + 2; i++) {
                if (A[i] != nullptr) {
                    if (min == nullptr) {
                        min = A[i]->left = A[i]->right = A[i];
                    }
                    else {
                        min->left->right = A[i];
                        A[i]->left = min->left;
                        min->left = A[i];
                        A[i]->right = min;
                        if (A[i]->key < min->key) {
                            min = A[i];
                        }
                    }
                }
            }
            delete[] A;
        }

        void fibHeapLink(FibNode* y, FibNode* x) {
            y->left->right = y->right;
            y->right->left = y->left;

            if (x->child == nullptr) {
                y->left = y;
                y->right = y;
                x->child = y;
            }
            else {
                y->left = x->child;
                y->right = x->child->right;
                x->child->right->left = y;
                x->child->right = y;
            }
            y->parent = x;
            x->degree++;
            y->mark = false;
        }

        void decreaseKey(FibNode* x, double k) {
            if (k > x->key) {
                throw std::invalid_argument("New key is greater than current key");
            }

            x->key = k;
            FibNode* y = x->parent;

            if (y != nullptr && x->key < y->key) {
                cut(x, y);
                cascadingCut(y);
            }

            if (x->key < min->key) {
                min = x;
            }
        }

        void cut(FibNode* x, FibNode* y) {
            if (x->right == x) {
                y->child = nullptr;
            }
            else {
                x->left->right = x->right;
                x->right->left = x->left;
                if (y->child == x) {
                    y->child = x->right;
                }
            }
            y->degree--;
            min->left->right = x;
            x->right = min;
            x->left = min->left;
            min->left = x;
            x->parent = nullptr;
            x->mark = false;
        }

        void cascadingCut(FibNode* y) {
            FibNode* z = y->parent;
            if (z != nullptr) {
                if (y->mark == false) {
                    y->mark = true;
                }
                else {
                    cut(y, z);
                    cascadingCut(z);
                }
            }
        }

        bool isEmpty() const {
            return min == nullptr;
        }

        FibNode* findMin() const {
            return min;
        }

        void deleteNode(FibNode* x) {
            decreaseKey(x, std::numeric_limits<double>::lowest());
            extractMin();
        }

};

void dijkstraFibonacci(Graph& graph, int start) {

    int n = graph.getNumVertices();
    vector<double> dist(n, numeric_limits<double>::infinity());
    vector<int> prev(n, -1);
    FibHeap pq;

    vector<FibHeap::FibNode*> heapNodes(n, nullptr);

    dist[start] = 0;
    heapNodes[start] = new FibHeap::FibNode(0, start);
    pq.insert(heapNodes[start]);

    while (!pq.isEmpty()) {
        FibHeap::FibNode* minNode = pq.extractMin();
        int u = minNode->par.second;
        double d = minNode->par.first;

        for (const auto& neighbor : graph.adjList[u].neighbors) {
            int v = neighbor.first;
            double weight = neighbor.second;

            if (dist[u] + weight < dist[v]) {
                dist[v] = dist[u] + weight;
                prev[v] = u;

                if (heapNodes[v] == nullptr) {
                    heapNodes[v] = new FibHeap::FibNode(dist[v], v);
                    pq.insert(heapNodes[v]);
                }
                else {
                    pq.decreaseKey(heapNodes[v], dist[v]);
                }
            }
        }
    }

    #if 1
    cout << "Distancias desde el nodo " << start << ":\n";
    for (int i = 0; i < n; ++i) {
        cout << "Distancia al nodo " << i << ": " << dist[i] << "\n";
    }
    cout << endl;

    cout << "Camino más corto desde el nodo " << start << ":\n";
    for (int i = 0; i < n; ++i) {
        if (dist[i] < numeric_limits<double>::infinity()) {
            cout << "Camino hasta el nodo " << i << ": ";
            for (int v = i; v != -1; v = prev[v]) {
                cout << v << " ";
            }
            cout << "\n";
        }
    }
    #endif
}

int main() {

    // Inicializar la semilla para rand()
    // srand(static_cast<unsigned int>(time(nullptr)));

    // Ejemplo de uso:
    int numVertices = pow(2, 12);
    int numEdges = pow(2, 22);

    Graph graph = generateRandomGraph(numVertices, numEdges);
    // graph.printGraph();

    int startNode = getRandomInt(numVertices);
    auto start1 = chrono::high_resolution_clock::now();
    dijkstraFibonacci(graph, startNode);
    auto end1 = chrono::high_resolution_clock::now();
    chrono::duration<double> duration1 = end1 - start1;
    cout << "Tiempo del algoritmo con cola de fibonacci: " << duration1.count() << " segundos" << std::endl;

    #if 0
    for (int i = 10; i <= 14; i+=2) {
        for (int j = 16; j <= 22; j++) {
            cout << "--------------- Caso i=" << i << ", j=" << j << ": ---------------"<< std::endl;
            for (int k = 0; k < 1; k++) {
                cout << "----- Iteración " << k+1 << std::endl;
                int v = pow(2, i);
                int e = pow(2, j);
                Graph graph = generateRandomGraph(numVertices, numEdges);
            }
        }
    }
    #endif
}
