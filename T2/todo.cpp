#include "dijkstra.hpp"

/*
 * Clase que representa un Heap de Fibonacci
 */
class FibHeap {

    public:

        /*
         * Clase que representa un Nodo del Heap de Fibonacci
         */        
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

        /*
         * Función que inserta un nodo en el Heap de Fibonacci
         */
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

        /*
         * Función que extrae el nodo con la menor clave del Heap de Fibonacci
         */
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

        /*
         * Función que consolida los nodos del Heap de Fibonacci
         */
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

        /*
         * Función que enlaza dos nodos en el Heap de Fibonacci
         */
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

        /*
         * Función que disminuye la clave de un nodo en el Heap de Fibonacci
         */
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

        /*
         * Función que corta un nodo del Heap de Fibonacci
         */
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

        /*
         * Función que realiza un corte en cascada en el Heap de Fibonacci
         */
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

        /*
         * Función que verifica si el Heap de Fibonacci está vacío
         */
        bool isEmpty() const {
            return min == nullptr;
        }

        /*
         * Función que encuentra el nodo con la menor clave en el Heap de Fibonacci
         */
        FibNode* findMin() const {
            return min;
        }

        /*
         * Función que elimina un nodo del Heap de Fibonacci
         */
        void deleteNode(FibNode* x) {
            decreaseKey(x, std::numeric_limits<double>::lowest());
            extractMin();
        }

};

/*
 * Función que implementa el algoritmo de Dijkstra con cola de Fibonacci
 */
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
    cout << "Distancias desde el nodo " << start + 1 << ":\n";
    for (int i = 0; i < n; ++i) {
        cout << "Distancia al nodo " << i + 1 << ": " << dist[i] << "\n";
    }
    cout << endl;

    cout << "Camino más corto desde el nodo " << start + 1 << ":\n";
    for (int i = 0; i < n; ++i) {
        if (dist[i] < numeric_limits<double>::infinity()) {
            cout << "Camino hasta el nodo " << i + 1 << ": ";
            std::stack<int> nodes;
            for (int v = i; v != -1; v = prev[v]) {
                nodes.push(v + 1);
            }
            while (!nodes.empty()) {
                cout << nodes.top() << " ";
                nodes.pop();
            }
            cout << "\n";
        }
    } 
    #endif
}

int main() {

    // Inicializar la semilla para rand()
    srand(static_cast<unsigned int>(time(nullptr)));

    // Ejemplo de uso:
    int numVertices = pow(2, 10);
    int numEdges = pow(2, 16);

    Graph graph = generateRandomGraph(numVertices, numEdges);
    graph.printGraph();

    int rootNode = getRandomInt(numVertices);
    auto start1 = chrono::high_resolution_clock::now();
    dijkstraFibonacci(graph, rootNode);
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
