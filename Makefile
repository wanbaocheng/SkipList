CXX = g++
CXXFLAGS = -Wall -Wextra -pedantic -std=c++11 -g -O2
TARGET = test

#-fsanitize=thread
#-fsanitize=address -fsanitize=leak 
all: $(TARGET)

$(TARGET): test.cc skiplist.o
	$(CXX) $(CXXFLAGS) -o $(TARGET) test.cc skiplist.h

skiplist.o: skiplist.h
	$(CXX) $(CXXFLAGS) -o skiplist.o -c skiplist.h

clean:
	rm -rf *.o $(TARGET)
