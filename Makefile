
CXX := g++
CXXFLAGS := -std=c++17 -Iinclude -Wall -Wextra

SRC := src/main.cpp src/core/*.cpp src/utils/*.cpp
TARGET := procmanager

all:
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)

.PHONY: all clean
