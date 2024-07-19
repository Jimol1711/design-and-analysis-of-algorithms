

// Function to read words from a file into a set
unordered_set<string> readWordsFromFile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening the file: " << filename << endl;
        return {};
    }

    unordered_set<string> words;
    string line;

    // Read each line from the file and insert into the set
    while (getline(file, line)) {
        words.insert(line);
    }

    file.close();
    return words;
}

// Function to read words from a file into a vector
vector<string> readWordsIntoVector(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening the file: " << filename << endl;
        return {};
    }

    vector<string> words;
    string line;

    // Read each line from the file and insert into the vector
    while (getline(file, line)) {
        words.push_back(line);
    }

    file.close();
    return words;
}

// Function to write words from a vector to a file
void writeWordsToFile(const vector<string>& words, const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening the file: " << filename << endl;
        return;
    }

    for (const auto& word : words) {
        file << word << endl;
    }

    file.close();
}

// Function to filter words
void filterWords(const string& file1, const string& file2, const string& outputFile) {
    // Read words from the first file into a set
    unordered_set<string> filterWords = readWordsFromFile(file1);

    // Read words from the second file into a vector
    vector<string> wordsToFilter = readWordsIntoVector(file2);

    // Vector to store filtered words
    vector<string> filteredWords;

    // Filter words that are not in the filterWords set
    for (const auto& word : wordsToFilter) {
        if (filterWords.find(word) == filterWords.end()) {
            filteredWords.push_back(word);
        }
    }

    // Write the filtered words to the output file
    writeWordsToFile(filteredWords, outputFile);
}
