TARGET = not-carlsen
LIBS = -lm -lpthread
CC = gcc
CFLAGS = -s -O3 -Wl,--stack,67108864

.PHONY: default all clean
.PRECIOUS: $(TARGET) $(OBJECTS)

default: $(TARGET)
all: default

OBJECTS = $(wildcard *.c)
HEADERS = $(wildcard *.h)

$(TARGET): $(OBJECTS)
 	$(CC) $(OBJECTS) $(LIBS) $(CFLAGS) -o $@
#	$(CC) $(OBJECTS) $(LIBS) -static-libgcc -ggdb -o $@
# Run drmemory -- not-carlsen.exe

clean:
	-rm -f $(TARGET)