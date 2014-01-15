main: tri.o
	gcc -o main TP-DIST.c tri.o
tri.o : tri.c tri.h
	gcc -c tri.c

clean: 
	rm main *.o