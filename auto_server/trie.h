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

    TrieNode() {
        isEndOfWord = false;
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
    void insert(const string& word) {
        TrieNode* current = root;
        for (char ch : word) {
            if (current->children.find(ch) == current->children.end()) {
                current->children[ch] = new TrieNode();
            }
            current = current->children[ch];
        }
        current->isEndOfWord = true;
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
};

// Function to load words from the file into the Trie
void loadWordsFromFile(Trie& trie, const string& filename) {
    ifstream file(filename);
    string word;
    if (file.is_open()) {
        while (file >> word) {
            trie.insert(word);
        }
        file.close();
    } else {
        cout << "Error: Could not open file '" << filename << "'\n";
    }
}

#endif // TRIE_H
