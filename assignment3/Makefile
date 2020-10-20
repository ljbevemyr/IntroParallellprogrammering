

#
# example Makefile to build an executable named myprog from myprog.cpp
#

all: $(PROG).cpp
	g++ -Wall -pthread -fopenmp prime.cpp -o prime
	gcc -DOUTPUT -Wall -fopenmp Game_Of_Life.c -o gameOfLife
	gcc -Wall -fopenmp cMatrixMultiplication.c -o cMatrixMultiplication
	gcc -DOUTPUT -Wall -fopenmp Game_Of_Life.c -o gameOfLife
	gcc -DOUTPUT -Wall -fopenmp -g gausian_elimination_row.c -o gausian_row
	gcc -DOUTPUT -Wall -fopenmp -g gausian_elimination_col.c -o gausian_col


run_tests:

clean:
	$(RM) $(PROG)
