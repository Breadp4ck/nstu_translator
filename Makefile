SHELL=/bin/bash

CC = g++
CFLAGS = -c -g -Wall -pg
LDFLAGS = -W -pg

SRC_DIR = src/
BIN_DIR = bin/
REPORT_DIR = report/

PROGRAM_NAME = $(BIN_DIR)translator

SRCS := $(wildcard $(SRC_DIR)*.cpp)
OBJS := $(SRCS:%.cpp=%.o)
BINS := $(SRCS:%.cpp=%~)

.INTERMEDIATE: $(OBJS)

all: bin remove_bin link

run: link
	./$(PROGRAM_NAME)

remove_bin:
	rm -rvf $(PROGRAM_NAME)

bin:
	mkdir bin

link: $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $(PROGRAM_NAME)

%.o: %.cpp
	@echo "Compiling "$<" to object file "$@"..."
	$(CC) $(CFLAGS) $< -o $@

clean:
	@echo "Cleaning up..."
	rm -rvf $(PROGRAM_NAME)
