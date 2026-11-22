# 2026-11-20 18:39 - Makefile - chriscarl - added isa.c test cases must pass first
# 2026-11-16 22:02 - Makefile - chriscarl - added ignores for test and docs dir
# 2026-10-13 15:34 - Makefile - chriscarl - initial commit

# Usage
# make -f /path/to/project/Makefile
# make -f /path/to/project/Makefile clean
# make -f CMPE-220/assignments/project/Makefile

# Learnings
# filter/filter-out are make commands

# gpt6.1sol-m: augment this makefile to complile and run the test suite at  test\isa\test_isa.c first, followed by the original target. isa.c uses all of the same h and c files as target does.
# 	- https://chatgpt.com/c/6b01018a-cf40-83e8-9401-c8979ae43591
# gemini3.8fm: makefile file that recursively includes all .c and .h files without enumerating them literally, contains a clean, and can be run from any directory and outputs relative to the makefile file. output only the makefile file and 2 example command line invocations without explanation.
#	- https://gemini.google.com/app/a1e0f8d0380c1341

MAKEFILE_DIR := $(patsubst %/,%,$(dir $(abspath $(lastword $(MAKEFILE_LIST)))))

CC ?= gcc
CFLAGS ?= -Wall -Wextra -O2
# NOTE: -Werror causes fail on any warnings, very stringent...

# https://stackoverflow.com/a/4210072
SRCS := $(shell find $(MAKEFILE_DIR) \
					-type d \( -path '$(MAKEFILE_DIR)/test' -o -path '$(MAKEFILE_DIR)/docs' -o -path '$(MAKEFILE_DIR)/ignoreme' \) -prune -o \
					-type f -name '*.c' -print)
HDRS := $(shell find $(MAKEFILE_DIR) \
					-type d \( -path '$(MAKEFILE_DIR)/test' -o -path '$(MAKEFILE_DIR)/docs' -o -path '$(MAKEFILE_DIR)/ignoreme' \) -prune -o \
					-type f -name '*.h' -print)
INC_DIRS := $(sort $(dir $(HDRS)))
INC_FLAGS := $(addprefix -I,$(INC_DIRS))
CPPFLAGS += $(INC_FLAGS) -MMD -MP

BUILD_DIR := $(MAKEFILE_DIR)/build
TARGET := $(MAKEFILE_DIR)/bin/main
TEST_TARGET := $(MAKEFILE_DIR)/bin/test_isa
TEST_SRC := $(MAKEFILE_DIR)/test/isa/test_isa.c
MAIN_SRCS ?= $(filter %/main.c,$(SRCS))

OBJS := $(patsubst $(MAKEFILE_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))
TEST_OBJ := $(patsubst $(MAKEFILE_DIR)/%.c,$(BUILD_DIR)/%.o,$(TEST_SRC))
TEST_OBJS := $(patsubst $(MAKEFILE_DIR)/%.c,$(BUILD_DIR)/%.o,$(filter-out $(MAIN_SRCS),$(SRCS))) $(TEST_OBJ)
DEPS := $(sort $(OBJS:.o=.d) $(TEST_OBJS:.o=.d))


.PHONY: all clean test build run
.NOTPARALLEL:

# .NOTPARALLEL keeps these steps ordered, including with make -j.
all: clean test build run

clean:
	rm -rf $(BUILD_DIR) $(MAKEFILE_DIR)/bin
	rm -rf $(TARGET)

test: $(TEST_TARGET)
	$(TEST_TARGET)

$(TEST_TARGET): $(TEST_OBJS)
	@mkdir -p $(dir $@)
	$(CC) $(TEST_OBJS) $(LDFLAGS) -o $@

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
