# Compiler and flags
CC = gcc
CFLAGS = -Wall -g 
LDFLAGS = -lSDL3 -lSDL3_ttf

# Source and target
SRC = main.c
OBJ = main.o
TARGET = app

# Default target to build the application
all: $(TARGET)

# Compile the object file from the source file
$(OBJ): $(SRC)
	$(CC) $(CFLAGS) -c $(SRC)

# Link the object file to create the executable
$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)

# Clean up the build files
clean:
	rm -f $(OBJ) $(TARGET) main

run:
	./$(TARGET) 

# Declare phony targets (these aren't files)
.PHONY: all clean run

