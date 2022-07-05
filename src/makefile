TARGET = not-carlsen
LIBS = -lm
CC = gcc
CFLAGS = -s -O3

.PHONY: default all clean

default: $(TARGET)
all: default

OBJECTS = $(wildcard *.c)
HEADERS = $(wildcard *.h)

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) $(LIBS) $(CFLAGS) -o $@

clean:
	-rm -f $(TARGET)