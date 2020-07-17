
main:main.o bmp.o
	gcc -o main bmp.o main.o -lm
bmp.o:bmp.c
	gcc -c bmp.c
main.o:main.c
	gcc -c main.c 

.PHONY: all main clean
