#include "solver.h"
#include <iostream>
#include <iomanip>
#include <cmath>
#include <numeric>

Solver::Solver(const std::unordered_map<std::string, Product>& products,
               const GlobalConstraints& globalConstraints,
               const std::vector<Objective>& objectives)
    : products(products), globalConstraints(globalConstraints), objectives(objectives) {}

void Solver::solve() {
    setupModel();
    applyConstraints();
    defineObjectiveFunction();
    model.primal();
    displayResults();
    validateSolution();
    performSensitivityAnalysis();
}

void Solver::setupModel() {
    size_t numProducts = products.size();
    model.resize(0, numProducts);

    objectiveCoefficients.resize(numProducts, 0.0);
    lowerBounds.resize(numProducts, 0.0);
    upperBounds.resize(numProducts, 0.0);
    avgCosts.resize(numProducts, 0.0);
    avgManHours.resize(numProducts, 0.0);

    size_t index = 0;
    for (const auto& [name, product] : products) {
        avgCosts[index] = (product.costMin + product.costMax) / 2.0;
        double avgProfit = (product.profitMin + product.profitMax) / 200.0;
        avgManHours[index] = (product.manHourPerUnitMin + product.manHourPerUnitMax) / 2.0;

        objectiveCoefficients[index] = avgCosts[index] * avgProfit;
        lowerBounds[index] = product.demandMin;
        upperBounds[index] = product.demandMax;

        model.setColumnLower(index, lowerBounds[index]);
        model.setColumnUpper(index, upperBounds[index]);
        ++index;
    }
}

void Solver::applyConstraints() {
    int index = 0;
    for (const auto& [name, product] : products) {
        double avgCost = avgCosts[index];
        double avgManHour = avgManHours[index];
        int columnIndex = index;

        model.addRow(1, &columnIndex, &avgCost, product.budgetMin, product.budgetMax);
        model.addRow(1, &columnIndex, &avgManHour, 0.0, product.totalManHoursMax);
        ++index;
    }

    std::vector<int> indices(products.size());
    std::iota(indices.begin(), indices.end(), 0);

    model.addRow(indices.size(), indices.data(), avgCosts.data(), globalConstraints.budgetMin, globalConstraints.budgetMax);
    model.addRow(indices.size(), indices.data(), avgManHours.data(), 0.0, globalConstraints.manHoursMax);
}

void Solver::defineObjectiveFunction() {
    double weightProfit = 0.0;
    double weightResource = 0.0;
    double weightBudget = 0.0;

    for (const auto& objective : objectives) {
        if (objective.name == "profit" && objective.type == "maximize") {
            weightProfit = 1.0 / objective.rank;
        } else if (objective.name == "resource_usage" && objective.type == "minimize") {
            weightResource = 1.0 / objective.rank;
        } else if (objective.name == "budget_usage" && objective.type == "maximize") {
            weightBudget = 1.0 / objective.rank;
        }
    }

    for (size_t i = 0; i < objectiveCoefficients.size(); ++i) {
        model.setObjectiveCoefficient(i, weightProfit * objectiveCoefficients[i] -
                                           weightResource * avgManHours[i] +
                                           weightBudget * avgCosts[i]);
    }

    model.setOptimizationDirection(1);
}

void Solver::displayResults() {
    std::cout << "\nOptimal solution found:\n";
    std::cout << std::setw(10) << "Product" << std::setw(15) << "Cost Picked" << std::setw(15) << "Profit %"
              << std::setw(15) << "Profit Value" << std::setw(15) << "Units" << std::setw(15) << "Man Hours"
              << std::setw(15) << "Budget Used" << "\n";
    std::cout << std::string(105, '-') << "\n";

    const double* solution = model.getColSolution();
    size_t index = 0;
    double totalProfit = 0.0;
    double totalBudgetUsed = 0.0;
    double totalManHoursUsed = 0.0;

    for (const auto& [name, product] : products) {
        double costPicked = avgCosts[index];
        double profitPercent = (product.profitMin + product.profitMax) / 2.0;
        double unitsProduced = solution[index];
        double manHoursUsed = unitsProduced * avgManHours[index];
        double budgetUsed = unitsProduced * costPicked;
        double profitValue = unitsProduced * costPicked * (profitPercent / 100.0);

        totalProfit += profitValue;
        totalBudgetUsed += budgetUsed;
        totalManHoursUsed += manHoursUsed;

        std::cout << std::setw(10) << name << std::setw(15) << costPicked << std::setw(15) << profitPercent
                  << std::setw(15) << profitValue << std::setw(15) << unitsProduced << std::setw(15) << manHoursUsed
                  << std::setw(15) << budgetUsed << "\n";
        ++index;
    }

    double overallProfitPercent = (totalProfit / totalBudgetUsed) * 100;

    std::cout << std::string(105, '-') << "\n";
    std::cout << std::setw(10) << "Total" << std::setw(15) << "-" << std::setw(15) << overallProfitPercent
              << std::setw(15) << totalProfit << std::setw(15) << "-" << std::setw(15) << totalManHoursUsed
              << std::setw(15) << totalBudgetUsed << "\n";
}

void Solver::validateSolution() {
    std::cout << "\nValidating solution:\n";
    const double* solution = model.getColSolution();
    size_t index = 0;

    double totalBudgetUsed = 0.0;
    double totalManHoursUsed = 0.0;

    for (const auto& [name, product] : products) {
        double costPicked = avgCosts[index];
        double unitsProduced = solution[index];
        double manHoursUsed = unitsProduced * avgManHours[index];
        double budgetUsed = unitsProduced * costPicked;

        totalBudgetUsed += budgetUsed;
        totalManHoursUsed += manHoursUsed;

        if (budgetUsed < product.budgetMin || budgetUsed > product.budgetMax) {
            std::cerr << "Budget constraint violated for product: " << name << "\n";
        }
        if (manHoursUsed > product.totalManHoursMax) {
            std::cerr << "Man-hours constraint violated for product: " << name << "\n";
        }
        ++index;
    }

    if (totalBudgetUsed > globalConstraints.budgetMax) {
        std::cerr << "Global budget constraint violated.\n";
    }
    if (totalManHoursUsed > globalConstraints.manHoursMax) {
        std::cerr << "Global man-hours constraint violated.\n";
    }
    std::cout << "Validation complete.\n";
}

void Solver::performSensitivityAnalysis() {
    std::cout << "\nPerforming sensitivity analysis:\n";

    for (double delta = -10; delta <= 10; delta += 5) {
        if (delta == 0) continue;

        std::cout << "\nAdjusting profit percentage by " << delta << "%:\n";
        size_t index = 0;
        for (const auto& [name, product] : products) {
            double adjustedProfitPercent = ((product.profitMin + product.profitMax) / 2.0) * (1 + delta / 100.0);
            double costPicked = avgCosts[index];
            double unitsProduced = model.getColSolution()[index];
            double profitValue = unitsProduced * costPicked * (adjustedProfitPercent / 100.0);

            std::cout << "Product: " << name << " - Adjusted Profit Value: " << profitValue << "\n";
            ++index;
        }
    }
    std::cout << "Sensitivity analysis complete.\n";
}

