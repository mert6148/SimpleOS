CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -O2
TARGET = factory_app
SRCS = Factory.c main.c

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)

clean:
	rm -f $(TARGET) *.o

.PHONY: all clean
