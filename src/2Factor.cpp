#include "2Factor.h"
#include "Hyperbolicity.h"
#include <algorithm>
#include <cmath>

// Constructor
TwoFactorApproximation::TwoFactorApproximation(const std::vector<std::vector<double>>& dist_matrix)
    : distances(dist_matrix) {}

// Compute eccentricity for all nodes (O(n^2))
std::vector<double> TwoFactorApproximation::compute_eccentricities() const {
    int n = distances.size();
    std::vector<double> ecc(n, -INFINITY);
    
    for (int i = 0; i < n; ++i) {
        ecc[i] = *std::max_element(distances[i].begin(), distances[i].end());
    }
    return ecc;
}

// Select node with maximum eccentricity
int TwoFactorApproximation::select_base_heuristic() const {
    auto ecc = compute_eccentricities();
    auto max_it = std::max_element(ecc.begin(), ecc.end());
    return std::distance(ecc.begin(), max_it);  // Index of max eccentricity
}

// Approximation entry point
double TwoFactorApproximation::compute_approximate_hyperbolicity() {
    HyperbolicityCalculator hc(distances);
    int r = select_base_heuristic();
    double delta_r = hc.compute_for_base(r);
    return 2 * delta_r;
}