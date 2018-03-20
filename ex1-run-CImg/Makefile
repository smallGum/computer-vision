TEST_LIB = -lgtest_main -lgtest
THREAD_FILE = -lpthread
IMG_LIB = -lX11
TEST_FILE = ./src/hw1_test.cpp
CPP_FILE = ./src/hw1.cpp ./src/hw1_main.cpp
HEADER_FILE = ./header/hw1.h ./header/CImg.h

all: hw1_main hw1_test

.PHONY: all

hw1_main: $(CPP_FILE) $(HEADER_FILE)
	g++ -Wall -o hw1_main $(HEADER_FILE) $(CPP_FILE) $(THREAD_FILE) $(IMG_LIB)

hw1_test: $(TEST_FILE) ./src/hw1.cpp $(HEADER_FILE)
	g++ -Wall -o hw1_test $(HEADER_FILE) ./src/hw1.cpp $(TEST_FILE) $(TEST_LIB) $(THREAD_FILE) $(IMG_LIB)
