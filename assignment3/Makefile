

#
# example Makefile to build an executable named myprog from myprog.cpp
#
PROG=prime

all: $(PROG).cpp
	export OMP_NUM_THREADS=4
	g++ -std=c++11 -Wall -pthread -fopenmp $(PROG).cpp -o $(PROG)
	gcc -DOUTPUT -Wall -fopenmp Game_Of_Life.c -o gameOfLife
	gcc -Wall -fopenmp cMatrixMultiplication.c -o cMatrixMultiplication

run_tests:

clean:
	$(RM) $(PROG)
