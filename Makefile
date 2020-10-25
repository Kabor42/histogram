# CC=gcc
CC=clang
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
	@echo Cleaning directories.
	@rm -r -f $(BIN)
	@rm *_new.dat
	@rm *_hist_16.dat
	@rm *_hist_256.dat
	@rm *_hist_256_u.dat
	@rm *_kum_eredeti.dat
	@rm *_kum_uj.dat
	@rm *_uj_ertekek.dat
	@echo Cleaning done.

.PHONY: build clean

$(EXECUTABLE_FILES): $(OBJECT_FILES)
	@$(CC) -o $@ $^ $(LDFLAGS)
	@echo "Build successful!"

$(OBJECT_FILES): $(OBJ)/%.o: $(SRC)/%.c
	@echo Compiling $<
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) -o $@ $<

run: $(EXECUTABLE_FILES)
	@echo Running ${EXECUTABLE_NAME}
	@./bin/${EXECUTABLE_NAME} lena256x256.dat 2B525x311.dat
	@./bin/${EXECUTABLE_NAME} photo1c1280x720.dat Auronzo1280x720rgb.dat
	@./bin/${EXECUTABLE_NAME} photo2c1280x720.dat Braies1280x720rgb.dat
	@./bin/${EXECUTABLE_NAME} secret1280x720.dat
