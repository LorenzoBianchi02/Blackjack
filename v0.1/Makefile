CC := gcc
SRC := src
OBJ := obj
INC := inc

TARGET := a.out

SRCS := $(shell find $(SRC) -name *.c)
FILES := $(notdir $(basename $(SRCS)))
OBJS := $(addprefix $(OBJ)/,$(addsuffix .o,$(FILES)))

ARGS := 10000000 -tr 1000000000 -wr split_test.txt
FLAGS := -Werror -Wall

LIB :=

all: clear build run

build: $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LIB)

run:
	./$(TARGET) $(ARGS)

$(OBJ)/%.o: $(SRC)/%.c
	$(CC) -I $(INC)/ $(FLAGS) -c $< -o $@

only: build

force:
	$(CC) $(FLAGS) $(SRCS) -o $(TARGET) $(LIB)


clean: clear remove build run

remove:
	-rm -f $(OBJS)

clear:
	clear