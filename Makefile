# Makefile for XML and CSV Parsing Semi-Markov Project

# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -O2 -fopenmp

# Directories
BINDIR = bin
BUILDDIR = build
INCLUDEDIR = include
SRCDIR = src

# Target executable
TARGET = $(BINDIR)/semi_markov

# Source files
SRCS = $(SRCDIR)/main.cpp $(SRCDIR)/smp.cpp

# Header files
HEADERS = $(INCLUDEDIR)/smp.h

# Object files
OBJS = $(BUILDDIR)/main.o $(BUILDDIR)/smp.o

# Default rule to build the target
all: $(TARGET)

# Rule to build the target executable
$(TARGET): $(OBJS) | $(BINDIR)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) -ltinyxml2

# Rule to compile main.cpp
$(BUILDDIR)/main.o: $(SRCDIR)/main.cpp $(HEADERS) | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -I$(INCLUDEDIR) -c $< -o $@

# Rule to compile smp.cpp
$(BUILDDIR)/smp.o: $(SRCDIR)/smp.cpp $(HEADERS) | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -I$(INCLUDEDIR) -c $< -o $@

# Rule to create the bin directory
$(BINDIR):
	mkdir -p $(BINDIR)

# Rule to create the build directory
$(BUILDDIR):
	mkdir -p $(BUILDDIR)

# Clean up the build
clean:
	rm -rf $(BUILDDIR) $(BINDIR)

# Phony targets
.PHONY: all clean

