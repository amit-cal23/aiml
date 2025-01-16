#include "input.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <vector>
#include <string>

// Helper to trim spaces
std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t");
    size_t end = str.find_last_not_of(" \t");
    return (start == std::string::npos || end == std::string::npos) ? "" : str.substr(start, end - start + 1);
}

// Helper to split a range value
std::pair<double, double> parseRange(const std::string& value) {
    std::istringstream ss(value);
    std::string min, max;
    if (std::getline(ss, min, ',') && std::getline(ss, max, ',')) {
        return {std::stod(trim(min)), std::stod(trim(max))};
    }
    throw std::invalid_argument("Invalid range format: " + value);
}

// Parse the input config
std::unordered_map<std::string, Product> parseInputConfig(const std::string& filename, GlobalConstraints& globalConstraints, std::vector<Objective>& objectives) {
    std::unordered_map<std::string, Product> products;
    std::ifstream file(filename);

    if (!file.is_open()) {
        throw std::runtime_error("Failed to open input file: " + filename);
    }

    std::string line, currentSection;
    Product currentProduct;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue; // Skip empty lines and comments

        if (line[0] == '[' && line.back() == ']') {
            // New section
            if (!currentSection.empty() && currentSection != "Global" && currentSection != "Objectives") {
                products[currentProduct.name] = currentProduct;
            }
            currentSection = line.substr(1, line.size() - 2);
            if (currentSection != "Global" && currentSection != "Objectives") {
                currentProduct = Product{};
            }
            continue;
        }

        size_t delimiterPos = line.find('=');
        if (delimiterPos == std::string::npos) {
            throw std::invalid_argument("Invalid config line: " + line);
        }

        std::string key = trim(line.substr(0, delimiterPos));
        std::string value = trim(line.substr(delimiterPos + 1));

        // Parse global constraints
        if (currentSection == "Global") {
            if (key == "global_budget") {
                std::tie(globalConstraints.budgetMin, globalConstraints.budgetMax) = parseRange(value);
            } else if (key == "global_profit") {
                std::tie(globalConstraints.profitMin, globalConstraints.profitMax) = parseRange(value);
            } else if (key == "global_man_hours") {
                std::tie(globalConstraints.manHoursMin, globalConstraints.manHoursMax) = parseRange(value);
            }
        } else if (currentSection == "Objectives") {
            // Parse objectives
            size_t spacePos = key.find('_');
            if (spacePos == std::string::npos) {
                throw std::invalid_argument("Invalid objective key: " + key);
            }

            std::string type = key.substr(0, spacePos);
            std::string name = key.substr(spacePos + 1);

            if (type != "maximize" && type != "minimize") {
                throw std::invalid_argument("Invalid objective type: " + type);
            }
            auto rank_value = std::stoi(value);
            objectives.push_back({name, type, rank_value > 10 ? 10 : rank_value});
        } else {
            // Parse product constraints
            if (key == "product_name") {
                currentProduct.name = value;
            } else if (key == "cost_range") {
                std::tie(currentProduct.costMin, currentProduct.costMax) = parseRange(value);
            } else if (key == "profit_range") {
                std::tie(currentProduct.profitMin, currentProduct.profitMax) = parseRange(value);
            } else if (key == "demand_range") {
                std::tie(currentProduct.demandMin, currentProduct.demandMax) = parseRange(value);
            } else if (key == "budget_range") {
                std::tie(currentProduct.budgetMin, currentProduct.budgetMax) = parseRange(value);
            } else if (key == "man_hour_per_unit") {
                std::tie(currentProduct.manHourPerUnitMin, currentProduct.manHourPerUnitMax) = parseRange(value);
            } else if (key == "total_man_hours") {
                std::tie(currentProduct.totalManHoursMin, currentProduct.totalManHoursMax) = parseRange(value);
            }
        }
    }

    if (!currentSection.empty() && currentSection != "Global" && currentSection != "Objectives") {
        products[currentProduct.name] = currentProduct;
    }

    return products;
}

bool validateInput(const std::unordered_map<std::string, Product>& products,
                   const GlobalConstraints& globalConstraints,
                   const std::vector<Objective>& objectives) {
    std::vector<std::string> criticalErrors;
    std::vector<std::string> warnings;

    for (const auto& [name, product] : products) {
        // Derive budget range
        double minBudget = product.costMin * product.demandMin;
        double maxBudget = product.costMax * product.demandMax;

        // Validate input budget range
        if (product.budgetMin < minBudget || product.budgetMax > maxBudget) {
            criticalErrors.emplace_back(
                "Product: " + name + " - Budget range [" +
                std::to_string(product.budgetMin) + ", " +
                std::to_string(product.budgetMax) +
                "] is outside the realistic range [" +
                std::to_string(minBudget) + ", " +
                std::to_string(maxBudget) + "].");
        } else if (product.budgetMin > minBudget || product.budgetMax < maxBudget) {
            warnings.emplace_back(
                "Product: " + name + " - Budget range [" +
                std::to_string(product.budgetMin) + ", " +
                std::to_string(product.budgetMax) +
                "] is narrower than the realistic range [" +
                std::to_string(minBudget) + ", " +
                std::to_string(maxBudget) + "].");
        }

        // Derive man-hour range
        double minManHours = product.manHourPerUnitMin * product.demandMin;
        double maxManHours = product.manHourPerUnitMax * product.demandMax;

        if (product.totalManHoursMax > 0 && product.totalManHoursMax < minManHours) {
            criticalErrors.emplace_back(
                "Product: " + name + " - Total man-hours range is below the realistic minimum. Suggested max: " +
                std::to_string(maxManHours) + ".");
        } else if (product.totalManHoursMax > maxManHours) {
            warnings.emplace_back(
                "Product: " + name + " - Total man-hours range exceeds the realistic maximum. Suggested max: " +
                std::to_string(maxManHours) + ".");
        }
    }

    // Global constraint validation
    double totalMinBudget = 0.0, totalMaxBudget = 0.0;

    for (const auto& [name, product] : products) {
        totalMinBudget += product.costMin * product.demandMin;
        totalMaxBudget += product.costMax * product.demandMax;
    }

    if (globalConstraints.budgetMin < totalMinBudget || globalConstraints.budgetMax > totalMaxBudget) {
        criticalErrors.emplace_back(
            "Global budget range [" + std::to_string(globalConstraints.budgetMin) + ", " +
            std::to_string(globalConstraints.budgetMax) +
            "] is outside the realistic range [" + std::to_string(totalMinBudget) + ", " +
            std::to_string(totalMaxBudget) + "].");
    } else if (globalConstraints.budgetMin > totalMinBudget || globalConstraints.budgetMax < totalMaxBudget) {
        warnings.emplace_back(
            "Global budget range [" + std::to_string(globalConstraints.budgetMin) + ", " +
            std::to_string(globalConstraints.budgetMax) +
            "] is narrower than the realistic range [" + std::to_string(totalMinBudget) + ", " +
            std::to_string(totalMaxBudget) + "].");
    }

    // Print all critical errors and warnings
    if (!criticalErrors.empty()) {
        std::cerr << "\nCritical Errors:\n";
        for (const auto& error : criticalErrors) {
            std::cerr << "  - " << error << "\n";
        }
        std::cerr << "Program terminated due to critical errors.\n";
        return false;
    }

    if (!warnings.empty()) {
        std::cout << "\nWarnings:\n";
        for (const auto& warning : warnings) {
            std::cout << "  - " << warning << "\n";
        }
        std::cout << "I may still find an optimal solutions with these warnings.\n" 
                  << "Do you want to proceed? (y/n): ";
        char choice;
        std::cin >> choice;
        if (choice != 'y' && choice != 'Y') {
            std::cerr << "Execution halted by user.\n";
            return false;
        }
    }

    std::cout << "Validation successful.\n";
    return true;
}

