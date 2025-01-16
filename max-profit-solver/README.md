PROJECT: Profit Maximizer Solver

ABSTRACT

This project is to solve a multi-constraint & multi-objective LP problem.
Here there are set of constraints with lower and upper bounds and a set of objectives 
which are mutually inclusive & exclusive at the same time.


DESCRIPTION

The Profit Maximizer Solver is a C++ application designed to optimize the allocation of resources 
and maximize profits across multiple products under various constraints. The program incorporates 
user-defined product-specific and global constraints, as well as ranked objectives, to compute an 
optimal solution using the COIN-OR Clp library for linear programming.

Key features:
- Flexible handling of input ranges for cost, demand, and profit percentage.
- Validation of user inputs to ensure feasibility.
- Ranked objectives (e.g., maximize profit, minimize resource usage) guide the optimization process.
- Sensitivity analysis to evaluate the robustness of the solution.


KEY PARAMETERS

The optimization problem is formulated as a linear programming model:

# Product-Level Inputs
Cost Range:
Profit Percentage Range:
Demand Range:

# Global Constraints
Global Budget Range:
Global Man-Hours Range:
Global Profit Range:

# Objective Functions
Maximize profit
Minimize Profit
Maximize resource usages
Minimize resource usages
Maximize budget usage
Minimize budget usage

## Getting Started

### Prerequisites

Ensure the following dependencies are installed:
- C++ Compiler: GCC or Clang with C++17 support.
- COIN-OR Clp Library: Includes 'Clp', 'OsiClp', and 'CoinUtils'.
- Make: For building the project.
- Package Manager: 'brew' (macOS) or 'apt-get' (Linux) for installing dependencies.

### Installation

1. Clone the Repository:
  
   - git clone https://github.com/yourusername/profit-maximizer-solver.git
   - cd profit-maximizer-solver

2. Install Dependencies:
   Run the provided build.sh script to automatically install required libraries:
   ./build.sh
   This script will:
   - Check for and install `brew` or `apt-get` if not already installed.
   - Install the `clp` and `coinutils` libraries if missing.

## Building the Project

1. Run the build.sh script:
   - ./build.sh (Note: You may want to change the hard coded paths in the build.sh to point
   your local systems Clp and coinutils instllation directories include header files path)
   
   This will:
   - Validate dependencies.
   - Compile the source code into the 'profit_maximizer' executable.

2. Ensure the build was successful:
   - ls
   You should see the `profit_maximizer` binary in the directory.

## Running the Project

1. Prepare Input Configuration:
   - Edit the 'input.config' file to define:
     - Product-specific constraints (cost, demand, budget, etc.).
     - Global constraints (budget, profit expectations, etc.).
     - Objectives with ranks.

2. Run the Program:
   Execute the solver with:
   - ./profit_maximizer

3. Review Output:
   - The program validates inputs, computes an optimal solution, and displays the results in a tabular format.
   - Includes warnings or errors for invalid inputs and suggestions for adjustments.

## Features

- Input Validation:
  - Ensures ranges are realistic and within calculated bounds.
  - Stops execution for critical errors or allows user intervention for warnings.

- Optimization:
  - Solves the linear programming problem with COIN-OR Clp.
  - Uses ranked objectives to guide decision-making.

- Sensitivity Analysis:
  - Evaluates profit variations with adjusted profit percentages.

## File Structure
.
|-- input.config      # Input file for defining constraints and objectives
|-- build.sh          # Script to install dependencies and build the project
|-- Makefile          # Makefile for compilation
|-- profit_maximizer.cpp  # Main driver program
|-- input.cpp         # Input parsing and validation logic
|-- input.h           # Header for input-related functions
|-- solver.cpp        # Solver logic for optimization
|-- solver.h          # Header for solver-related functions

## Contribution Guidelines

We welcome contributions to enhance this project! To contribute:
1. Fork the repository.
2. Create a new branch for your feature or bugfix.
3. Submit a pull request with a clear description of changes.

## License

This project is licensed under the MIT License. See the 'LICENSE' file for details.

## Acknowledgments

Special thanks to the COIN-OR project for their robust linear programming libraries.

## Contact

For questions or support, reach out to:
- Email:    amit_cal23@yahoo.com
- GitHub:   https://github.com/amit-cal23

