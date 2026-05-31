CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra
TARGET = table_cli
SRCS = TableDB.cpp table_cli.cpp

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS)

print: TableDB.cpp print.cpp
	$(CXX) $(CXXFLAGS) -o print TableDB.cpp print.cpp

clean:
	rm -f $(TARGET) *.o

.PHONY: all clean
