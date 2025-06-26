# === Colors ===
RED    := \033[0;31m
GREEN  := \033[0;32m
YELLOW := \033[0;33m
PURPLE := \033[0;35m
RESET  := \033[0m

# === Project Configuration ===
CXX := g++
BIN := main
SRC_DIR := src
OBJ_DIR := obj
INC_DIR := include

# === Compilation Flags ===
CFLAGS := -g -Wall -Wextra -std=c++17 \
  -I$(INC_DIR) \
  -I$(INC_DIR)/core \
  -I$(INC_DIR)/ecs \
	-I$(INC_DIR)/utils \
  -I$(INC_DIR)/external \
  -I$(INC_DIR)/game \

LDFLAGS := -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer

# === alternate ( for macOS )===
# LDFLAGS := $(shell pkg-config --libs sdl2 SDL2_image SDL2_ttf)
# CFLAGS  += $(shell pkg-config --cflags sdl2 SDL2_image SDL2_ttf)

# === Source Files ===
SRC_FILES := $(shell find $(SRC_DIR) -name "*.cpp") main.cpp
OBJ_FILES := $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))

# === Default Target ===
all: $(BIN)
	@echo -e "$(GREEN)[✓] Build complete.$(RESET)"

# === Link Objects to Binary ===
$(BIN): $(OBJ_FILES)
	@echo -e "$(YELLOW)[INFO] Linking objects...$(RESET)"
	$(CXX) $^ -o $@ $(LDFLAGS)

# === Compile .cpp to .o, preserving folder structure ===
$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	@echo -e "$(PURPLE)[DEBUG] Compiling $<$(RESET)"
	$(CXX) $(CFLAGS) -c $< -o $@

# === Run the Program ===
run: $(BIN)
	@echo -e "$(YELLOW)[INFO] Launching binary...$(RESET)"
	./$(BIN)

# === Clean Build Files ===
clean:
	@echo -e "$(RED)[CLEAN] Removed binaries and object files.$(RESET)"
	@rm -rf $(OBJ_DIR) $(BIN)

.PHONY: all clean run
