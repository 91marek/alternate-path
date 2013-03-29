CC = g++
CCFLAGS = -Wall

all: obj ap gg

obj:
	mkdir obj
	
ap: obj/Main.o obj/Graph.o
	$(CC) obj/Main.o obj/Graph.o -lboost_graph -lboost_program_options -lboost_filesystem -lboost_system -o $@

obj/Main.o: src/Main.cpp src/Graph.hpp
	$(CC) -c $(CCFLAGS) src/Main.cpp -o $@

obj/Graph.o: src/Graph.cpp src/Graph.hpp
	$(CC) -c $(CCFLAGS) src/Graph.cpp -o $@

gg: obj/Generator.o
	$(CC) obj/Generator.o -lboost_graph -lboost_program_options -o $@

obj/Generator.o: src/Generator.cpp
	$(CC) -c $(CCFLAGS) src/Generator.cpp -o $@

clean:
	rm -f *.o ap gg
