CC = g++
CCFLAGS = -Wall

all: ap gg
	
ap: Main.o Graph.o
	$(CC) Main.o Graph.o -lboost_graph -lboost_program_options -lboost_filesystem -lboost_system -o $@

Main.o: Main.cpp Graph.hpp
	$(CC) -c $(CCFLAGS) Main.cpp -o $@

Graph.o: Graph.cpp Graph.hpp
	$(CC) -c $(CCFLAGS) Graph.cpp -o $@

gg: Generator.o
	$(CC) Generator.cpp -lboost_graph -lboost_program_options -o $@

Generator.o: Generator.cpp
	$(CC) -c $(CCFLAGS) Generator.cpp -o $@

clean:
	rm -f *.o ap gg
