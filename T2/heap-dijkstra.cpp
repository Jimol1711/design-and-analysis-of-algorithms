#include <iostream>
#include <vector>
#include <string>
#include <bits/stdc++.h>

using namespace std;

class Heap {
private:
    vector<pair<double, int>> heap;

public:
    // constructor
    Heap(vector<pair<double, int>> array) : heap(array) {
        buildHeap(); // Se convierte el arreglo en un heap
    }

    // metodo heapify, para reorganizar el heap. Es O(log n)
    void heapify(int i) {
        int smallest = i; // Inicializar el nodo raíz como el más grande
        int left = 2 * i + 1; // Hijo izquierdo
        int right = 2 * i + 2; // Hijo derecho
        int n = heap.size(); // tamaño del heap

        // Si existe hijo izquierdo y es mayor que la raíz
        if (left < n && heap[left].first < heap[smallest].first)
            smallest = left;

        // Si existe hijo derecho y es mayor que el más grande hasta ahora
        if (right < n && heap[right].first < heap[smallest].first)
            smallest = right;

        // Si el más pequeño no es la raíz
        if (smallest != i) {
            swap(heap[i], heap[smallest]); // intercambiar la posición de los nodos
            heapify(smallest); // Recursivamente heapificar el subárbol afectado
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
            heapify(i);
        }
    }

    /* Este método obtiene el mínimo del heap (que siempre será la raíz), y luego manda el último elemento
    en el heap hacia la raíz para hacerle heapify. Finalmente, el heap queda ordenado nuevamente y se retorna
    el mínimo que se obtuvo al inicio
    */
    pair<double, int> extractMin() {
        pair<double, int> minElement = heap[0]; // Se guarda el elemento mínimo del heap
        heap[0] = heap.back(); // se asigna el último elemento a la raíz
        heap.pop_back(); // se elimina ese último elemento del heap
        heapify(0); // hacemos heapify al elemento que se llevó a la raíz
        return minElement; // retornamos el elemento minimo que se obtuvo al inicio
    }
};