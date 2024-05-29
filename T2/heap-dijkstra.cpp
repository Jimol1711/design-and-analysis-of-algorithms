#include <iostream>
#include <vector>
#include <string>
#include <bits/stdc++.h>

using namespace std;

int getRandomInt(int v) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, v - 1);
    return dis(gen);
}


struct Vertex {
    vector<pair<int, double>> neighbors;
    HeapNode* heapPtr;
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


struct HeapNode {
    double distance;
    int node;

    HeapNode(double d, int n) : distance(d), node(n) {} // constructor
};

class Heap {
private:
    vector<HeapNode>& heap;
    vector<Vertex>& adjList;

    // metodo heapifyUp, para reorganizar el heap cuando se inserta un elemento o en particular para este caso, 
    // para cuando se modifica la distancia de un nodo en el heap (su valor). Es O(log n)
    void heapifyUp(int i) {

        // Mientras el elemento a hacer heapifyUp no sea la raiz y su valor sea menor a la de su padre
        while (i > 0 && heap[i].distance < heap[(i - 1) / 2].distance) {
            // indice del padre
            int parent = (i - 1) / 2;

            // intercambiar el elemento con su padre
            swap(heap[i], heap[parent]);
            // intercambiar los punteros de los vertices en los grafos que son representados por estos elementos 
            swap(adjList[heap[i].node].heapPtr, adjList[heap[parent].node].heapPtr);

            // actualizar el indice del elemento que se le hizo heapifyUp, ahora su indice es el de su ex padre
            i = parent;
        }
    }

    // metodo heapifyDown, para reorganizar el heap cuando se extrae el mínimo. Es O(log n)
    void heapifyDown(int i) {
        int smallest = i; // Inicializar el nodo raíz como el más grande
        int left = 2 * i + 1; // Hijo izquierdo
        int right = 2 * i + 2; // Hijo derecho
        int n = heap.size(); // tamaño del heap

        // Si existe hijo izquierdo y es mayor que la raíz
        if (left < n && heap[left].distance < heap[smallest].distance)
            smallest = left;

        // Si existe hijo derecho y es mayor que el más grande hasta ahora
        if (right < n && heap[right].distance < heap[smallest].distance)
            smallest = right;

        // Si el más pequeño no es la raíz
        if (smallest != i) {
            // intercambiar la posición de los nodos en el heap
            swap(heap[i], heap[smallest]);

            // intercambiar los punteros de los vertices en el grafo
            swap(adjList[heap[i].node].heapPtr, adjList[heap[smallest].node].heapPtr);

            // Recursivamente heapificar el subárbol afectado
            heapifyDown(smallest); 
        }
    }

public:
    // constructor
    Heap(vector<HeapNode>& array, vector<Vertex>& adjList) : heap(array), adjList(adjList) {
        // Se convierte el arreglo en un heap
        buildHeap();

        // Setear los punteros de los vertices en el grafo a sus elementos representantes en el heap
        for (int i = 0; i < heap.size(); ++i) {
            adjList[heap[i].node].heapPtr = &heap[i];
        }
    }

    /* Método para construir el heap desde un arreglo desordenado por entero.
    Podría ir construyéndose el heap a medida que se hacen inserciones al arreglo y utilizando heapify, pero esto
    tomaría claramente O(n * log n). Para lograr construir el heap en O(n), notamos que los nodos hoja, es decir los 
    nodos que no tienen hijos, son ~ n/2 y ya cumplen por definición que son un heap. Por lo tanto, hacemos heapify a 
    cada nodo que no sea hoja, desde abajo hacia arriba. De esta manera, se logra construir un heap a partir de un 
    arreglo desordenado en o(n)
    */
    void buildHeap() {
        int n = heap.size(); // tamaño del arreglo que representa al heap

        // Empezar desde el último nodo no hoja
        for (int i = n / 2 - 1; i >= 0; i--) {
            heapifyDown(i);
        }
    }

    /* Este método obtiene el mínimo del heap (que siempre será la raíz), y luego manda el último elemento
    en el heap hacia la raíz para hacerle heapify. Finalmente, el heap queda ordenado nuevamente y se retorna
    el mínimo que se obtuvo al inicio
    */
    HeapNode extractMin() {
        HeapNode minElement = heap[0]; // Se guarda el elemento mínimo del heap
        heap[0] = heap.back(); // se asigna el último elemento a la raíz
        heap.pop_back(); // se elimina ese último elemento del heap
        if (!heap.empty())
            heapifyDown(0); // hacemos heapify al elemento que se llevó a la raíz
        return minElement; // retornamos el elemento minimo que se obtuvo al inicio
    }

    // Método para actualizar la distancia de un nodo en el heap
    void decreaseKey(int node, double newDistance) {

        // obtenemos el puntero al nodo en el heap y actualizamos la distancia del nodo
        HeapNode* ptr = adjList[node].heapPtr;
        ptr->distance = newDistance;

        // Obtener el índice del nodo en el heap (la resta de punteros devuelve un indice) y el indice de su padre
        int index = ptr - &heap[0]; 
        
        // aplicar heapifyUp al nodo del heap que se le cambió su distancia
        // Como en este caso solo se disminuyen las distancias de los nodos, solo se debe aplicar heapifyUp
        heapifyUp(index);
    }

    // Método que indica si el heap está vacío
    bool isEmpty() {
        return heap.empty();
    }
};

pair<vector<double>, vector<int>> dijkstraHeap(Graph graph, int source) {

    // Definir el valor infinito
    double INF = numeric_limits<double>::infinity();

    int numVertices = graph.getNumVertices();

    /* Definimos los arreglos de distancias y previos. Como inicialmente estos arreglos comienzan con
    un valor por defecto excepto el elemento que hace referencia a la raíz, para hacerlo mas eficiente inicializamos
    estos arreglos con esos valores por defecto y luego solo modificamos el elemento que corresponde a la raíz del grafo
     */
    vector<double> distances(numVertices, INF);
    vector<int> previous(numVertices, -1);

    // Declaramos el arreglo donde agregaremos los pares, el que luego lo convertiremos en un heap
    vector<HeapNode> array;
    array.reserve(numVertices);

    // Modificar la distancia del nodo raiz y ponerlo con valor 0 y agregamos el par (distancia = 0, nodo = raíz) a Q.
    distances[source] = 0;
    array.emplace_back(0, source);

    // por cada vertice del grafo:
    for (int i=0; i<numVertices; i++) {

        // si el vertice v es distinto a la raíz, agregamos (distancia = infinito, nodo = v) a Q
        if (i != source)
            array.emplace_back(INF, i);
    }

    // Hacemos la creacion del heap
    Heap Q(array, graph.adjList);

    // Mientras Q (el min heap) no se encuentre vacío, repetimos:
    while (!Q.isEmpty()) {
        // obtenemos el par con menor distancia en Q y lo eliminamos
        HeapNode minPair = Q.extractMin();

        // id del nodo representado como v en el enunciado
        int v = minPair.node;

        // para cada vecino del nodo en el par obtenido
        for (auto& neighbor : graph.adjList[v].neighbors) {

            // id del nodo vecino de v representado como u en el enunciado
            int u = neighbor.first;
            // peso de la arista entre v y u
            double weight = neighbor.second;

            // Si la distancia guardada de u es mayor que la distancia guardada de v más el peso de la arista (u,v)
            if (distances[u] > distances[v] + weight) {
                distances[u] = distances[v] + weight; // actualizamos el valor de la distancia de u
                previous[u] = v; // guardamos v como el nodo previo de u

                // actualizamos la distancia del par que representa al nodo u en Q utilizando decreaseKey.
                Q.decreaseKey(u, distances[u]);
            }
        }
    }

    // Retornamos el arreglo de distancias y previos
    return {distances, previous};

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
                dijkstraHeap(graph, raiz);
                auto end1 = chrono::high_resolution_clock::now();   // Registro de tiempo de finalización heap
                chrono::duration<double> duration1 = end1 - start1;   // Cálculo del tiempo transcurrido heap
                cout << "Tiempo del algoritmo con heap: " << duration1.count() << " segundos" << std::endl;
            }
        }
    }

    return 0;
}