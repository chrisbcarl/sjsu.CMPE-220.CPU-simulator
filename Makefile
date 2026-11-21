# 2026-11-16 22:02 - Makefile - chriscarl - added ignores for test and docs dir
# 2026-10-13 15:34 - Makefile - chriscarl - initial commit

# Usage
# make -f /path/to/project/Makefile
# make -f /path/to/project/Makefile clean
# make -f CMPE-220/assignments/project/Makefile

# gemini3.8fm: makefile file that recursively includes all .c and .h files without enumerating them literally, contains a clean, and can be run from any directory and outputs relative to the makefile file. output only the makefile file and 2 example command line invocations without explanation.
#	- https://gemini.google.com/app/a1e0f8d0380c1341

MAKEFILE_DIR := $(patsubst %/,%,$(dir $(abspath $(lastword $(MAKEFILE_LIST)))))

CC ?= gcc
CFLAGS ?= -Wall -Wextra -O2
# NOTE: -Werror causes fail on any warnings, very stringent...

# https://stackoverflow.com/a/4210072
SRCS := $(shell find $(MAKEFILE_DIR) \
					-type d \( -path '$(MAKEFILE_DIR)/test' -o -path '$(MAKEFILE_DIR)/docs' \) -prune -o \
					-type f -name '*.c' -print)
# SRCS := $(filter-out $(MAKEFILE_DIR)/src/test/exponentiation/%,$(SRCS))  # NOTE: filter-out is part of GNU Make, may be worth re-using later if things get weird
HDRS := $(shell find $(MAKEFILE_DIR) \
					-type d \( -path '$(MAKEFILE_DIR)/test' -o -path '$(MAKEFILE_DIR)/docs' \) -prune -o \
					-type f -name '*.h' -print)
INC_DIRS := $(sort $(dir $(HDRS)))
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

BUILD_DIR := $(MAKEFILE_DIR)/build
TARGET := $(MAKEFILE_DIR)/bin/main

OBJS := $(patsubst $(MAKEFILE_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))
DEPS := $(OBJS:.o=.d)

CPPFLAGS += $(INC_FLAGS) -MMD -MP

.PHONY: all clean build run
.NOTPARALLEL:

all: clean build run

clean:
	rm -rf $(BUILD_DIR) $(MAKEFILE_DIR)/bin
	rm -rf $(TARGET)

build: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(dir $@)
	$(CC) $(OBJS) $(LDFLAGS) -o $@

$(BUILD_DIR)/%.o: $(MAKEFILE_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	$(TARGET)

-include $(DEPS)
