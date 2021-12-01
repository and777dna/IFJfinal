CFLAGS= -std=gnu99 -Wall -Wextra -Werror -pedantic

all:	parser
parser:	parser.c
	gcc main.c parser.c symtable.c scanner.c expression.c interpreter.c -g -o main
run	:	
	./main < ifj21.tl > test.out
clean: 
	rm -f *.o main test.out