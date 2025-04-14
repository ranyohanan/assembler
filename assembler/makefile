assembler: assembler.o mcros.o as_to_am.o analyser.o binary.o symbols.o am_first.o am_sec.o
	gcc -g -ansi -Wall -pedantic assembler.o mcros.o as_to_am.o analyser.o binary.o symbols.o am_first.o am_sec.o -o assembler -lm
	
assembler.o: assembler.c assembler.h mcros.h as_to_am.h am_first.h
	gcc -g -c -ansi -Wall -pedantic assembler.c -o assembler.o
	
as_to_am.o: as_to_am.c as_to_am.h analyser.h mcros.h
	gcc -g -c -ansi -Wall -pedantic as_to_am.c -o as_to_am.o
	
mcros.o: mcros.c mcros.h analyser.h 
	gcc -g -c -ansi -Wall -pedantic mcros.c -o mcros.o
	
analyser.o: analyser.c analyser.h 
	gcc -g -c -ansi -Wall -pedantic analyser.c -o analyser.o
	
binary.o: binary.c binary.h analyser.h
	gcc -g -c -ansi -Wall -pedantic binary.c -o binary.o

symbols.o: symbols.c symbols.h analyser.h
	gcc -g -c -ansi -Wall -pedantic symbols.c -o symbols.o
	
am_first.o: am_first.c am_first.h analyser.h mcros.h symbols.h binary.h
	gcc -g -c -ansi -Wall -pedantic am_first.c -o am_first.o
	
am_sec.o: am_sec.c am_sec.h analyser.h symbols.h binary.h
	gcc -g -c -ansi -Wall -pedantic am_sec.c -o am_sec.o	
clean:
	rm -f *.o assembler
