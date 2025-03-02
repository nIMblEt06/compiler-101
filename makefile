output: driver.o first_and_follow.o lexer.o parser.o terminals.o symbolTable.o grammar.o
	gcc -w -O3 driver.o first_and_follow.o lexer.o parser.o terminals.o symbolTable.o grammar.o -o compiler

lexer.o: lexer.h lexer.c 
	gcc -w -c -g lexer.c

parser.o: parser.h parser.c 
	gcc -w -c -g parser.c

first_and_follow.o: first_and_follow.h first_and_follow.c
	gcc -w -c -g first_and_follow.c

driver.o: driver.c
	gcc -w -c -g driver.c

terminals.o: terminals.c symbolTable.h
	gcc -w -c -g terminals.c

symbolTable.o: symbolTable.c symbolTable.h
	gcc -w -c -g symbolTable.c

grammar.o: grammar.c grammar.h
	gcc -w -c -g grammar.c

clean:
	rm -f *.o compiler