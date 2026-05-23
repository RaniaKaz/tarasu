CC = gcc
CFLAGS = -Wall -Iinclude

SRC_DIR = src
INC_DIR = include

SRC = $(SRC_DIR)/main.c $(SRC_DIR)/archive.c $(SRC_DIR)/extract.c $(SRC_DIR)/utils.c
OBJ = $(SRC:.c=.o)
EXEC = tarsau

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(EXEC) $(SRC_DIR)/*.o
