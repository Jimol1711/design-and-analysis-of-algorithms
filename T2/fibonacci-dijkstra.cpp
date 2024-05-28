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

                bool mark;  // Booleano que indica si un nodo perdió un hijo y no es raíz (Se usa en el cut)
                FibNode *parent; // Puntero al padre
                FibNode *left;  // Puntero al nodo izquierdo en la lista enlazada
                FibNode *right; // Puntero al nodo derecho en la lista enlazada
                FibNode *child; // Puntero a un hijo
                int degree; // Grado del nodo (Cantidad de hijos)

                double key; // Clave del nodo
                std::pair<double, int> par; // Par (distancia, nodo) almacenado

                // Constructor del Nodo del Heap de Fibonacci
                FibNode(double key, int vertex) 
                    : par(key, vertex) { // Actualizar para que vertex sea Vertex
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
        FibNode *min; // Puntero al nodo con la menor clave

        // Constructor del Heap de Fibonacci
        FibHeap()
            : n(0), min(nullptr) {
        }
        
        /* 
         * Inserción de un nodo en el Heap de Fibonacci
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

        /*
         * Extracción del nodo mínimo del Heap de Fibonacci
         *
         * FIB-HEAP-EXTRACT-MIN(H)
         * 1. z = H.min
         * 2. if z != NIL
         * 3. 	for each child x of z
         * 4. 		add x to the root list of H
         * 5. 		x.p = NIL
         * 6. 	remove z from the root list of H
         * 7.		if z == z.right
         * 8. 		H.min = NIL
         * 9. 	else H.min = z.right
         *10. 		CONSOLIDATE(H)
         *11. 	H.n = H.n - 1
         *12. return z
         *
         */
        FibNode *extractMin() {

            // 1
            FibNode *z = min;

            // 2
            if (z != nullptr) {
                FibNode *x = z->child;
                FibNode *tempRight;

                // 3
                if (x != nullptr) {
                    FibNode *firstChild = x;
                    do {
                        tempRight = x->right;

                        // 4
                        min->left->right = x;
                        x->right = min;
                        x->left = min->left;
                        min->left = x;

                        // 5
                        x->parent = nullptr;

                        x = tempRight;
                    } while (x != firstChild);
                }

                // 6
                z->left->right = z->right;
                z->right->left = z->left;

                // 7
                if (z == z->right) {
                    // 8
                    min = nullptr;
                // 9
                } else {
                    min = z->right;
                    // 10
                    consolidate();
                }

                // 11
                n--;
            }

            return z;
        }

        /*
         * Consolidación de los nodos del Heap de Fibonacci (Reducir el número de árboles)
         *
         * CONSOLIDATE(H)
         * 1. let A[0 . . D(H.n)] be a new array
         * 2. for i = 0 to D(H.n)
         * 3. 	A[i] = NIL
         * 4. for each node w in the root list of H
         * 5. 	x = w
         * 6. 	d = x.degree
         * 7. 	while A[d] != NIL
         * 8. 		y = A[d]
         * 9. 		if x.key > y.key
         *10.			exchange x with y
         *11. 		FIB-HEAP-LINK(H,y,x)
         *12. 		A[d] = NIL
         *13. 		d = d + 1
         *14. 	A[d] = x
         *15. H.min = NIL
         *16. for i = 0 to D(H.n)
         *17. 	if A[i] != NIL
         *18. 		if H.min == NIL
         *19. 			create a root list for H containing just A[i]
         *20. 			H.min = A[i]
         *21. 		else insert A[i] into H's root list
         *22. 			if A[i].key < H.min.key
         *23. 				H.min = A[i]
         *
         */
        void consolidate() {
                
            // 1, 2 y 3
            int Dn = static_cast<int>(log2(n)) + 1;
            vector<FibNode *> A(Dn, nullptr);

            // 4
            FibNode *w = min;
            FibNode *x, *y;
            int d;

            do {
                // 5
                x = w;
                w = w->right;

                // 6
                d = x->degree;

                // 7
                while (A[d] != nullptr) {

                    // 8
                    y = A[d]; // Another node with the same degree as x

                    // 9
                    if (x->key > y->key) {

                        // 10
                        FibNode *temp = x;
                        x = y;
                        y = temp;
                    }

                    // 11
                    fibHeapLink(y, x);

                    // 12
                    A[d] = nullptr;

                    // 13
                    d++;
                }

                // 14
                A[d] = x;
            } while (w != min);

            // 15
            min = nullptr;

            // 16
            for (int i = 0; i < Dn; i++) {

                // 17
                if (A[i] != nullptr) {

                    // 18
                    if (min == nullptr) {

                        // 19
                        A[i]->left = A[i];
                        A[i]->right = A[i];

                        // 20
                        min = A[i];

                    // 21
                    } else {
                        min->left->right = A[i];
                        A[i]->left = min->left;
                        A[i]->right = min;
                        min->left = A[i];

                        // 22
                        if (A[i]->key < min->key) {

                            // 23
                            min = A[i];
                        }
                    }
                }
            }      
        }

        /*
         * Enlace de dos nodos en el Heap de Fibonacci
         *
         * FIB-HEAP-LINK(H,y,x)
         * 1. remove y from the root list of H
         * 2. make y a child of x, incrementing x.degree
         * 3. y.mark = FALSE
         *
         */
        void fibHeapLink(FibNode *y, FibNode *x) {

            // 1
            y->left->right = y->right;
            y->right->left = y->left;

            // 2 
            if (x->child == nullptr) {
                y->left = y;
                y->right = y;
                x->child = y;
            } else {  
                y->left = x->child;
                y->right = x->child->right;
                x->child->right = y;
                y->right->left = y;
            }

            y->parent = x;
            x->degree++;
            
            // 3
            y->mark = false;
        }

        /*
         * Decremento de la clave de un nodo en el Heap de Fibonacci (La clave pasa a ser k)
         *
         * FIB-HEAP-DECREASE-KEY(H,x,k)
         * 1. if k > x.key
         * 2. 	error "new key is greater than current key"
         * 3. x.key = k
         * 4. y = x.p
         * 5. if y != NIL and x.key < y.key
         * 6. 	CUT(H,x,y)
         * 7. 	CASCADING-CUT(H,y)
         * 8. if x.key < H.min.key
         * 9. 	H.min = x
         * 
         */
        void decreaseKey(FibNode *x, double k) {

            // 1
            if (k > x->key) {
                // 2
                return;
            }

            // 3
            x->key = k;
            x->par.first = k;

            // 4
            FibNode *y = x->parent;

            // 5
            if (y != nullptr && x->key < y->key) {

                // 6
                cut(x, y);

                // 7
                cascadingCut(y);
            }

            // 8
            if (x->key < min->key) {

                // 9
                min = x;
            }
        }

        /*
         * Corte de un nodo en el Heap de Fibonacci
         *
         * CUT(H,x,y)
         * 1. remove x from the child list of y, decrementing y.degree
         * 2. add x to the root list of H
         * 3. x.p = NIL
         * 4. x.mark = FALSE
         * 
         */
        void cut(FibNode *x, FibNode *y) {

            // 1
            if (x->right == x) {
                y->child = nullptr;
            } else {
                x->left->right = x->right;
                x->right->left = x->left;
                if (y->child == x) {
                    y->child = x->right;
                }
            }
            y->degree--;

            // 2
            x->left = min;
            x->right = min->right;
            min->right = x;
            x->right->left = x;

            // 3
            x->parent = nullptr;

            // 4
            x->mark = false;
        }

        /*
         * Corte en cascada de un nodo en el Heap de Fibonacci
         *
         * 1. z = y.p
         * 2. if z != NIL
         * 3. 	if y.mark == FALSE
         * 4. 		y.mark = TRUE
         * 5. 	else CUT(H,y,z)
         * 6. 		CASCADING-CUT(H,z)
         *
         */
        void cascadingCut(FibNode *y) {

            // 1
            FibNode *z = y->parent;

            // 2
            if (z != nullptr) {

                // 3
                if (y->mark == false) {

                    // 4
                    y->mark = true;

                } else {
                    // 5
                    cut(y, z);

                    // 6
                    cascadingCut(z);
                }
            }
        }

        /*
         * Remoción de un nodo en el Heap de Fibonacci
         *
         * FIB-HEAP-DELETE(H,x)
         * 1. FIB-HEAP-DECREASE-KEY(H,x,−∞)
         * 2. FIB-HEAP-EXTRACT-MIN(H)
         * 
         */
        void remove(FibNode *x) {

            // 1
            decreaseKey(x, -std::numeric_limits<double>::infinity());

            // 2
            extractMin();
        }

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