

#
# example Makefile to build an executable named myprog from myprog.cpp
#
PROG=prime

all: $(PROG).cpp
	g++ -Wall -pthread -fopenmp prime.cpp -o prime
	gcc -DOUTPUT -Wall -fopenmp Game_Of_Life.c -o gameOfLife
	gcc -DOUTPUT -Wall -fopenmp Game_Of_Life_Original.c -o gameOfLifeOriginal

run_tests:

clean:
	$(RM) $(PROG)
