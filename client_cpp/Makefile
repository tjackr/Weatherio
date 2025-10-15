# ======================================
# 🌤 Weatherio Client (C++) Makefile
# ======================================

# Compiler (use clang on macOS, gcc otherwise)
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
	CC := clang
else
	CC := gcc
endif

# Directories
SRC_DIR   := src
BUILD_DIR := build
CACHE_DIR := data/cache
CITIES_DIR := data/cities

# Compilation flags
CFLAGS := -std=c99 -MMD -MP -Wall -Wextra -Werror -Wfatal-errors -Wno-format-truncation -Iconfigs -g
#CFLAGS := -std=c99 -MMD -MP -Wall -Iconfigs -g

# Linker flags (auto-detect platform)
ifeq ($(UNAME_S),Darwin)
	LDFLAGS := -flto
else
	LDFLAGS := -flto -Wl,--gc-sections
endif

# Linked libraries
LIBS := -lcurl

# Find all .c source files recursively
SRC := $(shell find -L $(SRC_DIR) -type f -name '*.c')

# Map .c → .o in the build directory
OBJ := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC))

# Dependency files (.d)
DEP := $(OBJ:.o=.d)

# Binary name
BIN := weatherio_server_cpp

# ========================
# Build rules
# ========================

all: $(BIN)
	@echo "Build complete."

$(BIN): $(OBJ)
	@echo "Linking..."
	@$(CC) $(LDFLAGS) $(OBJ) -o $@ $(LIBS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "Compiling $<..."
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c $< -o $@

run: $(BIN)
	@echo "Running Weatherio..."
	@./$(BIN)

clean:
	@echo "Cleaning build files..."
	@rm -rf $(BUILD_DIR) $(CACHE_DIR) $(CITIES_DIR) $(BIN)
	@echo "Clean complete."

valgrind: $(BIN)
	@echo "Running using valgrind..."
	@valgrind -s --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(BIN)

print:
	@echo "Källfiler: $(SRC)"
	@echo "Objektfiler: $(OBJ)"
	@echo "Dependency-filer: $(DEP)"

-include $(DEP)

.PHONY: all run clean print
