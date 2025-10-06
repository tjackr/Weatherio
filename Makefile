# ======================================
# 🌤 Weatherio Makefile (Cross-platform)
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

# Compilation flags
#CFLAGS := -std=c99 -MMD -MP -Wall -Wextra -Werror -Wfatal-errors -Wno-format-truncation -Iconfigs -g
CFLAGS := -std=c99 -MMD -MP -Wall -Iconfigs -g

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
BIN := Weatherio

# Colors
GREEN := \033[0;32m
BLUE  := \033[1;34m
RESET := \033[0m

# ========================
# Build rules
# ========================

all: $(BIN)
	@echo "$(GREEN)✅ Build complete.$(RESET)"

$(BIN): $(OBJ)
	@echo "$(BLUE)🔗 Linking...$(RESET)"
	@$(CC) $(LDFLAGS) $(OBJ) -o $@ $(LIBS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "$(BLUE)⚙️  Compiling $<...$(RESET)"
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c $< -o $@

run: $(BIN)
	@echo "$(GREEN)🚀 Running Weatherio...$(RESET)"
	@./$(BIN)

clean:
	@echo "$(BLUE)🧹 Cleaning build files...$(RESET)"
	@rm -rf $(BUILD_DIR) $(CACHE_DIR) $(BIN)
	@echo "$(GREEN)✨ Clean complete.$(RESET)"

print:
	@echo "Källfiler: $(SRC)"
	@echo "Objektfiler: $(OBJ)"
	@echo "Dependency-filer: $(DEP)"

-include $(DEP)

.PHONY: all run clean print
