#
# example Makefile to build an executable named myprog from myprog.cpp
#
PROG=primeLP

all: $(PROG).cpp
	g++ -std=c++11 -Wall -pthread $(PROG).cpp -o $(PROG)
	g++ -Wall -std=c++11 -pthread -O3 benchmark_example1.cpp -o bench1
	g++ -Wall -std=c++11 -pthread -O3 benchmark_example2.cpp -o bench2
	g++ -Wall -std=c++11 -pthread -O3 benchmark_example3.cpp -o bench3
	g++ -Wall -std=c++11 -pthread -O3 benchmark_example4.cpp -o bench4

run_tests:
	./bench1 1
	./bench1 2
	./bench1 4
	./bench1 8
	./bench1 16
	./bench2 1
	./bench2 2
	./bench2 4
	./bench2 8
	./bench2 16
	./bench3 1
	./bench3 2
	./bench3 4
	./bench3 8
	./bench3 16
	./bench4 1
	./bench4 2
	./bench4 4
	./bench4 8
	./bench4 16



clean:
	$(RM) $(PROG)
