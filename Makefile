CC=g++

all:	hashtagcounter

hashtagcounter:	main.o
	$(CC) main.o -o hashtagcounter

main.o:	main.cpp
	$(CC) -c main.cpp

clean:
	rm -rf *.o hashtagcounter
