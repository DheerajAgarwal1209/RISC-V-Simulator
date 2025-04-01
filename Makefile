# Variables
CC = g++
CFLAGS = -Wall -Wextra -std=c++11

# Source and object files
SRC = main.cpp Execution.cpp helping.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = riscv_sim

# Rules
all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

%.o: %.cpp Instruction.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean