#include "bloom_filter.cpp"

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
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, words.size() - 1);

    for (int i = 0; i < N; ++i) {
        int index = dis(gen);
        randomWords.push_back(words[index]);
    }

    return randomWords;
}

int main() {

    // Tamaños de N
    vector<int> N_sizes = {};
    for (int i=10; i<17; i+=2) {
        N_sizes.push_back(pow(2, i));
    }

    // Selección de N palabras del archivo csv
    vector<vector<string>> N_sequences;
    for (int i=0; i<N_sizes.size(); i++) {
        vector<string> randomWords = getRandomWords(N_sizes[i], "csv/Popular-Baby-Names-Final.csv");
        cout << randomWords[23] << endl;
        N_sequences.push_back(randomWords);
    }

    /* 1. Búsqueda con grep
     *
     */
    //grep("csv/Popular-Baby-Names-Final.csv", N_sequences[0]);

    /* 2. Búsqueda con bloom filter
     *
     */
    // BloomFilter filter(100, 5, "csv/Popular-Baby-Names-Final.csv");
    // filter.search(N_sequences[0]);


    // BloomFilter filter2(100, 5, "csv/Film-Names.csv");
    // filter2.search(N_sequences[0]);

    cout << "N_sizes: " << N_sizes[0] << ", " << N_sizes[1] << ", " << N_sizes[2] << ", " << N_sizes[3] << endl;

    cout << "N_sequences first word on first sequence: " << N_sequences[0][0] << endl;

    return 0;
    
}