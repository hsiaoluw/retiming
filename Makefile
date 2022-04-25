code_constraint: main.o load.o circuit.o
	g++ -o code_constraint main.o load.o circuit.o
main.o: main.cc circuit.h
	g++ -g -c -Wall main.cc 
load.o: load.cc circuit.h
	g++ -g -c -Wall load.cc
circuit.o: circuit.cc circuit.h
	g++ -g -c -Wall circuit.cc
clean:
	rm -f *.o code_constraint
