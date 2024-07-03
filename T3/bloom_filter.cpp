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
 * Las funciones de hash escogidas son arbitrarias, obviamente, tomando algunas conocidas por 
 * su rapidez y buena dispersión. Además, es preferible que sean no criptográficas, pues tienen
 * menor complejidad y menor tiempor de cálculo y la seguridad no es una prioridad (como en el 
 * caso de codificar contraseñas o algo por el estilo).
 */

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
        
        // Leemos el archivo .csv
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

    // luego crear metodo de búsqueda, etc..

};

