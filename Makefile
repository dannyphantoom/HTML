flags=-O2 -Wall 
ldflags= 

.PHONY: all clean

all: clean HTML
HTML: HTML.o
	gcc $(flags) -o HTML HTML.o $(ldflags)
HTML.o: HTML.c
	gcc $(flags) -c HTML.c
clean:
	rm -f HTML HTML.o
