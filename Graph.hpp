#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <boost/graph/adjacency_list.hpp>

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
	
	/* constructors etc. */
	Graph()
	{}

	Graph(const Graph& g) :
		graph(g.graph)
	{}

	virtual ~Graph()
	{}
	
	/* read & write */
	void readGraph(istream& in);
	void writeGraph(ostream& out);
	void generateHTML(const std::string& filename);//TODO nie wiem nic o parametrach, które tu beda.
	
	EdgeList findShortestPath(const Vertex v1, const Vertex v2);
	
	/* getters */
	std::pair<Vertex, Vertex> getVerticesByName(const string& v1, const string& v2) const throw(std::string);
	
	const std::string& getVertexName(const Vertex& v) const
	{
		return get(vertex_name, graph, v);
	}
	
	Vertex getSourceVertex(const Edge& e) const
	{
		return source(e, graph);
	}
	
	Vertex getTargetVertex(const Edge& e) const
	{
		return target(e, graph);
	}
	
	double getEdgeWeight(const Edge& e) const
	{
		return get(edge_weight, graph, e);
	}
	
	const std::string& getEdgeURL(const Edge& e) const
	{
		return get(edge_name, graph, e);
	}
	
	/* setters */
	enum Color{RED, GREEN, BLUE, BLACK};
	void setEdgeColor(const Edge& e, Color c);
	void setEdgesColor(const EdgeList& el, Color c);
	
	void setEdgeWeight(const Edge& e, double weight)
	{
		put(edge_weight, graph, e, weight);
	}
	
	void setEdgeURL(const Edge& e, const std::string& url)
	{
		put(edge_name, graph, e, url);
	}
	
	/* add & remove edge */
	Edge addEdge(const Vertex& v1, const Vertex& v2)
	{
		// TODO: czy potrzebna jest informacja o tym czy udało się dodać (second)? chyba nie
		return add_edge(v1, v2, graph).first;
	}
	
	void removeEdge(const Vertex& v1, const Vertex& v2)
	{
		remove_edge(v1, v2, graph);
	}
	
	/* DEBUG */
	void DEBUGprint(EdgeList& e);
private:
	GraphContainer graph;
	
	bool computeDistances(const Vertex v1, const Vertex v2);
	EdgeList readShortestPath(const Vertex v1, const Vertex v2);
};

class CompVertexDist {
	const Graph::GraphContainer& graph;
public:
	CompVertexDist(const Graph::GraphContainer& g): graph(g) {}
	bool operator()(const Graph::Vertex v1, const Graph::Vertex v2) const 
	{
		return get(vertex_distance, graph, v1) > get(vertex_distance, graph, v2);
	}
};

#endif
