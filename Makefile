LIB_NAME = arenaalloc

ROOT = $(shell pwd)
BUILD_DIR = $(ROOT)/build
SRC_DIR = $(ROOT)/src
INC_DIR = $(ROOT)/include

CC := clang
LD := clang
AS := llvm-as
AR := ar

BUILD_TYPE := debug
BUILD_MODE := dynamic
BUILD_TARGET := $(BUILD_DIR)/lib$(LIB_NAME).so

CFLAGS = -I$(INC_DIR)
LDFLAGS =

ifeq ($(BUILD_TYPE), debug)
	CFLAGS += -g -O0 -DDEBUG -D_DEBUG
else
	CFLAGS += -O2 -DNDEBUG -D_NDEBUG -flto
	LDFLAGS += -flto
endif

ifeq ($(BUILD_MODE), static)
	LDFLAGS += -static
	BUILD_TARGET = $(BUILD_DIR)/lib$(LIB_NAME).a
endif

PREFIX := /usr/local
INSTALL_LIB_DIR := $(PREFIX)/lib
INSTALL_INC_DIR := $(PREFIX)/include

SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SOURCES))

TESTS = $(wildcard $(ROOT)/tests/*.c)
TEST_BINS = $(patsubst $(ROOT)/tests/%.c, $(BUILD_DIR)/tests/%, $(TESTS))

.PHONY: all clean setup install uninstall test

all: setup $(BUILD_TARGET)

setup:
	@mkdir -p $(BUILD_DIR) $(BUILD_DIR)/tests

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.asm
	$(AS) -o $@ $<  # Removed $(AFLAGS) since it's not defined

$(BUILD_DIR)/lib$(LIB_NAME).so: $(OBJECTS)
	$(LD) -shared $(LDFLAGS) -o $@ $^

$(BUILD_DIR)/lib$(LIB_NAME).a: $(OBJECTS)
	$(AR) rcs $@ $^

clean:
	rm -rf $(BUILD_DIR)

install: $(BUILD_TARGET)
	install -d $(INSTALL_LIB_DIR) $(INSTALL_INC_DIR)
	install -m 644 $(BUILD_TARGET) $(INSTALL_LIB_DIR)
	install -m 644 $(INC_DIR)/*.h $(INSTALL_INC_DIR)  # Fixed header installation

uninstall:
	rm -f $(INSTALL_LIB_DIR)/lib$(LIB_NAME).so
	rm -f $(INSTALL_LIB_DIR)/lib$(LIB_NAME).a
	rm -f $(INSTALL_INC_DIR)/*.h  # Uninstall headers as well

test: $(BUILD_TARGET) $(TEST_BINS)
	@for test in $(TEST_BINS); do \
		if [ "$(BUILD_MODE)" = "dynamic" ]; then \
			LD_LIBRARY_PATH=$(BUILD_DIR):$$LD_LIBRARY_PATH $$test; \
		else \
			strace $$test; \
		fi; \
	done

$(BUILD_DIR)/tests/%: $(ROOT)/tests/%.c
	$(CC) $(CFLAGS) -L$(BUILD_DIR) -l$(LIB_NAME) $< -o $@
