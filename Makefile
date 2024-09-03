CC := gcc
SRC := src
OBJ := obj
INC := inc

TARGET := a.out

SRCS := $(shell find $(SRC) -name '*.c')
FILES := $(notdir $(basename $(SRCS)))
OBJS := $(addprefix $(OBJ)/,$(addsuffix .o,$(FILES)))

ARGS := 
#FLAGS := -Werror -Wall
FLAGS :=
LIB :=

all: clean build run

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


push:
	git add .
	git commit -m "auto generated message"
	git push
