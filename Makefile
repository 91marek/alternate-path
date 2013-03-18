all: main graph
		g++ main.o graph.o -lboost_graph -o ap
main: graph Main.cpp
		g++ -c Main.cpp -o main.o
graph: Graph.hpp Graph.cpp
		g++ -c Graph.cpp -o graph.o
clean:
		rm *.o ap
		