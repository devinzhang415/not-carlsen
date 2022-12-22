TARGET = not-carlsen
LIBS = -lm -lpthread
CC = gcc
CFLAGS = -O3 -Wl,--stack,67108864 -w
# CFLAGS = -g -O3 -Wl,--stack,67108864 -w # GDB Debug Flags

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