#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <boost/graph/adjacency_list.hpp>
#include <iostream>
#include <queue>
using namespace boost;
using namespace std;


class CompVertexDist;
class Graph
{
public:
	typedef adjacency_list_traits<listS, vecS, undirectedS>::vertex_descriptor vertex_descriptor_tmp;
	
	// Vertex properties
	typedef property < vertex_name_t, std::string,
			property < vertex_distance_t, double,
			property < vertex_predecessor_t, vertex_descriptor_tmp > > > VertexProperty;
	// Edge properties
	typedef property < edge_weight_t, double,
			property < edge_color_t, std::string,
			property < edge_name_t, std::string > > > EdgeProperty;
	// Graph properties
	typedef property < graph_name_t, std::string > GraphProperty;
	// adjacency_list-based graph type
    typedef adjacency_list<listS, vecS, undirectedS, VertexProperty, EdgeProperty> GraphContainer;
    
	/* a bunch of graph-specific typedefs */
	typedef graph_traits<GraphContainer>::vertex_descriptor Vertex;
	typedef graph_traits<GraphContainer>::edge_descriptor Edge;
	
	typedef graph_traits<GraphContainer>::vertex_iterator VertexIter;
	typedef graph_traits<GraphContainer>::edge_iterator EdgeIter;
	typedef graph_traits<GraphContainer>::out_edge_iterator OutEdgeIter;
	typedef list<Edge> EdgeList;
	typedef priority_queue<Vertex, deque<Vertex>, CompVertexDist> VertexDistPriorityQueue;
	/* constructors etc. */
	Graph()
	{}

	Graph(const Graph& g) :
		graph(g.graph)
	{}

	virtual ~Graph()
	{}
	const GraphContainer& getGraph();
	
	/* properties accessors */
	
	/* IO */
	void readGraph(std::istream& in);
	void writeGraph(std::ostream& out);
//private:
	GraphContainer graph;
	
	enum Color{RED, GREEN, BLUE, BLACK};
	void setEdgeColor(const Edge& e, Color c);
	
	EdgeList getShortestPath(const Vertex& v1, const Vertex& v2);
	bool compVertexDist(const Vertex& v1, const Vertex& v2);
	
};
#endif

class CompVertexDist {
		const Graph::GraphContainer& graph;
	public:
		CompVertexDist(const Graph::GraphContainer& g): graph(g) {}
		bool operator()(const Graph::Vertex& v1, const Graph::Vertex& v2) const 
		{
			return get(vertex_distance, graph, v1) > get(vertex_distance, graph, v2);
		}
};
