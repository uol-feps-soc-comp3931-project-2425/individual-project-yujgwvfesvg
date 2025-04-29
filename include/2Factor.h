#pragma once
#include <vector>

class TwoFactorApproximation {
public:
    explicit TwoFactorApproximation(const std::vector<std::vector<double>>& dist_matrix);
    double compute_approximate_hyperbolicity();

private:
    const std::vector<std::vector<double>>& distances;
    
    // Eccentricity-based heuristic
    int select_base_heuristic() const;
    std::vector<double> compute_eccentricities() const;  // O(n^2) time
};