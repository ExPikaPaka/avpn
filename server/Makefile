# Default compiler
COMPILER = g++
CFLAGS = -std=c++14 -O3 -Wall -Wextra -pedantic

# Executable
EXECUTABLE = server
SRCDIR = src
SRCFILES = $(wildcard $(SRCDIR)/*.cpp)
OBJFILES = $(SRCFILES:$(SRCDIR)/%.cpp=$(BUILD_DIR)/%.o)
MAIN = main.cpp

# Directory settings
BUILD_DIR ?= build

# Default make target
all: $(BUILD_DIR)/$(EXECUTABLE)

# Executable recipe (compiling server.cpp and objects)
$(BUILD_DIR)/$(EXECUTABLE): $(MAIN) $(OBJFILES) | $(BUILD_DIR)
	$(COMPILER) $(CFLAGS) -o $@ $^

# Object files compilation
$(BUILD_DIR)/%.o: $(SRCDIR)/%.cpp | $(BUILD_DIR)
	$(COMPILER) $(CFLAGS) -c -o $@ $<

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Clean build directory
clean:
	rm -rf $(BUILD_DIR)
