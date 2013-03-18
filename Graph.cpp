#include "Graph.hpp"
#include <boost/graph/graphviz.hpp>

void Graph::readGraph(std::istream& in)
{
	dynamic_properties dp;
	dp.property("node_id", get(vertex_name, graph));
	dp.property("weight", get(edge_weight, graph));
	read_graphviz(in, graph, dp, "node_id");
}
