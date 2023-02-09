wordpairs: main.o hashFunctions.o 
	gcc -g -o wordpairs main.o hashFunctions.o
main.o: main.c headers.h
	gcc -g -c main.c
init.o: hashFunctions.c headers.h
	gcc -g -c hashFunctions.c
