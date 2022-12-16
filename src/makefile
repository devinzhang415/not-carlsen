TARGET = not-carlsen
LIBS = -lm -lpthread
CC = gcc
CFLAGS = -O3 -Wl,--stack,67108864 -w

# GDB Debug: `gdb not-carlsen`, `run`, ..., `bt`
# CFLAGS = -g -O0 -Wl,--stack,67108864 -w

.PHONY: default all clean
.PRECIOUS: $(TARGET) $(OBJECTS)

default: $(TARGET)
all: default

OBJECTS = $(wildcard *.c)
HEADERS = $(wildcard *.h)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) $(LIBS) $(CFLAGS) -o $@

# DRMEM Debug: `drmemory -- not-carlsen.exe`
# $(TARGET): $(OBJECTS)
# 	$(CC) $(OBJECTS) $(LIBS) -static-libgcc -ggdb -o $@

clean:
	-rm -f $(TARGET)