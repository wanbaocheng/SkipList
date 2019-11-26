CXX = g++
CXXFLAGS = -Wall -Wextra -pedantic -std=c++11 -g -Ofast
TARGET = test

all: $(TARGET)

$(TARGET): test.cc skiplist.o
	$(CXX) $(CXXFLAGS) -o $(TARGET) test.cc skiplist.h

skiplist.o: skiplist.h
	$(CXX) $(CXXFLAGS) -o skiplist.o -c skiplist.h

clean:
	rm -rf *.o $(TARGET)
