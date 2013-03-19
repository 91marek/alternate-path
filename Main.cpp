#include <boost/graph/graphviz.hpp>
#include <boost/program_options.hpp>
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
	
	// Vertex properties
	typedef property < vertex_name_t, std::string,
		property < vertex_color_t, float > > vertex_p;
	// Edge properties
	typedef property < edge_weight_t, double > edge_p;
	// Graph properties
	typedef property < graph_name_t, std::string > graph_p;
	// adjacency_list-based type
	typedef adjacency_list < vecS, vecS, directedS,
		vertex_p, edge_p, graph_p > graph_t;

	// Construct an empty graph and prepare the dynamic_property_maps.
	graph_t graph(0);
	dynamic_properties dp;

	property_map<graph_t, vertex_name_t>::type name =
		get(vertex_name, graph);
	dp.property("node_id",name);

	property_map<graph_t, vertex_color_t>::type mass =
		get(vertex_color, graph);
	dp.property("mass",mass);

	property_map<graph_t, edge_weight_t>::type weight =
		get(edge_weight, graph);
	dp.property("weight",weight);

	// Use ref_property_map to turn a graph property into a property map
	boost::ref_property_map<graph_t*,std::string>
		gname(get_property(graph,graph_name));
	dp.property("name",gname);

	// Sample graph as an std::istream;
	std::istringstream
		gvgraph("digraph { graph [name=\"graphname\"]  a c e [mass = 6.66] a->c [weight=1.0]}");

	bool status = read_graphviz(gvgraph,graph,dp,"node_id");
	
	write_graphviz_dp(std::cout, graph, dp, std::string("node_id"));
	return 0;
}
