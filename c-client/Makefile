OS := $(shell uname 2>/dev/null || echo Windows_NT)
BUILD_DIR := ./build

ifeq ($(OS), Windows_NT)
    MKDIR = mkdir
    CLIENT_OUT = $(BUILD_DIR)/client.exe
    LIBS = -lws2_32
else
    MKDIR = mkdir -p
    CLIENT_OUT = $(BUILD_DIR)/client
    LIBS =
endif

all: create_build_dir
	gcc ./client.c -o $(CLIENT_OUT) $(LIBS)

create_build_dir:
	$(MKDIR) $(BUILD_DIR)
