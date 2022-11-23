NAME=shell

SRC_DIR=./src/
BLD_DIR=./build/
BIN_DIR=./bin/

BIN=$(BIN_DIR)$(NAME)
OBJ=$(BLD_DIR)shell_main_prompt.o $(BLD_DIR)shell_utils.o $(BLD_DIR)shell_subprocess.o
HEADERS=$(SRC_DIR)shell_utils.h $(SRC_DIR)shell_subprocess.h

# --------

DEBUG_FLAGS=-g -fsanitize=address

CC=clang
CFLAGS=-Wall -Wextra -pedantic $(DEBUG_FLAGS)

LD=$(CC)
LDFLAGS=$(CFLAGS)

# --------

all: $(BIN)

clean:
	rm $(OBJ) $(BIN) || true

# --------

$(BIN): $(OBJ)
	$(LD) $(LDFLAGS) -o $@ $^

$(BLD_DIR)%.o: $(SRC_DIR)%.c $(HEADERS)
	$(CC) $(CFLAGS) -c -o $@ $<


