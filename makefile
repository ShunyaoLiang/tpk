cc = gcc
flags = -std=c11 -Wall -Werror
libs = ncurses

obj = actor.o dice.o main.o

tpk: actor.o dice.o main.o
	$(cc) -o tpk $(obj) -l $(libs)

actor.o: actor.h list.h
	$(cc) -c actor.c -l $(libs)

dice.o: dice.h
	$(cc) -c dice.c -l $(libs)

main.o: actor.h dice.h
	$(cc) -c main.c -l $(libs)

clean:
	rm tpk $(obj)
