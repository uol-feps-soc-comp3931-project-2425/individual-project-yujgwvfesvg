#pragma once
#include <vector>

class ShortestPath {
public:
    explicit ShortestPath(const std::vector<std::vector<std::pair<int, double>>>& adj_list);
    std::vector<std::vector<double>> compute_all_pairs();

private:
    const std::vector<std::vector<std::pair<int, double>>>& adjacency_list;
    std::vector<double> dijkstra(int source) const;
};