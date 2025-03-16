# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -Wall -Wextra -std=c++20 -Iinclude -pedantic

# Source files
SRCS = $(wildcard src/*.cpp)

# Header files
HDRS = $(wildcard include/*.hpp)

# Object files
OBJS = $(patsubst src/%.cpp,obj/%.o,$(SRCS))

# Create the objects directory if it doesn't exist
$(shell mkdir -p obj)

# Executable name
TARGET = ipk-l4-scan

# Default target
all: $(TARGET)

# Link object files to create the executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile source files into object files
obj/%.o: src/%.cpp $(HDRS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# zip the project
zip: 
	zip -r ipk-l4-scan.zip src include Makefile

# Clean up build files
clean:
	rm -f $(OBJS) $(TARGET)
	rm -rf obj/*

.PHONY: all clean