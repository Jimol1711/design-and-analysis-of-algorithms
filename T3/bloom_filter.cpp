#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include "./hashing/cityhash/src/city.h" // importar cityHash
#include "./hashing/smhasher/src/MurmurHash2.h" // importar murmurHash


using namespace std;

/** La idea es crear un Bloom Filter. Este puede variar en parámetros m y k, los cuales serían
 * el tamaño del arreglo de bits M y el número de funciones de hash, respectivamente.
 * 
 * En el paper del pdf "hashing_in_bloom.pdf", basicamente se concluye que se puede implementar
 * el filtro de Bloom utilizando solo dos funciones de hashing base, dado que se pueden simular
 * funciones de hash adicional a partir de las dos funciones de hash base de la siguiente forma:
 * g_i = h_1(x) + i*h_2(x).
 * 
 * Otra manera sería crear una familia universal (como aparece en el apunte),
 * aunque habría que ver como encontrar un primo >= N, con N el tamaño del Universo. ¿...?
 * 
 * Las funciones de hash escogidas son arbitrarias, obviamente, tomando algunas conocidas por 
 * su rapidez y buena dispersión. Además, es preferible que sean no criptográficas, pues tienen
 * menor complejidad y menor tiempor de cálculo y la seguridad no es una prioridad (como en el 
 * caso de codificar contraseñas o algo por el estilo).
 */


/** Funcion que imprime las palabras que coinciden dentro de un archivo mediante un proceso
 * tipo grep (ve cuales palabras coinciden con alguna linea en el archivo e imprime esas palabras)
 * 
 * Parametros:
 * filename: el archivo en el cual se realizará la búsqueda
 * N : Secuencia de palabras representado por un vector, donde sus elementos son strings.
 */
void grep(const string& filename, const vector<string>& N) {
    // Abrimos el archivo
    std::ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error al abrir el archivo." << endl;
        return;
    }

    // Variable donde se almacenará los string de cada linea
    string line;

    // Para cada palabra en N, realizamos si coincide con alguna línea en el archivo abierto
    for (auto& word : N) {

        // Reiniciar el indicador de posición al inicio del archivo
        file.clear();  // Limpiar cualquier bandera de error del archivo
        file.seekg(0, ios::beg);  // Mover el indicador de posición al inicio

        // Se recorre cada linea del archivo buscando coincidencias con la palabra actual de la iteracion
        // .find devuelve la posición de la primera aparición de una subcadena. Si no la encuentra, devuelve string::npos
        while (getline(file, line)) {
            if (line.find(word) != string::npos) {
                cout << line << endl;
            }
        }
    }

    file.close();
}

class BloomFilter {
private:
    vector<bool> M; // arreglo M
    int m; // tamaño del arreglo M
    int k; // número de funciones de hash
    string file_name; // archivo con el cual se formará el flitro de Bloom (.csv)


public:
    // Constructor de BloomFilter. Asigna los atributos m y k. Además inicializa el array M
    // de tamaño m, con sus celdas en 0 (false).
    BloomFilter(int array_size, int hash_count, string file_name) : m(array_size), k(hash_count), M(false, m), file_name(file_name) {
        
        // Abrimos el archivo .csv
        std::ifstream file(file_name);
        if (!file.is_open()) {
            cerr << "Error al abrir el archivo CSV." << endl;
        }

        string line; // string que almacenará cada linea en el csv mientras se vayan leyendo 
        
        getline(file, line); // leemos la primera linea y no hacemos nada, la omitimos.

        // Luego, con el resto de las lineas:
        while (getline(file, line)) {
            // Calcula el hash de la línea usando CityHash ( h1(x) )
            uint32_t city_hash = CityHash32(line.c_str(), line.size());
            
            // Calcula el hash de la linea usando MurmurHash ( h2(x) )
            uint32_t murmur_hash = MurmurHash2(line.c_str(), line.size(), 0);

            // Simulamos las k funciones de hashing diferentes y ponemos en 1 las celdas que corresponden
            for (int i=0; i<k; i++) {
                // g_i(x) = h1(x) + i*h2(x)
                int k_hash_value = (city_hash + i*murmur_hash) % m;

                // M[j] = 1, para cada j retornado por cada función de hash
                M[k_hash_value] = true;
            }
        }

        file.close();

    }

    /** Método que realiza la búsqueda utilizando filtro de Bloom.
     * Por cada palabra en la secuencia N, vemos si los indices que devuelvenentregados por las k funciones de 
     * hash están en 1 (true) en el arreglo M. Si es así, puede que esa palabra realmente esté en el archivo
     * abierto. En caso contrario, sin error la palabra no está en el archivo, por lo tanto quitamos esa palabra
     * de la secuencia N (quitamos el elemento del arreglo). Finalmente en N nos quedamos solo con las palabras 
     * que si pueden estar en el archivo y se aplica grep a N.
     */
    void search(vector<string>& N) {

        auto it = N.begin(); // iniciamos iterador en el primer elemento de N

        // N.end() entrega un iterador al elemento que sigue al último del vector.
        while (it != N.end()) {
            // Calcula el hash de la línea usando CityHash ( h1(x) )
            uint32_t city_hash = CityHash32(it->c_str(), it->size());
            
            // Calcula el hash de la línea usando MurmurHash ( h2(x) )
            uint32_t murmur_hash = MurmurHash2(it->c_str(), it->size(), 0);

            // variable que indicará si hay que eliminar el elemento del vector o no
            bool eliminar = false;

            // Calculamos el valor del hash de la palabra actual del iterador para cada funcion de hash h_i 
            for (int i = 0; i < k; i++) {
                // g_i(x) = h1(x) + i*h2(x)
                int k_hash_value = (city_hash + i * murmur_hash) % m;

                // Si una funcion da 0 (false), entonces la palabra no está y se debe eliminar de N
                if (!M[k_hash_value]) {
                    eliminar = true;
                    break;
                }
            }

            if (eliminar)
                it = N.erase(it);  // Borra el elemento y actualiza el iterador
            else
                ++it;  // Avanza al siguiente elemento
        }

        // En este punto N solo contiene las palabras que si podrían estar en el archivo, y aplicamos grep a N.
        grep(this->file_name, N);
    }

};

int main() {
    printf("corre");
    return 0;
}

