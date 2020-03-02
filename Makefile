
build: 2048.c
	gcc -Wall 2048.c -lcurses -o 2048

run: build
	./2048

clean: 
	rm 2048
	
