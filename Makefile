all: main

main:
	gcc main.c -o simple-wget -g 

clean:
	rm -f simple-wget