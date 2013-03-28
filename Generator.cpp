#include <boost/program_options.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <fstream>
#include "Graph.hpp"
#include <map>
#include <vector>
#include <boost/graph/graphviz.hpp>
#include <stdlib.h>     /* srand, rand */
#include <time.h> 

using namespace std;
using namespace boost;
using namespace boost::program_options;

// Vertex properties
typedef property <vertex_name_t, int> VertexProperty;
// Edge properties
typedef property <edge_weight_t, double> EdgeProperty;
// adjacency_list-based graph type
typedef adjacency_list<setS, vecS, undirectedS, VertexProperty, EdgeProperty> GraphContainer;

/* a bunch of graph-specific typedefs */
typedef graph_traits<GraphContainer>::vertex_descriptor Vertex;
typedef graph_traits<GraphContainer>::edge_descriptor Edge;

typedef graph_traits<GraphContainer>::vertex_iterator VertexIter;
typedef graph_traits<GraphContainer>::edge_iterator EdgeIter;
typedef graph_traits<GraphContainer>::out_edge_iterator OutEdgeIter;
typedef map<int, Vertex> VertexMap;

int main(int argc, const char* argv[])
{
	srand (time(NULL));
	int n, e;
	double prob;
	string filename;
	VertexMap vertexes;
	// Declare the supported options.
	options_description desc("Usage: " + string(argv[0]) + " [options]\nOptions:");
	desc.add_options()
		("help,h", "Produce help message")
		("output,o", value<string>(&filename), "Result graph filename")
		("num,n", value<int>(&n), "Number of vertexes")
		("edges,e", value<int>(&e), "Number of edges")
		("prob,p", value<double>(&prob), "Probability, that edge will be moved")
	;
	positional_options_description p;
	p.add("output", -1);
	variables_map vm;
	store(command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
	notify(vm);

	if(vm.count("help") || !vm.count("output") ||
		!vm.count("num") || !vm.count("edges") || !vm.count("prob"))
	{
		cerr << desc << endl;
		return EXIT_FAILURE;
	}
	ofstream result(filename.c_str());
	if(!result.is_open())
	{
		cerr<<"Unable to open file \""<<filename<<"\"."<<endl;
		return EXIT_FAILURE;
	}
	GraphContainer graph;
	for(int i = 0; i<n; ++i) //For num of vertexes
	{
		vertexes[i] = add_vertex(graph);
		put(vertex_name, graph, vertexes[i], i);
	}
	for(int i = 0; i<n; ++i) //For num of vertexes
	{
		for(int j =i+1; j<i+1+e; ++j)//For num of edges to create
		{
			Edge new_e = add_edge(vertexes[i], vertexes[j%n], graph).first;
			put(edge_weight, graph, new_e, 1);
		}
	}
	EdgeIter ei, ei_end;
	vector<Edge> edge_vec;
	for (tie(ei, ei_end) = edges(graph); ei != ei_end; ++ei)
	{
		edge_vec.push_back(*ei);
	}
	
	BOOST_FOREACH(Edge& e, edge_vec)
	{
		if(rand() % 100 < prob)
		{	
			Vertex v_sour = source(e, graph);
			Vertex v_targ = target(e, graph);
			Vertex new_targ = vertexes[rand()%n];
			while(new_targ == v_sour)
				new_targ = vertexes[rand()%n];
			remove_edge(e, graph);
			Edge new_e = add_edge(v_sour, new_targ, graph).first;
			put(edge_weight, graph, new_e, 1);
		}
	}
	
	dynamic_properties dp;
	dp.property("node_id", get(vertex_name, graph));
	dp.property("weight", get(edge_weight, graph));
	write_graphviz_dp(result, graph, dp);
	result.close();
	return EXIT_SUCCESS;
}
