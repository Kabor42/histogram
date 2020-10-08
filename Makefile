CC=gcc
CFLAGS=-c -Wall -Werror -pedantic -std=gnu99 -g -ggdb
LDFLAGS= -lm
EXECUTABLE_NAME=hist_eq

# Folders
SRC=.
BIN=bin
OBJ=$(BIN)/obj

# Files
SOURCE_FILES=\
	     main.c

EXECUTABLE_FILES = $(EXECUTABLE_NAME:%=$(BIN)/%)
OBJECT_FILES     = $(SOURCE_FILES:%.c=$(OBJ)/%.o)

build: $(EXECUTABLE_FILES)

clean:
	rm -r -f $(BIN)

.PHONY: build clean

$(EXECUTABLE_FILES): $(OBJECT_FILES)
	@$(CC) -o $@ $^ $(LDFLAGS)
	@echo "Build successful!"

$(OBJECT_FILES): $(OBJ)/%.o: $(SRC)/%.c
	@echo Compiling $<
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) -o $@ $<

run: $(EXECUTABLE_FILES)
	./bin/${EXECUTABLE_NAME} ./photo1c1280x720.dat
