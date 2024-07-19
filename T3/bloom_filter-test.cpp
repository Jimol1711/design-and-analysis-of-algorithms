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
