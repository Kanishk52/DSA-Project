#include <iostream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fstream>
#include "trie.h" // Include the Trie header

using namespace std;

const int PORT = 8080; // Server port
const string WEB_DIR = "./web/"; // Directory for web files

Trie trie; // Global Trie instance

// Function to send a response to the client
void respond(int client_socket, const string& response) {
    send(client_socket, response.c_str(), response.size(), 0);
}

// Function to read a file's content
// Function to read a file's content
string get_file_content(const string& file_path) {
    ifstream file(WEB_DIR + file_path);
    if (!file) {
        cerr << "Error: File not found - " << WEB_DIR + file_path << endl; // Log error
        return ""; // File not found
    }
    stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}


// Function to parse the query from the request
string parse_query(const string& request) {
    size_t query_pos = request.find("query=");
    if (query_pos == string::npos) return ""; // No query parameter
    query_pos += 6; // Move past "query="
    size_t end_pos = request.find(' ', query_pos); // Find space after query
    if (end_pos == string::npos) end_pos = request.find('\r', query_pos); // Handle CR
    return request.substr(query_pos, end_pos - query_pos); // Extract query
}

string get_query_param(const string& request, const string& param) {
    size_t start = request.find(param + "=");
    if (start == string::npos) return "";
    start += param.length() + 1; // Move past "param="
    size_t end = request.find("&", start); // Look for the next parameter or end of string
    if (end == string::npos) end = request.find(" ", start); // Stop at the next space
    return request.substr(start, end - start);
}

// Function to handle incoming client requests
void handle_request(int client_socket) {
    char buffer[1024];
    recv(client_socket, buffer, sizeof(buffer) - 1, 0);
    buffer[sizeof(buffer) - 1] = '\0'; // Null-terminate the buffer

    string request(buffer);
    cout << "Received request: " << request << endl; // Log the entire request

    // Check if the request is a GET request and if it contains the /suggestions path
    size_t pos = request.find("GET ");
    if (pos == string::npos) return; // Invalid request
    pos += 4; // Move past "GET "
    size_t end_pos = request.find(" HTTP/1.1");
    if (end_pos == string::npos) return; // Invalid request format
    string path = request.substr(pos, end_pos - pos); // Extract the requested path

    cout << "Request path: " << path << endl; // Log the parsed path

    if (path.find("/suggestions") == 0) {  // Check if the path starts with "/suggestions"
        // Handle autocomplete suggestions
        string query = get_query_param(request, "query"); // Get the query parameter
        cout << "Query parameter: " << query << endl; // Log the query

        if (query.empty()) {
            string not_found_response = "HTTP/1.1 400 Bad Request\r\nContent-Length: 0\r\n\r\n";
            respond(client_socket, not_found_response);
            close(client_socket);
            return;
        }

        vector<string> suggestions = trie.autocomplete(query); // Get suggestions from Trie

        // Prepare JSON response
        string json_response = "[";
        for (size_t i = 0; i < suggestions.size(); ++i) {
            json_response += "\"" + suggestions[i] + "\"";
            if (i < suggestions.size() - 1) json_response += ",";
        }
        json_response += "]";

        // Respond with suggestions in JSON format
        string response = "HTTP/1.1 200 OK\r\n"
                          "Content-Type: application/json\r\n"
                          "Access-Control-Allow-Origin: *\r\n" // Allow CORS
                          "Content-Length: " + to_string(json_response.size()) + "\r\n\r\n" + json_response;
        respond(client_socket, response);
    } else {
        // Serve static files (HTML, CSS, JS)
        if (path == "/") path = "index.html"; // Default file
        string content = get_file_content(path);
        if (content.empty()) {
            string not_found_response = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n\r\n";
            respond(client_socket, not_found_response);
        } else {
            string response = "HTTP/1.1 200 OK\r\n"
                              "Content-Length: " + to_string(content.size()) + "\r\n\r\n" + content;
            respond(client_socket, response);
        }
    }
    close(client_socket);
}


int main() {
    loadWordsFromFile(trie, WEB_DIR + "dictionary.txt"); // Load words from the dictionary file

    // Create server socket
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        cerr << "Error opening socket" << endl;
        return 1;
    }

    // Set up the server address structure
    sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY; // Listen on all interfaces
    server_addr.sin_port = htons(PORT); // Set port

    // Bind the socket to the address
    if (::bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        cerr << "Error on binding" << endl;
        return 1;
    }

    listen(server_socket, 5); // Start listening for incoming connections
    cout << "Listening on port " << PORT << endl;

    while (true) {
        sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);
        if (client_socket < 0) {
            cerr << "Error on accept" << endl;
            continue;
        }
        handle_request(client_socket); // Handle the client request
    }

    close(server_socket); // Close the server socket
    return 0;
}
