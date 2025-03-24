# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -Wall -Wextra -std=c++20 -Iinclude -pedantic

# Ensure object directories exist
$(shell mkdir -p obj obj/src obj/tests)

# Source files
SRCS = $(wildcard src/*.cpp)

# Header files
HDRS = $(wildcard include/*.hpp)

# Source files for argTest
ARGSRCS = tests/testArgs.cpp src/utils.cpp src/arguments.cpp

# Object files
OBJS = $(patsubst src/%.cpp,obj/src/%.o,$(SRCS))
ARGOBJS = $(patsubst %.cpp,obj/%.o,$(ARGSRCS))

# Executable names
TARGET = ipk-l4-scan
ARGTARGET = argTest

# Default target
all: $(TARGET)

# Main executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Argument testing executable
argTest: $(ARGOBJS)
	$(CXX) $(CXXFLAGS) -o $(ARGTARGET) $^

# Compile src files into obj/src/
obj/src/%.o: src/%.cpp $(HDRS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile test and extra files into obj/
obj/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# run the arg test script
testArgs:
	./testArgs.sh

# Zip the project
zip: 
	zip -r x247581.zip images src include Makefile LICENSE README.md CHANGELOG.md

# Clean build files
clean:
	rm -f $(OBJS) $(ARGOBJS) $(TARGET) $(ARGTARGET)
	rm -rf obj/*
	rm -f ./x247581.zip

uml_diagram:
	hpp2plantuml -i "./include/*.hpp" -o output.dot
	dot -Tsvg output.dot -o output.svg
	rm -f output.dot

.PHONY: all clean argTest zip valgrind rebuild
