TARGET = not-carlsen
LIBS = -lm
CC = gcc
CFLAGS = -O3 -w
# CFLAGS = -g -O0 -Wl,--stack,67108864 -w # GDB Debug Flags; gdb not-carlsen.exe, run

.PHONY: default all clean
.PRECIOUS: $(TARGET) $(OBJECTS)

default: $(TARGET)
all: default

OBJECTS = $(wildcard *.c)
HEADERS = $(wildcard *.h)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) $(LIBS) $(CFLAGS) -o $@

clean:
	-rm -f $(TARGET)