#ifndef INPUT_H
#define INPUT_H

#include <string>
#include <vector>
#include <unordered_map>

// Product-specific constraints
struct Product {
    std::string name;
    double costMin, costMax;
    double profitMin, profitMax;
    double demandMin, demandMax;
    double budgetMin, budgetMax;
    double manHourPerUnitMin, manHourPerUnitMax;
    double totalManHoursMin, totalManHoursMax;
};

// Global constraints
struct GlobalConstraints {
    double budgetMin, budgetMax;
    double profitMin, profitMax;
    double manHoursMin, manHoursMax;
};

// Objectives
struct Objective {
    std::string name;
    std::string type; // "maximize" or "minimize"
    int rank;         // Ranking priority
};

// Function declarations
std::unordered_map<std::string, Product> parseInputConfig
    (const std::string& filename, 
    GlobalConstraints& globalConstraints, 
    std::vector<Objective>& objectives);

bool validateInput(const std::unordered_map<std::string, 
    Product>& products, 
    const GlobalConstraints& globalConstraints, 
    const std::vector<Objective>& objectives);

#endif // INPUT_H

