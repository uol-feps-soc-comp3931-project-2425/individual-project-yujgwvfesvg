#include "ShortestPath.h"
#include <queue>
#include <limits>
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <omp.h>

constexpr double INF = std::numeric_limits<double>::infinity();

ShortestPath::ShortestPath(const std::vector<std::vector<std::pair<int, double>>>& adj_list)
    : adjacency_list(adj_list) {}

std::vector<double> ShortestPath::dijkstra(int source) const {
    int n = adjacency_list.size();
    std::vector<double> dist(n, INF);
    std::priority_queue<std::pair<double, int>, 
                        std::vector<std::pair<double, int>>, 
                        std::greater<>> pq;

    dist[source] = 0.0;
    pq.emplace(0.0, source);

    while (!pq.empty()) {
        auto [current_dist, u] = pq.top();
        pq.pop();

        if (current_dist > dist[u]) continue;

        for (const auto& [v, weight] : adjacency_list[u]) {
            if (dist[v] > dist[u] + weight) {
                dist[v] = dist[u] + weight;
                pq.emplace(dist[v], v);
            }
        }
    }
    return dist;
}


std::vector<std::vector<double>> ShortestPath::compute_all_pairs() {
    std::vector<std::vector<double>> dist_matrix;
    int n = adjacency_list.size();
    dist_matrix.resize(n);

    #pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < n; ++i) {
        dist_matrix[i] = dijkstra(i);
    }
    
    return dist_matrix;
}
