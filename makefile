# Shell to use
SHELL := /bin/bash

# Compiler to use
CC := g++

# Defining directories
SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := bin

# Target executables
CLIENT_EXE := $(BIN_DIR)/client
SERVER_EXE := $(BIN_DIR)/server

 # List of all files ending with .cpp
SRC := $(wildcard $(SRC_DIR)/*.cpp)

 # List of object files generated from source files, i.e., foo.cpp -> foo.o
OBJ := $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# Libraries to include
ASIO_INCL := -I/usr/include/asio-1.18.0/include/
SDL2_INCL := -I/usr/include/SDL2/
INCLS := $(ASIO_INCL) $(SDL2_INCL)

# Compile options
CPPFLAGS := -Iinclude $(INCLS) -pthread -std=c++17 -MMD -MP # -MMD and -MP generate dependencies

# Linking options
LDLIBS   := -lboost_serialization -lSDL2 -lpthread

# Default targets when running make
all: $(CLIENT_EXE) $(SERVER_EXE)

.PHONY: all clean # ignore these targets to avoid conflicts with files with same names

# Rules to link .o files (not sophisticated at the moment; each object file is made into a corresponding executable)
$(CLIENT_EXE): obj/client.o | $(BIN_DIR)
	$(CC) $^ $(LDLIBS) -o $@ # $@ is left side of :, $^ is right side of :

$(SERVER_EXE): obj/server.o | $(BIN_DIR)
	$(CC) $^ $(LDLIBS) -o $@

# Rule to create .o files from .cpp files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CC) $(CPPFLAGS) -c $< -o $@ # $< is first item in $(SRC_DIR)/%.cpp

# Make sure these directories exist
$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

clean:
	@$(RM) -rv $(BIN_DIR) $(OBJ_DIR) # The @ disables the echoing of the command

-include $(OBJ:.o=.d) # The dash is used to silence errors if the files don't exist yet
