#include "GraphParser.h"
#include <fstream>
#include <sstream>
#include <algorithm>

void GraphParser::add_node(const std::string& node) {
    if (node_indices.find(node) == node_indices.end()) {
        node_indices[node] = nodes.size();
        nodes.push_back(node);
        adjacency_list.resize(nodes.size());
    }
}

void GraphParser::add_edge(const std::string& src, const std::string& dest, double weight) {
    add_node(src);
    add_node(dest);
    int u = node_indices[src];
    int v = node_indices[dest];
    adjacency_list[u].emplace_back(v, weight);
    adjacency_list[v].emplace_back(u, weight); 
}

void GraphParser::parse(const std::string& filename) {
    std::ifstream file(filename);
    std::string line;
    
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string src, dest;
        double weight;
        char comma;
        
        if (std::getline(iss, src, ',') &&
            std::getline(iss, dest, ',') &&
            (iss >> weight)) {
            add_edge(src, dest, weight);
        }
    }
}

const std::vector<std::vector<std::pair<int, double>>>& GraphParser::get_adjacency_list() const {
    return adjacency_list;
}

const std::vector<std::string>& GraphParser::get_nodes() const {
    return nodes;
}