
	gcc -c -ansi -Wall -pedantic main.c -o main.o -lm 


	gcc -c -ansi -Wall -pedantic firstrun.c -o firstrun.o


	gcc -c -ansi -Wall -pedantic secRun.c -o secrun.o


	gcc -c -ansi -Wall -pedantic GetMethods.c  -o GetMethods.o

	
	gcc -c -ansi -Wall -pedantic CheckMethods.c -o CheckMethods.o

	gcc -g -ansi -Wall -pedantic main.o firstrun.o secrun.o GetMethods.o CheckMethods.o -o assembler -lm


