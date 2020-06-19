# Makefile
CC = g++

cpp_snake : snake.o main.o
	$(CC) -o cpp_snake snake.o main.o -lncurses
	echo "Enjoy Snake"

snake.o : snake.cpp
	$(CC) -c -o snake.o snake.cpp -lncurses

main.o : main.cpp
	$(CC) -c -o main.o main.cpp

clean:
	rm -f *.o 
