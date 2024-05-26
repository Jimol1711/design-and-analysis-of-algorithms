#include "heap-dijkstra.cpp"

// Grafo
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

    int getNumVertices() const {
        return numVertices;
    }
};

// Heap de Fibonacci
class FibHeap {

    public:
        // Nodo del Heap de Fibonacci
        class FibNode {
            public:

                double key; // Clave del nodo
                bool mark;  // Booleano que indica si un nodo perdió un hijo y no es raíz (Se usa en el cut)
                FibNode *parent; // Puntero al padre
                FibNode *left;  // Puntero al nodo izquierdo en la lista enlazada
                FibNode *right; // Puntero al nodo derecho en la lista enlazada
                FibNode *child; // Puntero a un hijo
                int degree; // Grado del nodo (Cantidad de hijos)
                void *payload; // Puntero a información adicional (En nuestro caso el par (distancia, nodo) de Dijkstra)

                // Constructor del Nodo del Heap de Fibonacci
                FibNode(double key, void *pl) {
                    this->key = key;
                    this->mark = false;
                    this->parent = nullptr;
                    this->left = this;
                    this->right = this;
                    this->child = nullptr;
                    this->degree = 0;
                    this->payload = pl;
                }

                // Destructor
                ~FibNode() {

                }

        };

        int n; // Cantidad de nodos en el heap
        FibNode *min; // Puntero al nodo con la menor clave

        // Constructor del Heap de Fibonacci
        FibHeap()
            : n(0), min(nullptr) {
        }
        
        /* Inserción de un nodo en el Heap de Fibonacci
        *
        * insert(x)
        * 1. x.degree = 0
        * 2. x.p = NIL
        * 3. x.child = NIL
        * 4. x.mark = FALSE
        * 5. if H.min == NIL
        * 6. 	create a root list for H containing just x
        * 7. 	H.min = x
        * 8. else insert x into H's root list
        * 9. 	if x.key < H.min.key
        *10. 		H.min = x
        *11. H.n = H.n + 1
        *
        */
        void insert(FibNode *x) {

            // 1
            x->degree = 0;
            // 2
            x->parent = nullptr;
            // 3
            x->child = nullptr;
            // 4
            x->mark = false;

            // 5
            if (min == nullptr) {
                // 6 y 7
                x->left = x;
                x->right = x;
                min = x;
            // 8
            } else {
                min->left->right = x;
                x->left = min->left;
                x->right = min;
                min->left = x;

                // 9
                if (x->key < min->key) {
                    // 10
                    min = x;
                }
            }

            // 11
            n++;
        }

    // WIP...

};

// Algoritmo de Dijkstra con Heap de Fibonacci
void dijkstraFibonacci(Graph graph, int raiz) {

    double INF = std::numeric_limits<double>::infinity();

    int numVertices = graph.getNumVertices(); //Warning erróneo de VSCode

    // Parte 1.
    vector <double> distancias(numVertices, INF);
    vector <int> previos(numVertices, -1);

    // WIP...
}