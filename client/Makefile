CC=gcc
CFLAGS= -I/usr/include/libxml2 -Iinclude -Wextra -DDEBUG -g -o0 `xml2-config --cflags`
LIBS= `xml2-config --libs`

TARGET=main

SRC_DIR=src
HEADER_DIR=include

SOURCES  = $(wildcard $(SRC_DIR)/*.c) $(wildcard $(SRC_DIR)/*/*.c)
HEADERS  = $(wildcard $(HEADER_DIR)/*.h) $(wildcard $(HEADER_DIR)/*/*.h)

build: $(SOURCES) $(TARGET)

$(TARGET): $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) $(SOURCES) -o $@ `xml2-config --libs` `xml2-config --cflags`

clean:
	rm -rf main

.PHONY: build clean