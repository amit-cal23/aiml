CXX = g++
CXXFLAGS = -std=c++17 -Wall -O2 -I/opt/homebrew/opt/clp/include/clp/coin -I/opt/homebrew/opt/coinutils/include/coinutils/coin
LIBS = -L/opt/homebrew/opt/clp/lib -L/opt/homebrew/opt/coinutils/lib -L/opt/homebrew/opt/osi/lib -lClp -lOsiClp -lOsi -lm

TARGET = profit_maximizer
SOURCES = profit_maximizer.cpp input.cpp solver.cpp

all: 
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET) $(LIBS)

clean:
	rm -f $(TARGET)
