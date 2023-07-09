TARGET_EXEC := doom_pico

SRC_DIR := src
INC_DIR := inc
LIB_DIR := lib
BIN_DIR := bin

SRCS := $(shell find $(SRC_DIR) -name '*.c' -or -name '*.s')
OBJS := $(SRCS:%=$(BIN_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

CFLAGS := -Iinc -Ilib -MMD -MP -g
LDFLAGS := -lm -lraylib

$(BIN_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

$(BIN_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -r $(BIN_DIR)

-include $(DEPS)
