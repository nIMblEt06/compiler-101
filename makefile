output: driver.o first_and_follow.o lexer.o parser.o
	gcc -w -O3 driver.o first_and_follow.o lexer.o parser.o -o compiler

lexer.o: lexer.h lexer.c 
	gcc -w -c -g lexer.c

parser.o: parser.h parser.c 
	gcc -w -c -g parser.c

first_and_follow.o: first_and_follow.h first_and_follow.c
	gcc -w -c -g first_and_follow.c

driver.o: driver.c
	gcc -w -c -g driver.c

clean:
	rm -f *.o compiler