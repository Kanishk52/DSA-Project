#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <string>
#include <algorithm>
#include <memory>
#include <cctype>

using namespace std;

class TrieNode {
public:
    unordered_map<char, unique_ptr<TrieNode>> children;
    bool isEndOfWord;
    int frequency;

    TrieNode() : isEndOfWord(false), frequency(0) {}
};

class Trie {
private:
    unique_ptr<TrieNode> root;
    const size_t MAX_SUGGESTIONS = 10;

    void findAllWords(TrieNode* node, const string& prefix, 
                      vector<pair<string, int>>& result) const {
        if (node->isEndOfWord) {
            result.emplace_back(prefix, node->frequency);
        }
        for (const auto& child : node->children) {
            findAllWords(child.second.get(), prefix + child.first, result);
        }
    }

public:
    Trie() : root(make_unique<TrieNode>()) {}

    void insert(const string& word, int freq = 1) {
        TrieNode* current = root.get();
        for (char ch : word) {
            char normalized = tolower(ch);
            if (current->children.find(normalized) == current->children.end()) {
                current->children[normalized] = make_unique<TrieNode>();
            }
            current = current->children[normalized].get();
        }
        current->isEndOfWord = true;
        current->frequency += freq;
    }

    vector<string> autocomplete(const string& prefix) const {
        vector<pair<string, int>> suggestions;
        TrieNode* current = root.get();
        string normalizedPrefix = prefix;
        for (char& ch : normalizedPrefix) ch = tolower(ch);

        for (char ch : normalizedPrefix) {
            if (current->children.find(ch) == current->children.end()) {
                return {};
            }
            current = current->children[ch].get();
        }
        
        findAllWords(current, normalizedPrefix, suggestions);
        sort(suggestions.begin(), suggestions.end(),
             [](const auto& a, const auto& b) {
                 return a.second > b.second || 
                        (a.second == b.second && a.first < b.first);
             });

        vector<string> result;
        for (size_t i = 0; i < min(suggestions.size(), MAX_SUGGESTIONS); ++i) {
            result.push_back(suggestions[i].first);
        }
        return result;
    }
};

void loadWordsFromFile(Trie& trie, const string& filename) {
    ifstream file(filename);
    string line;
    if (file.is_open()) {
        while (getline(file, line)) {
            if (line.empty()) continue;
            size_t comma = line.find(',');
            if (comma != string::npos) {
                string word = line.substr(0, comma);
                int freq = stoi(line.substr(comma + 1));
                trie.insert(word, freq);
            } else {
                trie.insert(line);
            }
        }
        file.close();
    } else {
        throw runtime_error("Could not open file: " + filename);
    }
}

int main() {
    Trie trie;
    loadWordsFromFile(trie, "dictionary.txt");

    ifstream inputFile("input.txt");
    string prefix;
    getline(inputFile, prefix);
    inputFile.close();

    vector<string> suggestions = trie.autocomplete(prefix);

    ofstream outputFile("output.txt");
    for (const string& suggestion : suggestions) {
        outputFile << suggestion << endl;
    }
    outputFile.close();

    return 0;
}
