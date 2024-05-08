# Default compiler
COMPILER = g++
CFLAGS = -std=c++14 -O3 -Wall -Wextra -pedantic

# Executable
EXECUTABLE = server

# Source directories
SRCDIR = src
TESTDIR = tests

# Source files
SRCFILES = $(wildcard $(SRCDIR)/*.cpp)
TESTFILES = $(wildcard $(TESTDIR)/*.cpp)

# Object files
OBJFILES = $(SRCFILES:$(SRCDIR)/%.cpp=$(BUILD_DIR)/%.o)
TESTOBJFILES = $(TESTFILES:$(TESTDIR)/%.cpp=$(BUILD_DIR)/%.o)

# Main file
MAIN = main.cpp

# Build directory
BUILD_DIR ?= build

# External libraries
LIBS = -lcrypto
TESTLIBS = -lgtest -lgtest_main -lpthread

# Default make target
all: $(BUILD_DIR)/$(EXECUTABLE) $(BUILD_DIR)/run_tests

# Server executable
$(BUILD_DIR)/$(EXECUTABLE): $(MAIN) $(OBJFILES) | $(BUILD_DIR)
	$(COMPILER) $(CFLAGS) -o $@ $^ $(LIBS)

# Test executable recipe
$(BUILD_DIR)/run_tests: $(TESTOBJFILES) $(OBJFILES) | $(BUILD_DIR)
	$(COMPILER) $(CFLAGS) -o $@ $^ $(TESTLIBS) $(LIBS)

# Object files compilation
$(BUILD_DIR)/%.o: $(SRCDIR)/%.cpp | $(BUILD_DIR)
	$(COMPILER) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR)/%.o: $(TESTDIR)/%.cpp | $(BUILD_DIR)
	$(COMPILER) $(CFLAGS) -c -o $@ $<

# Run tests
run_tests: $(BUILD_DIR)/run_tests
	./$(BUILD_DIR)/run_tests

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Clean build directory
clean:
	rm -rf $(BUILD_DIR)



