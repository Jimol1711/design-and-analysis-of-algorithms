#include "dijkstra.hpp"

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
        HeapNode* ptr = (HeapNode *)adjList[node].heapPtr;
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

pair<vector<double>,vector<int>> dijkstraHeap(Graph graph, int source) {

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
