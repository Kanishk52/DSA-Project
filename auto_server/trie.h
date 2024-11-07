#ifndef TRIE_H
#define TRIE_H

#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <string>

using namespace std;

// Trie Node structure
class TrieNode {
public:
    unordered_map<char, TrieNode*> children;
    bool isEndOfWord;
    int frequency;  // Track word frequency

    TrieNode() {
        isEndOfWord = false;
        frequency = 0;
    }
};

// Trie Class
class Trie {
private:
    TrieNode* root;

    // Helper function to recursively find words with a given prefix
    void findAllWords(TrieNode* node, string prefix, vector<string>& result) {
        if (node->isEndOfWord) {
            result.push_back(prefix);
        }
        for (auto child : node->children) {
            findAllWords(child.second, prefix + child.first, result);
        }
    }

public:
    Trie() {
        root = new TrieNode();
    }

    // Insert a word into the Trie
    void insert(const string& word, int freq = 1) {
        TrieNode* current = root;
        for (char ch : word) {
            if (current->children.find(ch) == current->children.end()) {
                current->children[ch] = new TrieNode();
            }
            current = current->children[ch];
        }
        current->isEndOfWord = true;
        current->frequency += freq;  // Increment frequency
    }

    // Get all words in the Trie with a given prefix
    vector<string> autocomplete(const string& prefix) {
        TrieNode* current = root;
        vector<string> result;

        // Traverse the Trie up to the end of the prefix
        for (char ch : prefix) {
            if (current->children.find(ch) == current->children.end()) {
                return result; // If prefix is not found, return empty result
            }
            current = current->children[ch];
        }

        // Find all words starting with the given prefix
        findAllWords(current, prefix, result);
        return result;
    }

    // Check if a word exists in the Trie
    bool isWord(const string& word) {
        TrieNode* current = root;
        for (char ch : word) {
            if (current->children.find(ch) == current->children.end()) {
                return false;
            }
            current = current->children[ch];
        }
        return current->isEndOfWord;
    }

    // Destructor to clean up the Trie
    ~Trie() {
        deleteTrie(root);
    }

private:
    // Helper function to delete the Trie nodes
    void deleteTrie(TrieNode* node) {
        for (auto& pair : node->children) {
            deleteTrie(pair.second);
        }
        delete node;
    }
};

// Function to load words from file with frequency information
void loadWordsFromFile(Trie& trie, const string& filename) {
    ifstream file(filename);
    string line;
    if (file.is_open()) {
        while (getline(file, line)) {
            // Assuming format: "word frequency" (if frequency is not provided, default to 1)
            size_t space_pos = line.find(' ');
            if (space_pos != string::npos) {
                string word = line.substr(0, space_pos);
                int frequency = stoi(line.substr(space_pos + 1));
                trie.insert(word, frequency);
            } else {
                trie.insert(line, 1);
            }
        }
        file.close();
    } else {
        cout << "Error: Could not open file '" << filename << "'\n";
    }
}

#endif // TRIE_H