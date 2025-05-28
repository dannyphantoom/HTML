CC = gcc
CFLAGS = -O2 -Wall -I./src # Add -I./src to find headers in src/
LDFLAGS = 

SRC_DIR = src
BUILD_DIR = build

# List of source files
SOURCES = $(SRC_DIR)/tokens.c $(SRC_DIR)/lexer.c $(SRC_DIR)/main.c

# Generate object file names in the build directory
# e.g., src/HTML.c -> build/HTML.o
OBJECTS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SOURCES))

# Name of the final executable in the build directory
EXECUTABLE = $(BUILD_DIR)/HTML

.PHONY: all clean run

all: $(EXECUTABLE)

# Rule to link the executable
$(EXECUTABLE): $(OBJECTS)
	@mkdir -p $(@D) # Ensure the directory for the executable exists (e.g. build/)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) # $^ means all prerequisites (the .o files)

# Pattern rule to compile .c files from SRC_DIR to .o files in BUILD_DIR
# e.g. makes build/HTML.o from src/HTML.c
# Ensure all necessary headers are listed as dependencies for .o files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(SRC_DIR)/HTML.h $(SRC_DIR)/tokens.h $(SRC_DIR)/lexer.h Makefile
	@mkdir -p $(@D) # Ensure the directory for the .o file exists (e.g. build/)
	$(CC) $(CFLAGS) -c $< -o $@   # $< means the first prerequisite (the .c file)

clean:
	@echo "Cleaning up build files..."
	rm -rf $(BUILD_DIR)

# Optional: A target to run the program
run: $(EXECUTABLE)
	./$(EXECUTABLE)
