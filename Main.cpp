#include <boost/graph/graphviz.hpp>
#include <boost/program_options.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <iostream>
#include "Graph.hpp"

using namespace boost;
using namespace boost::program_options;
	
int main(int argc, const char* argv[])
{
	// Declare the supported options.
	options_description desc("Allowed options");
	desc.add_options()
		("help", "produce help message")
		("graph", value<int>(), "input graph")
	;
	variables_map vm;
	store(parse_command_line(argc, argv, desc), vm);
	notify(vm);

	if (vm.count("help"))
	{
		std::cout << desc << "\n";
		return 1;
	}

	// Use ref_property_map to turn a graph property into a property map
	//boost::ref_property_map<graph_t*,std::string>
		//gname(get_property(graph,graph_name));
	//dp.property("name",gname);

	// Sample graph as an std::istream;
	std::ifstream in("data/example1.dot");
	Graph g;
	g.readGraph(in);
	Graph::VertexIter v, v_end;
	v= boost::vertices(g.getGraph()).first;
	
	Graph::Vertex v1 = *v;
	v++;
	Graph::Vertex v2 = *v;
	
	g.setEdgeColor(boost::edge(v1, v2, g.getGraph()).first, Graph::RED);
	g.getShortestPath(v1,v2);
	g.writeGraph(std::cout);
	
	return 0;
}
