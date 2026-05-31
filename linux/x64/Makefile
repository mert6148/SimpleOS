CXX=g++
CXXFLAGS=-Wall -Wextra -O2 -m64 -std=c++17
TARGET=x64_system
SRC=x64.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)
