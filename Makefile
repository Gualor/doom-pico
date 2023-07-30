TARGET_EXEC := doom_pico

CC := gcc
RM := rm -rf
MKDIR := mkdir -p

SRC_DIR := src
INC_DIR := inc
BIN_DIR := bin
RAYLIB_DIR := raylib

SRCS := $(shell find $(SRC_DIR) -name "*.c")
OBJS := $(patsubst $(SRC_DIR)/%,$(BIN_DIR)/%.o,$(SRCS))
DEPS := $(OBJS:.o=.d)

CFLAGS := -I$(INC_DIR) -MMD -MP -g
LDFLAGS := -lm

ifeq ($(USE_RAYLIB), 1)
    CFLAGS += -I$(RAYLIB_DIR)/include -DUSE_RAYLIB
    LDFLAGS += -L$(RAYLIB_DIR)/lib -lraylib
    ifeq ($(OS), Windows_NT)
        LDFLAGS += -lopengl32 -lgdi32 -lwinmm
    else ifeq ($(shell uname -s), Linux)
        LDFLAGS += -lGL -lpthread -ldl -lrt -lX11
    else
        $(error Platform $(OS) currently not supported for make "run" target)
    endif
endif

$(TARGET_EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

$(BIN_DIR)/%.c.o: $(SRC_DIR)/%.c
	$(MKDIR) $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(RAYLIB_DIR):
	./install_raylib.sh

.PHONY: run
run: $(RAYLIB_DIR)
	$(MAKE) USE_RAYLIB=1
	./$(TARGET_EXEC)

.PHONY: clean
clean:
ifneq (,$(wildcard $(BIN_DIR)))
	$(RM) $(BIN_DIR)
endif

-include $(DEPS)
