#include "heap-dijkstra.cpp"

// Grafo
class Graph {

    public:

        struct Vertex {
            vector<pair<int, double>> neighbors;
            void* nodeInHeap; // Puntero al nodo en el heap
            int id;

            Vertex(int id = 0) : id(id), nodeInHeap(nullptr) {}
        };

        vector<Vertex> adjList;
        int numVertices;

        // Constructor
        Graph(int numVertices) 
            : numVertices(numVertices) {
            adjList.resize(numVertices);
            for (int i = 0; i < numVertices; i++) {
                adjList[i] = Vertex(i); // Initialize each vertex with its ID
            }
        }

        // Añadir una arista con un peso
        void addEdge(int u, int v, double weight) {
            adjList[u].neighbors.emplace_back(v, weight);
            adjList[v].neighbors.emplace_back(u, weight);  // No dirigido
        }

        // Print the adjacency list representation of the graph
        void printGraph() {
            for (int i = 0; i < numVertices; ++i) {
                cout << "Vertex " << i << ":";
                for (const auto& neighbor : adjList[i].neighbors) {
                    cout << " -> (" << neighbor.first << ", " << neighbor.second << ")";
                }
                cout << endl;
            }
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
                std::pair<double, Graph::Vertex> par; // Par (distancia, nodo) almacenado

                // Constructor del Nodo del Heap de Fibonacci
                FibNode(double key, Graph::Vertex vertex) 
                    : par(key, vertex) {
                    this->key = key;
                    this->mark = false;
                    this->parent = nullptr;
                    this->left = this;
                    this->right = this;
                    this->child = nullptr;
                    this->degree = 0;

                    // Asignar puntero al par en el heap
                    
                    par.second.nodeInHeap = (FibNode *)this;
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
            #if 0
            FibNode *w, *next, *x, *y, *temp;
            FibNode **A, **rootList;

            int d, rootSize;
            int Dn = static_cast<int>(log2(n)) + 1;

            // 1
            A = new FibNode*[Dn + 2];

            // 2, 3
            for (int i = 0; i < Dn; i++) {
                A[i] = nullptr;
            }

            // 4
            w = min;
            next = w;
            rootSize = 1;

            while (next != w) {
                rootSize++;
                next = next->right;
            }

            rootList = new FibNode*[rootSize];

            for (int i = 0; i < rootSize; i++) {
                rootList[i] = next;
                next = next->right;
            }
            
            for (int i = 0; i < rootSize; i++) {

                w = rootList[i];

                // 5
                x = w;

                // 6
                d = x->degree;

                // 7
                while (A[d] != nullptr) {

                    // 8
                    y = A[d]; // Another node with the same degree as x

                    // 9
                    if (x->key > y->key) {

                        // 10
                        temp = x;
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
            }
            delete [] rootList;

            // 15
            min = nullptr;

            // 16
            for (int i = 0; i < Dn + 2; i++) {

                // 17
                if (A[i] != nullptr) {

                    // 18
                    if (min == nullptr) {

                        // 19 y 20
                        min = A[i];
                        A[i]->left = A[i];
                        A[i]->right = A[i];

                        // 20
                        // min = A[i];

                    // 21
                    } else {
                        printf("A[i]: %i and min: %i\n", A[i]->key, min->key);
                        min->left->right = A[i];
                        A[i]->left = min->left;
                        min->left = A[i];
                        A[i]->right = min;

                        // 22
                        if (A[i]->key < min->key) {

                            // 23
                            min = A[i];
                        }
                    }
                }
            }
            delete [] A; 
            #endif
            FibNode* w, * next, * x, * y, * temp;
            FibNode** A, ** rootList;
            // Max degree <= log base golden ratio of n
            int d, rootSize;
            int max_degree = static_cast<int>(floor(log(static_cast<double>(n))/log(static_cast<double>(1 + sqrt(static_cast<double>(5)))/2)));

            // 1
            A = new FibNode*[max_degree+2]; // plus two both for indexing to max degree and so A[max_degree+1] == NIL
            // 2, 3
            std::fill_n(A, max_degree+2, nullptr);
            // 4
            w = min;
            rootSize = 0;
            next = w;
            do
            {
            rootSize++;
            next = next->right;
            } while ( next != w );
            rootList = new FibNode*[rootSize];
            for ( int i = 0; i < rootSize; i++ )
            {
            rootList[i] = next;
            next = next->right;
            }
            for ( int i = 0; i < rootSize; i++ )
            {
            w = rootList[i];
            // 5
            x = w;
            // 6
            d = x->degree;
            // 7
            while ( A[d] != nullptr )
            {
                // 8
                y = A[d];
                // 9
                if ( y->key < x->key )
                {
                // 10
                temp = x;
                x = y;
                y = temp;
                }
                // 11
                fibHeapLink(y,x);
                // 12
                A[d] = nullptr;
                // 13
                d++;
            }
            // 14
            A[d] = x;
            }
            delete [] rootList;
            // 15
            min = nullptr;
            // 16
            for ( int i = 0; i < max_degree+2; i++ )
            {
            // 17
            if ( A[i] != nullptr )
            {
                // 18
                if ( min == nullptr )
                {
                // 19, 20
                min = A[i]->left = A[i]->right = A[i];
                }
                else
                {
                // 21
                min->left->right = A[i];
                A[i]->left = min->left;
                min->left = A[i];
                A[i]->right = min;
                // 22
                if ( A[i]->key < min->key )
                {
                    // 23
                    min = A[i];
                }
                }
            }
            }
            delete [] A;
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
pair<vector<double>,vector<int>> dijkstraFibonacci(Graph graph, Graph::Vertex raiz) {

    double INF = std::numeric_limits<double>::infinity();

    int numVertices = graph.numVertices;

    for (int i = 0; i < numVertices; i++) {
        graph.adjList[i].id = i;
    }

    // Parte 1.
    vector <double> distancias(numVertices, INF);
    vector <int> previos(numVertices, -1);

    // Parte 2.
    FibHeap fibHeap;

    // Parte 3.
    FibHeap::FibNode *nodoRaiz = new FibHeap::FibNode(0, raiz);

    // Parte 4.
    for (int i = 0; i < numVertices; i++) {
        if (i != raiz.id) {
            FibHeap::FibNode *nodo = new FibHeap::FibNode(INF, graph.adjList[i]);

            // 4.1
            distancias[i] = INF;
            previos[i] = -1;

            // 4.2
            nodo->par.first = INF;
            nodo->par.second = graph.adjList[i];

            // Parte 5. (Es O(n) porque el insert tiene costo O(1) en el heap de Fibonacci)
            fibHeap.insert(nodo);
        }
    }

    // Parte 6.
    while (fibHeap.min != nullptr) {
        // a)
        FibHeap::FibNode *u = fibHeap.extractMin();
        std::pair<double, Graph::Vertex> par = u->par;

        // b)
        for (auto vecino : par.second.neighbors) {
            int v = vecino.first;
            double peso = vecino.second;

            if (distancias[par.second.id] > peso + distancias[v]) {
                distancias[v] = distancias[par.second.id] + peso;
                previos[v] = par.second.id;

                for (int i = 0; i < fibHeap.n; i++) {
                    if (fibHeap.min->par.second.id == v) {
                        fibHeap.decreaseKey(fibHeap.min, distancias[v]);
                        break;
                    }
                    fibHeap.min = fibHeap.min->right;
                }
            }
        }
    }

    cout << distancias[467];

    // Parte 7.
    pair<vector<double>, vector<int>> distancias_y_previos = make_pair(distancias, previos);

    return distancias_y_previos; // Aquí podría retornar otra cosa

}