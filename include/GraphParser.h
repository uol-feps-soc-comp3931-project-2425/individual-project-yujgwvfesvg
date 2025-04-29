#pragma once
#include <string>
#include <vector>
#include <unordered_map>

class GraphParser {
public:
    void parse(const std::string& filename);
    const std::vector<std::vector<std::pair<int, double>>>& get_adjacency_list() const;
    const std::vector<std::string>& get_nodes() const;

private:
    std::unordered_map<std::string, int> node_indices;
    std::vector<std::string> nodes;
    std::vector<std::vector<std::pair<int, double>>> adjacency_list;

    void add_node(const std::string& node);
    void add_edge(const std::string& src, const std::string& dest, double weight);
};