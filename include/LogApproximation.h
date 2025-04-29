

#pragma once

#include <vector>
#include <stdexcept>
#include <utility> // For std::pair

// Basic structure to represent the unweighted approximation tree
struct ApproxTree {
    int num_nodes = 0;
    // Adjacency list for the unweighted tree.
    std::vector<std::vector<int>> adj_list;
};

class LogApproxHyperbolicity {
public:
    // ***** MODIFIED CONSTRUCTOR *****
    // Takes both the distance matrix AND the original graph's adjacency list
    explicit LogApproxHyperbolicity(
        const std::vector<std::vector<double>>& dist_matrix,
        const std::vector<std::vector<std::pair<int, double>>>& graph_adj_list
    );

    // Computes the O(log n)-approximate hyperbolicity value
    double compute_approximate_hyperbolicity();

private:
    // Reference to the original graph's distance matrix
    const std::vector<std::vector<double>>& distances;
    // ***** NEW MEMBER *****
    // Reference to the original graph's adjacency list
    const std::vector<std::vector<std::pair<int, double>>>& original_adj_list;

    int n; // Number of nodes

    // --- Implementation replaces the stub ---
    ApproxTree construct_approximation_tree() const;
    // ---

    // Computes all-pairs shortest paths (edge counts) within the given unweighted tree T
    std::vector<std::vector<double>> compute_tree_distances(const ApproxTree& tree) const;

    // Helper function: Performs BFS from a source node in the unweighted tree T
    std::vector<double> bfs_tree(int source, const ApproxTree& tree) const;
};