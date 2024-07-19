#include <vector>
#include <string>
#include <random>
#include <algorithm>
#include <iostream>

std::vector<std::string> replaceRandomWords(
    const std::vector<std::string>& words1,
    const std::vector<std::string>& words2,
    int n,
    double p
) {
    // Validate inputs
    if (words1.empty() || words2.empty() || n <= 0 || p < 0.0 || p > 1.0) {
        throw std::invalid_argument("Invalid input parameters.");
    }
    
    // Create a vector of n strings from words1
    std::vector<std::string> result;
    int numWords = std::min(static_cast<int>(words1.size()), n);
    result.reserve(n);
    for (int i = 0; i < numWords; ++i) {
        result.push_back(words1[i]);
    }
    
    // Calculate the number of words to replace
    int numReplacements = static_cast<int>(n * p);
    numReplacements = std::min(numReplacements, static_cast<int>(words2.size()));
    
    // Seed for random number generation
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, static_cast<int>(result.size()) - 1);
    
    // Replace random words from words2
    for (int i = 0; i < numReplacements; ++i) {
        int index = dis(gen);
        result[index] = words2[dis(gen) % words2.size()];
    }
    
    return result;
}

int main() {
    // Example usage
    std::vector<std::string> words1 = {"apple", "banana", "cherry", "date", "elderberry"};
    std::vector<std::string> words2 = {"fig", "grape", "honeydew", "kiwi", "lemon"};
    
    try {
        std::vector<std::string> result = replaceRandomWords(words1, words2, 5, 0.4);
        
        for (const std::string& word : result) {
            std::cout << word << " ";
        }
        std::cout << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    
    return 0;
}
