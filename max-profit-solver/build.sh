#!/bin/bash

# Install Homebrew if not already installed (macOS)
if [[ "$(uname -s)" == "Darwin" && ! -x "$(command -v brew)" ]]; then
    echo "Homebrew not found. Installing..."
    /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
    echo "Homebrew installed successfully."
fi

# Install apt-get if missing (Linux - Debian/Ubuntu based systems)
if [[ -x "$(command -v lsb_release)" && $(lsb_release -is) == "Ubuntu" && ! -x "$(command -v apt-get)" ]]; then
    echo "apt-get not found. Ensure a package manager is installed. Cannot proceed automatically."
    exit 1
fi

# Check if COIN-OR Clp is installed
if ! [ -x "$(command -v clp)" ]; then
    echo "COIN-OR Clp not found. Installing..."
    if [ -x "$(command -v brew)" ]; then
        # Install using Homebrew on macOS
        brew install clp
    elif [ -x "$(command -v apt-get)" ]; then
        # Install using apt-get on Linux
        sudo apt-get update -y
        sudo apt-get install -y coinor-libclp-dev coinor-libclp1 coinor-libosi-dev coinor-libosi1 build-essential
    else
        echo "Error: Package manager not found (brew or apt-get). Install COIN-OR Clp manually."
        exit 1
    fi
    echo "COIN-OR Clp installed successfully."
else
    echo "COIN-OR Clp is already installed."
fi

# Check if Osi is installed
if [ ! -d "/opt/homebrew/opt/osi" ] && [ -x "$(command -v brew)" ]; then
    echo "Osi not found. Installing..."
    brew install osi
    echo "Osi installed successfully."
elif [ -x "$(command -v apt-get)" ]; then
    sudo apt-get install -y coinor-libosi-dev
    echo "Osi installed successfully on Linux."
else
    echo "Osi is already installed."
fi

# Check if CoinUtils is installed
if ! [ -x "$(command -v coinutils-config)" ]; then
    echo "CoinUtils not found. Installing..."
    if [ -x "$(command -v brew)" ]; then
        brew install coinutils
        echo "CoinUtils installed successfully."
    elif [ -x "$(command -v apt-get)" ]; then
        sudo apt-get install -y coinor-libcoinutils-dev
        echo "CoinUtils installed successfully on Linux."
    else
        echo "Error: Package manager not found (brew or apt-get). Install CoinUtils manually."
        exit 1
    fi
else
    echo "CoinUtils is already installed."
fi

# Hardcoded include and library paths based on user's system
CLP_INCLUDE_PATH="/opt/homebrew/opt/clp/include/clp/coin"
COINUTILS_INCLUDE_PATH="/opt/homebrew/opt/coinutils/include/coinutils/coin"
CLP_LIB_PATH="/opt/homebrew/opt/clp/lib"
COINUTILS_LIB_PATH="/opt/homebrew/opt/coinutils/lib"
OSI_LIB_PATH="/opt/homebrew/opt/osi/lib"

# Verify header file existence
if [ ! -f "${CLP_INCLUDE_PATH}/ClpSimplex.hpp" ]; then
    echo "Error: Clp header file not found at ${CLP_INCLUDE_PATH}. Please check the installation."
    exit 1
fi

if [ ! -f "${COINUTILS_INCLUDE_PATH}/CoinPragma.hpp" ]; then
    echo "Error: CoinUtils header file not found at ${COINUTILS_INCLUDE_PATH}. Please check the installation."
    exit 1
fi

# Create Makefile
cat <<EOF > Makefile
CXX = g++
CXXFLAGS = -std=c++17 -Wall -O2 -I${CLP_INCLUDE_PATH} -I${COINUTILS_INCLUDE_PATH}
LIBS = -L${CLP_LIB_PATH} -L${COINUTILS_LIB_PATH} -L${OSI_LIB_PATH} -lClp -lOsiClp -lOsi -lm

TARGET = profit_maximizer
SOURCES = profit_maximizer.cpp input.cpp solver.cpp

all: 
	\$(CXX) \$(CXXFLAGS) \$(SOURCES) -o \$(TARGET) \$(LIBS)

clean:
	rm -f \$(TARGET)
EOF

echo "Makefile created."

# Build the project
make clean && make

if [ $? -eq 0 ]; then
    echo "Build completed successfully. Run ./profit_maximizer to execute."
else
    echo "Build failed. Check errors and try again."
fi

