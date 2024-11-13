# Compiler and flags
CC = gcc
CFLAGS = -m32 -no-pie -nostdlib

# Output files
OUTPUT_FIB = ../bin/fib
OUTPUT_SUM = ../bin/sum

# Default target
all: $(OUTPUT_FIB) $(OUTPUT_SUM)

# Rule to build the fib executable
$(OUTPUT_FIB): fib.c
	$(CC) $(CFLAGS) -o $(OUTPUT_FIB) fib.c

# Rule to build the sum executable
$(OUTPUT_SUM): sum.c
	$(CC) $(CFLAGS) -o $(OUTPUT_SUM) sum.c

# Clean up
clean:
	rm -f $(OUTPUT_FIB) $(OUTPUT_SUM)

.PHONY: all clean

