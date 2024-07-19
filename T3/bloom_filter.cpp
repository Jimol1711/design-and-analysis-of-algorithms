#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <math.h>
#include <random>
#include <ctime>
#include <chrono>
#include <algorithm>
#include <numeric>
#include <unordered_set>
#include <ostream>
#include <cmath>
#include "csv-filter.cpp"
#include "City.cpp" // importar cityHash
#include "MurmurHash2.cpp" // importar murmurHash

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


/** Funcion que imprime las palabras que coinciden dentro de un archivo mediante un proceso
 * tipo grep (ve cuales palabras coinciden con alguna linea en el archivo e imprime esas palabras)
 * 
 * Parametros:
 * filename: el archivo en el cual se realizará la búsqueda
 * N : Secuencia de palabras representado por un vector, donde sus elementos son strings.
 * 
 * Return: 
 *  int coincidences: El numero de palabras en N encontradas en el archivo.
 */
int grep(const string& filename, const vector<string>& N) {

    // Variable que cuenta el numero de palabras encontradas en el archivo 
    int coincidences = 0;

    // Abrimos el archivo
    std::ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error al abrir el archivo." << endl;
        return -1;
    }

    // Variable donde se almacenará los string de cada linea
    string line;

    // Para cada palabra en N, realizamos si coincide con alguna línea en el archivo abierto
    for (auto& word : N) {

        // Reiniciar el indicador de posición al inicio del archivo
        file.clear();  // Limpiar cualquier bandera de error del archivo
        file.seekg(0, ios::beg);  // Mover el indicador de posición al inicio

        // Se recorre cada linea del archivo buscando coincidencias con la palabra actual de la iteracion
        while (getline(file, line)) {
            if (line == word) {
                coincidences++;
                cout << line << endl;
                break;
            }
        }
    }

    // Cerramos el archivo que abrimos inicialmente
    file.close();

    return coincidences;
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
    BloomFilter(int array_size, int hash_count, string file_name) : m(array_size), k(hash_count), M(array_size, false), file_name(file_name) {


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

            uint64 city_hash = CityHash64(line.c_str(), line.size());
            
            // Calcula el hash de la linea usando MurmurHash ( h2(x) )
            uint64_t murmur_hash = MurmurHash64A(line.c_str(), line.size(), 0);

            // Simulamos las k funciones de hashing diferentes y ponemos en 1 las celdas que corresponden
            for (int i=0; i<k; i++) {
                // g_i(x) = h1(x) + i*h2(x)
                int k_hash_value = (city_hash + i*murmur_hash) % m; // 20

                // M[j] = 1, para cada j retornado por cada función dse hash
                M[k_hash_value] = true;
            }
            
        }

        file.close();

    }

    /** Método que realiza la búsqueda utilizando filtro de Bloom.
     * Por cada palabra en la secuencia N, vemos si los indices que devuelve entregados por las k funciones de 
     * hash están en 1 (true) en el arreglo M. Si es así, puede que esa palabra realmente esté en el archivo
     * abierto. En caso contrario, sin error la palabra no está en el archivo, por lo tanto quitamos esa palabra
     * de la secuencia N (quitamos el elemento del arreglo). Finalmente en N nos quedamos solo con las palabras 
     * que si pueden estar en el archivo y se aplica grep a N.
     */
    void search(vector<string>& N) {

        // tamaño inicial de N, es decir, el numero de palabras en el vector
        int N_initial_size = N.size();

        auto it = N.begin(); // iniciamos iterador en el primer elemento de N

        // N.end() entrega un iterador al elemento que sigue al último del vector.
        while (it != N.end()) {
            // uint32_t city_hash = CityHash32(line.c_str(), line.size());
            
            // Calcula el hash de la linea usando MurmurHash ( h2(x) )
            uint64 city_hash = CityHash64(it->c_str(), it->size());
            uint64_t murmur_hash = MurmurHash64A(it->c_str(), it->size(), 0);

            // variable que indicará si hay que eliminar el elemento del vector o no
            bool eliminar = false;

            // Calculamos el valor del hash de la palabra actual del iterador para cada funcion de hash h_i 
            for (int i = 0; i < k; i++) {
                // g_i(x) = h1(x) + i*h2(x)
                int k_hash_value = (city_hash + i * murmur_hash) % m; // 20

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

        // Cantidad de palabras que quedan en N despues de hacer el filtro en el vector 
        int N_final_size = N.size();

        // En este punto N solo contiene las palabras que si podrían estar en el archivo, y aplicamos grep a N.
        int coincidences = grep(this->file_name, N);
        cout << "coincidencias reales:" << coincidences << endl;
        
        // Prob. de error del filtro.
        double p = double(N_final_size - coincidences) / N_initial_size;
        cout << "La probabilidad de error es:" << p << endl;
    }

};

// From here on is bloom filter test

/* Función que obtiene las primeras N palabras de un archivo csv y retorna vector con ellas
 *
 */
vector<string> getFirstNWords(int N, const string& file) {
    // Abrimos el archivo
    ifstream infile(file);
    if (!infile.is_open()) {
        cerr << "Error al abrir el archivo." << endl;
        return {};
    }

    // Vector para almacenar las palabras del archivo
    vector<string> words;
    string line;

    // Leemos y omitimos la primera línea
    getline(infile, line);

    // Leemos las primeras N líneas del archivo
    int count = 0;
    while (count < N && getline(infile, line)) {
        words.push_back(line);
        count++;
    }

    infile.close();

    // Verificamos que hay suficientes palabras en el archivo
    if (words.size() < static_cast<size_t>(N)) {
        cerr << "El archivo no contiene suficientes palabras." << endl;
        return {};
    }

    return words;
}

/* Función que obtiene N palabras aleatorias de un archivo csv y retorna vector con ellas
 *
 */
vector<string> getRandomWords(int N, const string& file) {
    // Abrimos el archivo
    ifstream infile(file);
    if (!infile.is_open()) {
        cerr << "Error al abrir el archivo." << endl;
        return {};
    }

    // Vector para almacenar todas las palabras del archivo
    vector<string> words;
    string line;

    // Leemos y omitimos la primera línea
    getline(infile, line);

    // Leemos el resto del archivo línea por línea
    while (getline(infile, line)) {
        words.push_back(line);
    }

    infile.close();

    // Verificamos que hay suficientes palabras en el archivo
    if (words.size() < static_cast<size_t>(N)) {
        cerr << "El archivo no contiene suficientes palabras." << endl;
        return {};
    }

    // Seleccionamos N palabras aleatorias
    vector<string> randomWords;
    random_device rd;
    mt19937 gen(time(0)); // Usamos el tiempo actual como semilla para asegurar diferentes secuencias
    uniform_int_distribution<> dis(0, words.size() - 1);

    for (int i = 0; i < N; ++i) {
        int index = dis(gen);
        randomWords.push_back(words[index]);
    }

    return randomWords;
}

// Function to read a CSV file and return a vector of strings
vector<string> vectorizeCSV(const string& file) {
    // Open the file
    ifstream infile(file);
    if (!infile.is_open()) {
        cerr << "Error opening file." << endl;
        return {};
    }

    // Vector to store the words
    vector<string> words;
    string line;

    getline(infile, line); // Skip the first line

    // Read each line of the file
    while (getline(infile, line)) {
        words.push_back(line);
    }

    // Close the file
    infile.close();

    return words;
}

// Function to generate a random integer within a range
int getRandomInt(int max_value) {
    auto seed = chrono::system_clock::now().time_since_epoch().count();
    static mt19937 gen(seed);
    uniform_int_distribution<> dis(0, max_value - 1);
    return dis(gen);
}

// Function to replace n random words in words1 with random words from words2
vector<string> replaceRandomWords(const vector<string>& words1, const vector<string>& words2, int n) {
    if (words1.size() < n || words2.empty()) {
        cerr << "Invalid parameters: words1 size is less than n or words2 is empty." << endl;
        return {};
    }

    // Create a copy of words1
    vector<string> result = words1;

    // Generate n unique random indices for words1
    vector<int> indices(result.size());
    iota(indices.begin(), indices.end(), 0); // Fill indices with 0, 1, ..., result.size()-1
    shuffle(indices.begin(), indices.end(), mt19937{random_device{}()}); // Shuffle the indices

    for (int i = 0; i < n; ++i) {
        // Select a random index from words2
        int randomIndexFromWords2 = getRandomInt(words2.size());
        
        // Replace word at random index in result with a random word from words2
        result[indices[i]] = words2[randomIndexFromWords2];
    }

    return result;
}

// Función que retorna un vector con n palabras de words1 reemplazando n*p de words2
vector<string> replaceRandomWords(
    const vector<string>& words1,
    const vector<string>& words2,
    int n,
    double p
) {
    
    // Create a vector of n strings from words1
    vector<string> result;
    int numWords = min(static_cast<int>(words1.size()), n);
    result.reserve(n);
    for (int i = 0; i < numWords; ++i) {
        result.push_back(words1[i]);
    }
    
    // Calculate the number of words to replace
    int numReplacements = static_cast<int>(n * p);
    numReplacements = min(numReplacements, static_cast<int>(words2.size()));
    
    // Seed for random number generation
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, static_cast<int>(result.size()) - 1);
    
    // Replace random words from words2
    for (int i = 0; i < numReplacements; ++i) {
        int index = dis(gen);
        result[index] = words2[dis(gen) % words2.size()];
    }
    
    return result;
}

int main() {

    vector<string> sequence = {"CRISTIANORONALDOOO", "ANDRES", "dsfklñgjikldfjldf"};

    // Tamaños de N y proporciones
    vector<int> N_sizes = {1024, 4096, 16384, 65536};
    vector<double> proportions = {0, 0.25, 0.5, 0.75, 1};

    // Se vectorizan los archivos csv
    vector<string> vectorized_babies = vectorizeCSV("csv/Popular-Baby-Names-Final.csv");
    vector<string> vectorized_movies = vectorizeCSV("csv/Filtered-Movies.csv");

    vector<vector<string>> sequences;

    for (int i=0; i<N_sizes.size(); i++) {
        for (int j=0; j<proportions.size(); j++) {
            sequences.push_back(replaceRandomWords(vectorized_babies, vectorized_movies, N_sizes[i], proportions[j]));
        }
    }

    // grep("csv/Popular-Baby-Names-Final.csv", sequence);

    BloomFilter filter(100, 5, "csv/Popular-Baby-Names-Final.csv");
    filter.search(sequence);

    return 0;
    
}
