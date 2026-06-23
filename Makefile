CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11 -D_DEFAULT_SOURCE -Iinclude
LDFLAGS =

TARGET  = samples/a.out
BUILD   = build

SRCS    = \
          src/main.c \
          src/lms.c

OBJS    = $(patsubst src/%.c,$(BUILD)/%.o,$(SRCS))

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJS) | samples
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS)

$(BUILD)/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

samples:
	mkdir -p samples

run: $(TARGET)
	cd samples && ./a.out

clean:
	rm -rf $(BUILD) $(TARGET)
