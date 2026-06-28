CC = gcc
CFLAGS = -Wall -Wextra -pthread -std=c99
TARGET = mmsv
SRC = mmsv.c

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)
