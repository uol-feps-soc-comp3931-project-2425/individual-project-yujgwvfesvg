#pragma once
#include <vector>
#include <utility>

class HyperbolicityCalculator {
public:
    explicit HyperbolicityCalculator(const std::vector<std::vector<double>>& dist_matrix);
    double compute_for_base(int r);
    double compute_exact_hyperbolicity();


    
    
private:
    const std::vector<std::vector<double>>& distances;
    
    std::vector<std::vector<double>> gromov_product_matrix(int r) const;
    std::vector<std::vector<double>> max_min_product(const std::vector<std::vector<double>>& A) const;
    // std::vector<std::vector<double>> small_max_min_product(const std::vector<std::vector<double>>& A,
    // const std::vector<std::vector<double>>& B);
};