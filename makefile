# === Colors ===
RED    := \033[0;31m
GREEN  := \033[0;32m
YELLOW := \033[0;33m
PURPLE := \033[0;35m
RESET  := \033[0m

# === Project Configuration ===
CXX := g++
BUILD_DIR := build
BIN := $(BUILD_DIR)/main
ASSETS_DST := $(BUILD_DIR)/assets
SRC_DIR := src
OBJ_DIR := $(BUILD_DIR)/obj
INC_DIR := include

# === Compilation Flags ===
CFLAGS := -g -Wall -Wextra -std=c++17 \
  -I/usr/include/SDL2 -D_REENTRANT \
  -I$(INC_DIR) \
  -I$(INC_DIR)/core \
  -I$(INC_DIR)/ecs \
  -I$(INC_DIR)/ecs/core \
  -I$(INC_DIR)/ecs/systems \
  -I$(INC_DIR)/ecs/components \
  -I$(INC_DIR)/ecs/events \
	-I$(INC_DIR)/utils \
	-I$(INC_DIR)/scripts \
	-I$(INC_DIR)/editor \
	-I$(INC_DIR)/world \
	-I$(INC_DIR)/serializers \
  -I$(INC_DIR)/external \
  -I$(INC_DIR)/external/imgui \
  -I$(INC_DIR)/external/imgui/backends \
  -I$(INC_DIR)/external/sol/ \
  -I$(INC_DIR)/external/sol/compatibility \
  -I$(INC_DIR)/external/sol/detail \
  -I$(INC_DIR)/external/sol/stack \
  -I$(INC_DIR)/external/sol/utility \

LDFLAGS := -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -llua

# === alternate ( for macOS )===
# LDFLAGS := $(shell pkg-config --libs sdl2 SDL2_image SDL2_ttf)
# CFLAGS  += $(shell pkg-config --cflags sdl2 SDL2_image SDL2_ttf)

# === Source Files ===
SRC_FILES := $(shell find $(SRC_DIR) -name "*.cpp") main.cpp
OBJ_FILES := $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))

# === Default Target ===
all: $(BIN) copy_assets
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

# === Copy Assets to Build Folder ===
copy_assets:
	@mkdir -p $(ASSETS_DST)
	@echo -e "$(YELLOW)[INFO] Copied assets to build folder.$(RESET)"

run: $(BIN)
	@echo -e "$(YELLOW)[INFO] Launching binary from build dir...$(RESET)"
	cd $(BUILD_DIR) && ./main

# === Clean Build Files ===
clean:
	@echo -e "$(RED)[CLEAN] Removed binaries and object files.$(RESET)"
	@rm -rf $(OBJ_DIR)
	@rm $(BUILD_DIR)/main 

.PHONY: all clean run copy_assets
