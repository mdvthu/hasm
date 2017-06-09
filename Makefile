CC = gcc
# -g: add debugging info. -Wall: turn on warnings
CFLAGS = -g -Wall $(INCLUDES)

default: hasm

hasm: src/hasm.o src/functions.o
	$(CC) $(CFLAGS) -o bin/hasm src/hasm.o src/functions.o

# remove executable file and old object files
clean:
	$(RM) $(TARGET) src/*.o src/*~ bin/hasm
