# Makefile for gatorLibrary project

# Compiler settings - Can be customized.
CXX = g++
CXXFLAGS = -Wall -std=c++2a

# Target executable name
TARGET = gatorLibrary

# Build target
all: $(TARGET)

$(TARGET): GatorLib.cpp
	$(CXX) $(CXXFLAGS) -o $(TARGET) GatorLib.cpp

# Clean build files
clean:
	rm -f $(TARGET)

# Phony targets
.PHONY: all clean
