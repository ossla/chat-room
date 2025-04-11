OS := $(shell uname 2>/dev/null || echo Windows_NT)
BUILD_DIR := ./build

ifeq ($(OS), Windows_NT)
    MKDIR = mkdir
    CLIENT_OUT = $(BUILD_DIR)/client.exe
    SERVER_OUT = $(BUILD_DIR)/server.exe
    LIBS = -lws2_32
else
    MKDIR = mkdir -p
    CLIENT_OUT = $(BUILD_DIR)/client
    SERVER_OUT = $(BUILD_DIR)/server
    LIBS =
endif

all: create_build_dir
	gcc ./client/client.c -o $(CLIENT_OUT) $(LIBS)
	gcc ./server/server.c -o $(SERVER_OUT) $(LIBS)

create_build_dir:
	$(MKDIR) $(BUILD_DIR)
