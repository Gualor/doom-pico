TARGET_EXEC := doom_pico

CC := gcc
RM := rm -rf

SRC_DIR := src
INC_DIR := inc
LIB_DIR := lib
BIN_DIR := bin

SRCS := $(shell find $(SRC_DIR) -name '*.c' -or -name '*.s')
OBJS := $(SRCS:%=$(BIN_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

CFLAGS := -Iinc -Ilib -MMD -MP -g
LDFLAGS := -lm

ifeq ($(USE_RAYLIB), 1)
	CFLAGS += -DUSE_RAYLIB
	LDFLAGS += -lraylib
endif

$(BIN_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

$(BIN_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: run
run:
	$(MAKE) clean
	$(MAKE) USE_RAYLIB=1
	./$(BIN_DIR)/$(TARGET_EXEC)

.PHONY: clean
clean:
	$(RM) $(BIN_DIR)

-include $(DEPS)
