CC=g++
FLAGS=-lpthread

TARGET=tema1

run: build
	./tema1 3 4 test0/test.txt

build: $(TARGET)

tema1: tema1.cpp File.cpp
	$(CC) -o $@ $^ $(FLAGS)                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      

.PHONY: clean

clean:
	rm -f $(TARGET) *.txt