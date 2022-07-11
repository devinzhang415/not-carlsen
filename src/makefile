TARGET = not-carlsen
LIBS = -lm -lpthread
CC = gcc
CFLAGS = -s -O3

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