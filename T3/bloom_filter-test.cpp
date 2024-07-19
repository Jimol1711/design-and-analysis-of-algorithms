#include "csv-filter.cpp"

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

    return 0;
    
}