# DSA-Project
Autocomplete feature and spelling checker
# Smart Search - README

## Overview
Smart Search is a web-based autocomplete and spelling checker system designed to suggest words as you type and manage a dictionary by adding new words. Built with a `Trie` data structure for efficient prefix-based search and suffix trie support, Smart Search provides fast, accurate suggestions for words that match given prefixes, infixes, or suffixes. This project includes server-side and client-side code with RESTful endpoints for interaction.

## Features
1. **Autocomplete**: Provides word suggestions based on input prefixes, infixes, or suffixes.
2. **Spelling Check**: Validates if a word exists in the dictionary.
3. **Add Word**: Supports adding new words to the dictionary, with persistence to a file.
4. **REST API**:
   - `POST /add-word`: Add a new word to the dictionary.
   - `GET /suggestions?query=<query>`: Fetch suggestions based on input query.
5. **Responsive UI**: The front-end includes a search box and an "Add Word" feature with instant feedback.

## Project Structure
- **`Trie.h`**: Defines the `Trie` and `SuffixTrie` classes with functions for word insertion, autocomplete, infix search, and trie cleanup.
- **Server**: Handles HTTP requests, performs word searches, and responds with suggestions or status updates for added words.
- **Client**: HTML, CSS, and JavaScript for the UI, handling user input, displaying suggestions, and managing dictionary updates.

## Code Structure and Implementation

### Trie Data Structure
- **`Trie` Class**: Core structure for handling word insertion, autocomplete, and word lookup.
  - `insert(word, freq)`: Adds a word to the Trie, optionally setting its frequency.
  - `autocomplete(pattern)`: Retrieves a list of words that match the input prefix, suffix, or infix.
  - `isWord(word)`: Checks if a word is in the Trie.
  - `findWordsWithInfix(infix)`: Finds words that contain a specified infix.

- **`SuffixTrie` Class**: Supports suffix-based autocomplete by constructing a trie for each word's suffixes.
  - `buildSuffixTrie(word)`: Inserts all suffixes of a given word into the trie.
  - `searchSuffix(suffix)`: Checks if a suffix exists within the trie.

### Server-Side Code
- **`handle_request(client_socket)`**: Processes incoming requests:
  - Routes `/add-word` POST requests to add a new word to the dictionary.
  - Routes `/suggestions` GET requests to provide autocomplete suggestions.
- **Persistent Storage**: `loadWordsFromFile()` loads words with frequency from a file into the Trie on startup.
  
### Client-Side Code
- **JavaScript**: 
  - `addNewWord()`: Adds a word to the server's dictionary through an API call.
  - `searchBox.oninput`: Displays autocomplete suggestions by fetching them from the server.
  - Keyboard navigation (arrow keys) and mouse click handling for selecting suggestions.
- **HTML/CSS**: Provides a clean and responsive interface with input fields for searching and adding words.

## Running the Project

### Prerequisites
- C++11 or higher
- Web browser (for testing the UI)

### Setup Instructions
1. **Compile the Server**:
   ```bash
   g++ -o server main.cpp -std=c++11
   ```
2. **Run the Server**:
   ```bash
   ./server
   ```
3. **Access the Application**:
   Open `http://localhost:8080` in a web browser to access the Smart Search interface.

## Usage
1. **Autocomplete**: Start typing in the search box. Suggestions will appear based on the input pattern (prefix, suffix, infix).
2. **Add Word**: Use the input box and "Add Word" button to add new entries to the dictionary. The server responds with success or error messages.

## Technologies
- **C++**: Trie and SuffixTrie implementation
- **HTML/CSS**: User interface styling
- **JavaScript**: Handles input events, fetch API calls
- **HTTP**: Communication between client and server

## Notes
- **Performance**: Efficient lookup and suggestion generation with Trie data structures.
- **Error Handling**: Includes handling for file access errors, network issues, and invalid input.

This project demonstrates a simple yet powerful autocomplete system with efficient data storage, real-time interaction, and an intuitive interface for user engagement.