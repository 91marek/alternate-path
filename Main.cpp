#include <boost/graph/graphviz.hpp>
#include <boost/program_options.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <iostream>
#include "Graph.hpp"

using namespace boost;
using namespace boost::program_options;
	
int main(int argc, const char* argv[])
{
	std::string filename;
	std::string source;
	std::string target;

	// Declare the supported options.
	options_description desc("Usage: " + std::string(argv[0]) + " [options]\nOptions:");
	desc.add_options()
		("help,h", "Produce help message")
		("file,f", value<std::string>(&filename), "Input graph filename")
		("source,s", value<std::string>(&source), "Source vertex name")
		("target,t", value<std::string>(&target), "Target vertex name")
	;
	positional_options_description p;
	p.add("file", -1);
	variables_map vm;
	store(command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
	notify(vm);

	if (vm.count("help") || !vm.count("file") ||
		!vm.count("source") || !vm.count("target"))
	{
		std::cout << desc << std::endl;
		return 1;
	}
	
	std::cout << "filename: " << filename << "\ns: " << source << "\nt: " << target << std::endl;
	
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
