# 2026-11-23 - Chris Carl - Makefile - added test_instructions.c, might need a for loop type thing somehow, augmented to run ALL tests in the ./test directory
# 2026-11-22 - Chris Carl - Makefile - added -g flag for PDB debugging
# 2026-11-20 18:39 - Chris Carl - Makefile - added isa.c test cases must pass first
# 2026-11-16 22:02 - Chris Carl - Makefile - added ignores for test and docs dir
# 2026-10-13 15:34 - Chris Carl - Makefile - initial commit

# Usage
# make -f /path/to/project/Makefile
# make -f /path/to/project/Makefile clean
# make -f CMPE-220/assignments/project/Makefile

# Learnings
# filter/filter-out are make commands

# neon4: Modify this Makefile to for-loop through all .c files in the ./src/test directory that have a main entrypoint during the "test" phase of "all"
#	- https://gemini.google.com/app/9ca1455fbdaaa6d1
# gpt6.1sol-m: augment this makefile to complile and run the test suite at  test\isa\test_isa.c first, followed by the original target. isa.c uses all of the same h and c files as target does.
# 	- https://chatgpt.com/c/6b01018a-cf40-83e8-9401-c8979ae43591
# gemini3.8fm: makefile file that recursively includes all .c and .h files without enumerating them literally, contains a clean, and can be run from any directory and outputs relative to the makefile file. output only the makefile file and 2 example command line invocations without explanation.
#	- https://gemini.google.com/app/a1e0f8d0380c1341

MAKEFILE_DIR := $(patsubst %/,%,$(dir $(abspath $(lastword $(MAKEFILE_LIST)))))

CC ?= gcc
CFLAGS ?= -g -Wall -Wextra -O2
# NOTE: -Werror causes fail on any warnings, very stringent...

# https://stackoverflow.com/a/4210072
TEST_DIR := $(MAKEFILE_DIR)/test

# find .c/.h except in ./test, ./src/test, ./docs, ./ignoreme etc
SRCS := $(shell find $(MAKEFILE_DIR) \
					-type d \( -path '$(TEST_DIR)' -o -path '$(MAKEFILE_DIR)/test' -o -path '$(MAKEFILE_DIR)/docs' -o -path '$(MAKEFILE_DIR)/ignoreme' \) -prune -o \
					-type f -name '*.c' -print)
HDRS := $(shell find $(MAKEFILE_DIR) \
					-type d \( -path '$(TEST_DIR)' -o -path '$(MAKEFILE_DIR)/test' -o -path '$(MAKEFILE_DIR)/docs' -o -path '$(MAKEFILE_DIR)/ignoreme' \) -prune -o \
					-type f -name '*.h' -print)
INC_DIRS := $(sort $(dir $(HDRS)))
INC_FLAGS := $(addprefix -I,$(INC_DIRS))
CPPFLAGS += $(INC_FLAGS) -MMD -MP

# find .c files under ./test that define main() entrypoint
TEST_SRCS := $(shell find $(TEST_DIR) -type f -name '*.c' -exec grep -lE '^(int|void) *main' {} \;)

BUILD_DIR := $(MAKEFILE_DIR)/build
TARGET := $(MAKEFILE_DIR)/bin/main
MAIN_SRCS ?= $(filter %/main.c,$(SRCS))

OBJS := $(patsubst $(MAKEFILE_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))
LIB_OBJS := $(patsubst $(MAKEFILE_DIR)/%.c,$(BUILD_DIR)/%.o,$(filter-out $(MAIN_SRCS),$(SRCS)))
TEST_OBJS := $(patsubst $(MAKEFILE_DIR)/%.c,$(BUILD_DIR)/%.o,$(TEST_SRCS))
TEST_TARGETS := $(patsubst $(TEST_DIR)/%.c,$(MAKEFILE_DIR)/bin/test/%,$(TEST_SRCS))
DEPS := $(sort $(OBJS:.o=.d) $(TEST_OBJS:.o=.d))

.PHONY: all clean test build run
.NOTPARALLEL:

# .NOTPARALLEL -> ordered, even with make -j.
all: clean test build run

clean:
	rm -rf $(BUILD_DIR) $(MAKEFILE_DIR)/bin
	rm -rf $(TARGET)

test: $(TEST_TARGETS)
	@set -e; \
	for t in $(TEST_TARGETS); do \
		echo ""; \
		echo "=== $$t ==="; \
		"$$t"; \
		echo ""; \
	done

# link each test binary with all non-main project objects
$(MAKEFILE_DIR)/bin/test/%: $(BUILD_DIR)/test/%.o $(LIB_OBJS)
	@mkdir -p $(dir $@)
	$(CC) $^ $(LDFLAGS) -o $@

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