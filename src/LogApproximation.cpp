#include "LogApproximation.h"
#include <vector>
#include <cmath>
#include <limits>
#include <queue>
#include <stdexcept>
#include <algorithm>
#include <iostream>
#include <map>          // For component mapping
#include <set>          // For keeping track of visited nodes

constexpr double INF = std::numeric_limits<double>::infinity();

// --- MODIFIED Constructor ---
LogApproxHyperbolicity::LogApproxHyperbolicity(
    const std::vector<std::vector<double>>& dist_matrix,
    const std::vector<std::vector<std::pair<int, double>>>& graph_adj_list)
    : distances(dist_matrix), // Initialize distances reference
      original_adj_list(graph_adj_list) // Initialize adjacency list reference
{
    if (distances.empty()) {
        n = 0;
    } else {
        n = distances.size();
        if (n > 0 && (distances[0].size() != n || original_adj_list.size() != n)) {
             throw std::invalid_argument("LogApproxHyperbolicity: Matrix/Adjacency list size mismatch or not square.");
        }
    }
     if (n != original_adj_list.size()){
          throw std::invalid_argument("LogApproxHyperbolicity: Distance matrix size and adjacency list size do not match.");
     }
}

// --- Chepoi-Dragan Approximation Tree Construction ---
// Implementation based on "A Note on Distance Approximating Trees in Graphs"
ApproxTree LogApproxHyperbolicity::construct_approximation_tree() const {
    if (n == 0) return ApproxTree{0};

    ApproxTree result_tree;
    result_tree.num_nodes = n;
    result_tree.adj_list.resize(n);

    // --- 1. Leveling ---
    int base_node = 0; // Choose an arbitrary base node (e.g., 0)
    std::vector<std::vector<int>> levels;
    std::vector<int> node_level(n, -1);
    int max_level = 0;

    // Ensure distances from base_node are valid
    if (base_node >= distances.size()) {
         throw std::runtime_error("Base node index out of bounds for distance matrix.");
    }

    for (int i = 0; i < n; ++i) {
        double dist = distances[base_node][i];
        if (dist == INF || dist < 0) {
             // Handle disconnected graph or invalid distances if necessary
             // For now, assume graph is connected and distances are valid
             // Assign a large level or handle error
             continue; // Skip nodes unreachable from base_node
        }
        int level = static_cast<int>(std::round(dist)); // Use distances from base node for levels
        if (level >= levels.size()) {
            levels.resize(level + 1);
        }
        levels[level].push_back(i);
        node_level[i] = level;
        max_level = std::max(max_level, level);
    }

    // --- 2. Find Components within Levels & Representatives ---
    // levels_components[k] stores a list of components (each a vector<int>) for level k
    std::vector<std::vector<std::vector<int>>> levels_components(max_level + 1);
    // node_to_component[node_id] -> pair{level, component_index_within_level}
    std::vector<std::pair<int, int>> node_to_component(n, {-1, -1});
    // component_representatives[k][comp_idx] -> representative node id
    std::vector<std::vector<int>> component_representatives(max_level + 1);

    for (int k = 0; k <= max_level; ++k) {
        std::set<int> visited_in_level;
        std::vector<int>& current_level_nodes = levels[k];

        for (int node_in_level : current_level_nodes) {
            if (visited_in_level.find(node_in_level) == visited_in_level.end()) {
                // Start BFS/DFS to find a component within this level
                std::vector<int> current_component;
                std::queue<int> q;

                q.push(node_in_level);
                visited_in_level.insert(node_in_level);

                while (!q.empty()) {
                    int u = q.front();
                    q.pop();
                    current_component.push_back(u);

                     // Bounds check for original_adj_list
                     if (u < 0 || u >= original_adj_list.size()) continue;

                    // Explore neighbors *within the same level k*
                    for (const auto& edge : original_adj_list[u]) {
                        int v = edge.first;
                        // Check if neighbor v is in the same level and not visited yet
                        if (v >= 0 && v < n && node_level[v] == k &&
                            visited_in_level.find(v) == visited_in_level.end())
                        {
                            visited_in_level.insert(v);
                            q.push(v);
                        }
                    }
                }

                // Store component and representative
                if (!current_component.empty()) {
                    int representative = current_component[0]; // Choose first node as representative
                    int component_idx = levels_components[k].size();
                    levels_components[k].push_back(current_component);
                    component_representatives[k].push_back(representative);
                    for (int node : current_component) {
                        node_to_component[node] = {k, component_idx};
                    }
                }
            }
        }
    }

    // --- 3. Determine Component Adjacency & Build Tree ---
    // component_parent_rep[k][comp_idx] -> representative of parent component in level k-1
    std::vector<std::map<int, int>> component_parent_rep(max_level + 1);

    for (int k = 1; k <= max_level; ++k) {
        for (int comp_idx = 0; comp_idx < levels_components[k].size(); ++comp_idx) {
            int parent_rep = -1;
            bool parent_found = false;
            // Find the parent component representative in level k-1
            for (int node_in_comp : levels_components[k][comp_idx]) {
                 // Bounds check
                 if (node_in_comp < 0 || node_in_comp >= original_adj_list.size()) continue;

                for (const auto& edge : original_adj_list[node_in_comp]) {
                    int neighbor = edge.first;
                    if (neighbor >= 0 && neighbor < n && node_level[neighbor] == k - 1) {
                        // Found a connection to the previous level
                        auto [parent_level, parent_comp_idx] = node_to_component[neighbor];
                        if (parent_level == k - 1) {
                            parent_rep = component_representatives[k - 1][parent_comp_idx];
                            parent_found = true;
                            break; // Found the unique parent representative
                        }
                    }
                }
                if (parent_found) break;
            }

            if (!parent_found && !levels_components[k][comp_idx].empty()) {
                 // This shouldn't happen in a connected graph according to the paper's claim
                 std::cerr << "Warning: Component at level " << k << " index " << comp_idx << " has no parent component in level " << k-1 << "?" << std::endl;
                 // Handle error or continue, potentially resulting in disconnected tree
                 continue;
            }

            // Add inter-component edge T = (v_Q, v_{parent(Q)})
            int current_rep = component_representatives[k][comp_idx];
            if(current_rep != -1 && parent_rep != -1) {
                 result_tree.adj_list[current_rep].push_back(parent_rep);
                 result_tree.adj_list[parent_rep].push_back(current_rep);
            }

            // Add intra-component edges T = (v, v_Q) for v in Q, v != v_Q
             for (int node_in_comp : levels_components[k][comp_idx]) {
                 if (node_in_comp != current_rep && current_rep != -1) {
                      result_tree.adj_list[current_rep].push_back(node_in_comp);
                      result_tree.adj_list[node_in_comp].push_back(current_rep);
                 }
             }
        }
         // Connect components within level 0 (should only be one if graph connected from base_node)
         if (k == 1) {
              for(int comp_idx_k0 = 0; comp_idx_k0 < levels_components[0].size(); ++comp_idx_k0){
                   int rep_k0 = component_representatives[0][comp_idx_k0];
                    for (int node_in_comp_k0 : levels_components[0][comp_idx_k0]) {
                         if (node_in_comp_k0 != rep_k0 && rep_k0 != -1) {
                              result_tree.adj_list[rep_k0].push_back(node_in_comp_k0);
                              result_tree.adj_list[node_in_comp_k0].push_back(rep_k0);
                         }
                    }
              }
         }
    }


    return result_tree;
}


// --- BFS for Tree Distances (Unweighted) ---
// [Keep the bfs_tree function from the previous response - no changes needed here]
std::vector<double> LogApproxHyperbolicity::bfs_tree(int source, const ApproxTree& tree) const {
    // ... (implementation remains the same as before) ...
    if (source < 0 || source >= tree.num_nodes) {
        throw std::out_of_range("LogApproxHyperbolicity::bfs_tree: Source node index out of range.");
    }
    if (tree.adj_list.size() != tree.num_nodes) {
         throw std::runtime_error("LogApproxHyperbolicity::bfs_tree: Tree adjacency list size mismatch.");
    }


    std::vector<double> dist(tree.num_nodes, INF);
    std::queue<int> q;

    if (source < tree.num_nodes) { // Check if source is valid before accessing
        dist[source] = 0.0;
        q.push(source);
    } else {
         return dist; // Source out of bounds
    }


    while (!q.empty()) {
        int u = q.front();
        q.pop();

        // Check bounds for adjacency list access
         if (u < 0 || u >= tree.adj_list.size()) {
             std::cerr << "Warning: Invalid node index " << u << " encountered during BFS." << std::endl;
             continue; // Skip this invalid node
         }

        for (int v : tree.adj_list[u]) {
             // Check bounds for neighbor index
             if (v < 0 || v >= tree.num_nodes) {
                   std::cerr << "Warning: Invalid neighbor index " << v << " for node " << u << " during BFS." << std::endl;
                   continue; // Skip this invalid neighbor
             }

            // If neighbor v hasn't been visited yet
            if (dist[v] == INF) {
                dist[v] = dist[u] + 1.0; // Increment distance by 1 (unweighted edge)
                q.push(v);
            }
        }
    }
    return dist;
}


// --- Compute All-Pairs Shortest Paths in Tree ---
// [Keep the compute_tree_distances function - no changes needed here]
std::vector<std::vector<double>> LogApproxHyperbolicity::compute_tree_distances(const ApproxTree& tree) const {
     // ... (implementation remains the same as before) ...
     if (tree.num_nodes != n) {
          throw std::runtime_error("LogApproxHyperbolicity::compute_tree_distances: Tree node count mismatch.");
     }
     if (n == 0) return {}; // Handle empty graph case

    std::vector<std::vector<double>> tree_dist_matrix(n, std::vector<double>(n, INF));

    // Parallelize this loop if performance is critical and n is large enough
    // #pragma omp parallel for // Requires OpenMP setup in your project
    for (int i = 0; i < n; ++i) {
        tree_dist_matrix[i] = bfs_tree(i, tree);
    }
    return tree_dist_matrix;
}

// --- Main Computation Logic ---
// [Keep the compute_approximate_hyperbolicity function - no changes needed here]
double LogApproxHyperbolicity::compute_approximate_hyperbolicity() {
     // ... (implementation remains the same, calls the new construct_approximation_tree) ...
     if (n <= 1) {
          return 0.0; // Hyperbolicity is typically 0 for graphs with 0 or 1 node
     }

    // 1. Construct the approximation tree T
    ApproxTree approx_tree = construct_approximation_tree(); // Calls the *new* implementation


     // Basic check after construction
      if (approx_tree.num_nodes != n) {
           throw std::runtime_error("LogApproxHyperbolicity: Constructed tree node count does not match original graph.");
      }

    // 2. Compute all-pairs shortest paths (edge counts) within the tree T
    std::vector<std::vector<double>> tree_distances = compute_tree_distances(approx_tree);

    // 3. Find the maximum absolute difference 'm' between graph distances and tree distances
    double max_diff = 0.0;
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
             // Bounds check
             if (i >= distances.size() || j >= distances[i].size() ||
                 i >= tree_distances.size() || j >= tree_distances[i].size()) {
                   throw std::out_of_range("LogApproxHyperbolicity: Index out of bounds when comparing distances.");
             }

            double dist_g = distances[i][j];
            double dist_t = tree_distances[i][j];

            if (dist_g != INF && dist_t != INF) {
                 max_diff = std::max(max_diff, std::abs(dist_g - dist_t));
            }
        }
    }

    // 4. Return 4 * m as the approximation
    return std::max(0.0, 4.0 * max_diff);
}