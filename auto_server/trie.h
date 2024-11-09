#ifndef TRIE_H
#define TRIE_H

#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <string>
#include <algorithm>
#include <map>

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

// Suffix Trie Node structure
class SuffixTrieNode {
public:
    unordered_map<char, SuffixTrieNode*> children;
    bool isEndOfWord;

    SuffixTrieNode() {
        isEndOfWord = false;
    }
};

// Suffix Trie Class
class SuffixTrie {
private:
    SuffixTrieNode* root;

    // Insert all suffixes of a given word into the suffix trie
    void insertSuffixes(const string& word) {
        for (int i = 0; i < word.size(); ++i) {
            SuffixTrieNode* current = root;
            for (int j = i; j < word.size(); ++j) {
                if (current->children.find(word[j]) == current->children.end()) {
                    current->children[word[j]] = new SuffixTrieNode();
                }
                current = current->children[word[j]];
            }
            current->isEndOfWord = true;  // Mark end of each suffix
        }
    }

public:
    SuffixTrie() {
        root = new SuffixTrieNode();
    }

    // Build the suffix trie for a word
    void buildSuffixTrie(const string& word) {
        insertSuffixes(word);
    }

    // Search for a suffix in the trie
    bool searchSuffix(const string& suffix)const {
        SuffixTrieNode* current = root;
        for (char ch : suffix) {
            if (current->children.find(ch) == current->children.end()) {
                return false;
            }
            current = current->children[ch];
        }
        return true;
    }
};

// Trie Class
class Trie {
private:
    TrieNode* root;
    unordered_map<string, SuffixTrie> suffixTries;  // Store suffix tries for each word

    // Helper function to recursively find words with a given prefix
    void findAllWords(TrieNode* node, string prefix, vector<string>& result) {
        if (node->isEndOfWord) {
            result.push_back(prefix);
        }
        for (auto child : node->children) {
            findAllWords(child.second, prefix + child.first, result);
        }
    }

    // Helper function to recursively search for infix matches
    void findInfixMatches(TrieNode* node, const string& infix, string currentWord, vector<string>& result) {
        if (node->isEndOfWord && currentWord.find(infix) != string::npos) {
            result.push_back(currentWord);
        }
        for (auto& child : node->children) {
            findInfixMatches(child.second, infix, currentWord + child.first, result);
        }
    }

    // Helper function to generate all suffixes of a word
    void generateSuffixes(TrieNode* node, const string& word, vector<string>& result) {
        if (node->isEndOfWord) {
            result.push_back(word);
        }
        for (auto& child : node->children) {
            generateSuffixes(child.second, word + child.first, result);
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

        // Build a suffix trie for this word
        SuffixTrie suffixTrie;
        suffixTrie.buildSuffixTrie(word);
        suffixTries[word] = suffixTrie;
    }

    // Get all words in the Trie with a given prefix
    vector<string> autocomplete(const string& pattern) {
    vector<string> prefixResults;
    vector<string> suffixResults;
    vector<string> infixResults;
    unordered_map<string, int> seen;  // To track word frequency for better ordering

    // Prefix Suggestions
    TrieNode* current = root;
    bool isPrefix = true;
    for (char ch : pattern) {
        if (current->children.find(ch) == current->children.end()) {
            isPrefix = false;
            break;
        }
        current = current->children[ch];
    }

    // Collect results for prefix suggestions
    if (isPrefix) {
        findAllWords(current, pattern, prefixResults);
        for (const string& word : prefixResults) {
            seen[word] = root->children[pattern[0]]->frequency;
        }
    }

    // Suffix Search
    for (const auto& wordSuffixPair : suffixTries) {
        if (wordSuffixPair.second.searchSuffix(pattern)) {
            suffixResults.push_back(wordSuffixPair.first);
            if (seen.find(wordSuffixPair.first) == seen.end()) {
                seen[wordSuffixPair.first] = root->children[wordSuffixPair.first[0]]->frequency;
            }
        }
    }

    // Infix Search (only add words that are not already in prefix or suffix results)
    vector<string> infixCandidates;
    findInfixMatches(root, pattern, "", infixCandidates);
    
    for (const string& word : infixCandidates) {
        // Add word to infixResults if it's not already in prefix or suffix results
        if (seen.find(word) == seen.end()) {
            infixResults.push_back(word);
            seen[word] = root->children[word[0]]->frequency;
        }
    }

    // Combine results: Prefix, Suffix, and Infix
    vector<string> result;
    result.insert(result.end(), prefixResults.begin(), prefixResults.end());
    result.insert(result.end(), suffixResults.begin(), suffixResults.end());
    result.insert(result.end(), infixResults.begin(), infixResults.end());

    // Remove duplicates while preserving order
    vector<string> uniqueResults;
    for (const string& word : result) {
        if (seen.find(word) != seen.end() && seen[word] > 0) {  // Word frequency check
            uniqueResults.push_back(word);
            seen[word] = 0; // Mark as added
        }
    }

    return uniqueResults;
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

    // Find words that have a given infix
    vector<string> findWordsWithInfix(const string& infix) {
        vector<string> result;
        findInfixMatches(root, infix, "", result);
        return result;
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
