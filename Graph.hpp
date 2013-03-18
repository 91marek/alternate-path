#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>
#include <iostream>

#ifndef GRAPH_HPP
#define GRAPH_HPP

using namespace boost;

class Graph
{
public:
	typedef adjacency_list_traits<listS, vecS, undirectedS>::vertex_descriptor vertex_descriptor;
	// Vertex properties
	typedef property < vertex_name_t, std::string,
			property < vertex_distance_t, double,
			property < vertex_predecessor_t, vertex_descriptor > > > VertexProperty;
	// Edge properties
	typedef property < edge_weight_t, double,
			property < edge_color_t, default_color_type > > EdgeProperty;
	// Graph properties
	typedef property < graph_name_t, std::string > GraphProperty;
	// adjacency_list-based graph type
    typedef adjacency_list<listS, vecS, undirectedS, VertexProperty, EdgeProperty> GraphContainer;
    
	/* a bunch of graph-specific typedefs */
	typedef graph_traits<GraphContainer>::vertex_descriptor Vertex;
	typedef graph_traits<GraphContainer>::edge_descriptor Edge;
	
	/* constructors etc. */
	Graph()
	{}

	Graph(const Graph& g) :
		graph(g.graph)
	{}

	virtual ~Graph()
	{}
	
	void readGraph(const std::istream& in);
private:
	GraphContainer graph;
};
#endif
