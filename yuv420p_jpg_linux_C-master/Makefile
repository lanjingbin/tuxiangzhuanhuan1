test:test.o yuv2jpg.o
	gcc -o test test.o yuv2jpg.o
test.o:test.c
	gcc -c test.c
yuv2jpg:yuv2jpg.c
	gcc -c yuv2jpg.c
.PHONY: all test clean
