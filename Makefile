#! /usr/bin/make
# Makefile for Bandwidth Simulator

#TARGET    = chat
TEST_MENU  = test_menu

# compilers with one miscellaneous setting
# -pipe: link in memory instead of temp files
#CXX = g++ -pipe
CC  = gcc -std=c99 -pipe

# compiler flags
FLAGS    = -pedantic -ggdb -I$(INC_DIR) -I$(SRC_DIR)
#CXXFLAGS = -std=c++0x $(FLAGS)
CFLAGS   = -O2 $(FLAGS)

LINKER   = gcc -std=c99 -pipe -o

#linking flags
LDLIBS = -Wall -I$(INC_DIR) -I$(SRC_DIR) -lm
LDFLAGS = $(LDLIBS)

INC_DIR = include
LIB_DIR = lib
OBJ_DIR = obj
OUT_DIR = bin
SRC_DIR = src
TST_DIR = test

SOURCES := $(wildcard $(SRC_DIR)/*.c)
HEADERS := $(wildcard $(INC_DIR)/*.h)
OBJECTS := $(SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

rm       = rm -f

#$(OUT_DIR)/$(TARGET): $(OBJECTS)
#  @$(LINKER) $@ $(LDFLAGS) $(OBJECTS)
#  @echo "linking complete"

$(OUT_DIR)/$(TEST_MENU): $(OBJECTS)
	$(LINKER) $@ test/test_menu.c $(LDFLAGS) $(OBJECTS)

$(TESTS): $(OBJ_DIR)/%.o : $(TST_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJECTS): $(OBJ_DIR)/%.o : $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@
	@echo "Compiled "$<" successfully."

.PHONEY: clean

clean:
	$(rm) $(OBJECTS)
	@echo "Cleanup complete!"

.PHONEY: remove
remove: clean
	@$(rm) $(OUT_DIR)/*
	@echo "Executable removed!"
