#include "input.h"
#include "solver.h"
#include <iostream>
#include <vector>

int main() {
    const std::string inputFile = "input.config";
    std::unordered_map<std::string, Product> products;
    GlobalConstraints globalConstraints;
    std::vector<Objective> objectives;

    try {
        // Parse inputs
        products = parseInputConfig(inputFile, globalConstraints, objectives);
        
        // Validate inputs
        if (!validateInput(products, globalConstraints, objectives)) {
            return 1;
        }

        // Display parsed and validated inputs
        std::cout << "Parsed Inputs:\n";
        for (const auto& [name, product] : products) {
            std::cout << "Product: " << name << "\n"
                      << "  Cost Range: [" << product.costMin << ", " << product.costMax << "]\n"
                      << "  Profit Range: [" << product.profitMin << ", " << product.profitMax << "]\n"
                      << "  Demand Range: [" << product.demandMin << ", " << product.demandMax << "]\n"
                      << "  Budget Range: [" << product.budgetMin << ", " << product.budgetMax << "]\n"
                      << "  Man-Hour Per Unit Range: [" << product.manHourPerUnitMin << ", " << product.manHourPerUnitMax << "]\n"
                      << "  Total Man-Hours Range: [" << product.totalManHoursMin << ", " << product.totalManHoursMax << "]\n";
        }

        std::cout << "\nGlobal Constraints:\n"
                  << "  Global Budget Range: [" << globalConstraints.budgetMin << ", " << globalConstraints.budgetMax << "]\n"
                  << "  Global Profit Range: [" << globalConstraints.profitMin << ", " << globalConstraints.profitMax << "]\n"
                  << "  Global Man-Hours Range: [" << globalConstraints.manHoursMin << ", " << globalConstraints.manHoursMax << "]\n";

        std::cout << "\nObjectives:\n";
        for (const auto& objective : objectives) {
            std::cout << "  " << objective.type << "_" << objective.name
                      << " = " << objective.rank << "\n";
        }

        // Initialize and run the solver
        Solver solver(products, globalConstraints, objectives);
        solver.solve();

    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << "\n";
        return 1;
    }

    return 0;
}

