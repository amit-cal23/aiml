// solver.h
#ifndef SOLVER_H
#define SOLVER_H

#include "input.h"
#include <unordered_map>
#include <vector>
#include <ClpSimplex.hpp>

// Function declarations for solver
class Solver {
public:
    Solver(const std::unordered_map<std::string, Product>& products,
           const GlobalConstraints& globalConstraints,
           const std::vector<Objective>& objectives);

    // Run the solver
    void solve();

private:
    // Internal data
    const std::unordered_map<std::string, Product>& products;
    const GlobalConstraints& globalConstraints;
    const std::vector<Objective>& objectives;
    ClpSimplex model;

    // Decision variables
    std::vector<double> objectiveCoefficients;
    std::vector<double> lowerBounds;
    std::vector<double> upperBounds;

    // Average costs and man-hours for each product
    std::vector<double> avgCosts;
    std::vector<double> avgManHours;

    // Helper methods
    void setupModel();
    void applyConstraints();
    void defineObjectiveFunction();
    void displayResults();
    void validateSolution();
    void performSensitivityAnalysis();
};

#endif // SOLVER_H

