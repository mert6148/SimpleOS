CC=gcc
CFLAGS=-Wall -Wextra -O2 -m32
TARGET=x86_system
SRC=x86.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)
