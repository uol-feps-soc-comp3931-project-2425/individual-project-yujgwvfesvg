// Exact Algorithm
//----------
#include "Hyperbolicity.h"
#include <algorithm>
#include <limits>
#include <stdexcept>
#include <functional>
#include <set>
#include <omp.h>

constexpr double INF = std::numeric_limits<double>::infinity();

HyperbolicityCalculator::HyperbolicityCalculator(const std::vector<std::vector<double>>& dist_matrix)
    : distances(dist_matrix) {
    int n = distances.size();
    for (int i = 0; i < n; ++i) {
        if (distances[i].size() != n) {
            throw std::invalid_argument("Distance matrix must be square.");
        }
        if (distances[i][i] != 0.0) {
            throw std::invalid_argument("Diagonal entries must be zero.");
        }
        
    }
}

std::vector<std::vector<double>> HyperbolicityCalculator::gromov_product_matrix(int r) const {
    int n = distances.size();
    std::vector<std::vector<double>> A(n, std::vector<double>(n));
    

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            // A[i][j] = 0.5 * (distances[i][r] + distances[r][j] - distances[i][j]);
            // Assuming distances matrix is valid and square
            double dr_i = distances[r][i];
            double dr_j = distances[r][j];
            double di_j = distances[i][j];

            // Check if ALL relevant distances are finite
            if ((dr_i)!=INF && (dr_j)!=INF && (di_j)!=INF)
            {
                // If all finite, compute the Gromov product
                A[i][j] = 0.5 * (dr_i + dr_j - di_j);
                // Optional: check for NaN result from potential 0*INF if distances were non-numeric?
                
            }
            else
            {
                // If any distance is INF, explicitly set Gromov product to indicate disconnection
                A[i][j] = -INF;
            }
        }
    }
    
    return A;
}

std::vector<std::vector<double>> HyperbolicityCalculator::max_min_product(
    const std::vector<std::vector<double>>& A
) const {
    int n = A.size();
    if (n == 0) return {};
     if (A[0].size() != n) {
         throw std::invalid_argument("Input matrix A must be square.");
    }

    constexpr double N_INF = -std::numeric_limits<double>::infinity();
    std::vector<std::vector<double>> result(n, std::vector<double>(n, N_INF));

    // Parallelize the outer loop 'i'
    #pragma omp parallel for schedule(static)
    for (int i = 0; i < n; ++i) {
        for (int k = 0; k < n; ++k) {
             double Aik = A[i][k];
            for (int j = 0; j < n; ++j) {
                double min_val = std::min(Aik, A[k][j]);
                result[i][j] = std::max(result[i][j], min_val);
            }
        }
    }
    return result;
}


double HyperbolicityCalculator::compute_for_base(int r) {
    auto A = gromov_product_matrix(r);
    auto A_sq = max_min_product(A);
    double max_delta = 0.0;
    
    for (auto i = 0; i < A.size(); ++i) {
        for (auto j = 0; j < A.size(); ++j) {

            double a_ij = A[i][j];
            double a_sq_ij = A_sq[i][j];

            
            if ((a_ij)!=INF && (a_sq_ij)!=INF)
            {

                max_delta = std::max(max_delta, a_sq_ij - a_ij);
            }
        }
    }
    return max_delta;
}

double HyperbolicityCalculator::compute_exact_hyperbolicity() {
    int n = distances.size();
    double max_hyperbolicity = 0.0;

    // #pragma omp parallel for reduction(max:max_hyperbolicity) schedule(dynamic)
    for (int r = 0; r < n; ++r) {
        double delta_r = compute_for_base(r);
        max_hyperbolicity = std::max(max_hyperbolicity, delta_r);
    }

    return max_hyperbolicity;
}

