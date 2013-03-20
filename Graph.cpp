#include "Graph.hpp"
#include <boost/graph/graphviz.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <queue>

class CompVertexDist {
		const Graph::GraphContainer& graph;
	public:
		CompVertexDist(const Graph::GraphContainer& g): graph(g) {}
		bool operator()(const Graph::Vertex& v1, const Graph::Vertex& v2) const 
		{
			return get(vertex_distance, graph, v1) < get(vertex_distance, graph, v2);
		}
};

void Graph::readGraph(std::istream& in)
{
	dynamic_properties dp;
	dp.property("node_id", get(vertex_name, graph));
	dp.property("weight", get(edge_weight, graph));
	read_graphviz(in, graph, dp);
}

void Graph::writeGraph(std::ostream& out)
{
	dynamic_properties dp;
	dp.property("node_id", get(vertex_name, graph));
	dp.property("weight", get(edge_weight, graph));
	dp.property("color", get(edge_color, graph));
	dp.property("URL", get(edge_name, graph));
	write_graphviz_dp(out, graph, dp);
}

void Graph::setEdgeColor(const Edge& e, Color c)
{
	std::string color;
	switch(c)
	{
		case RED: color = "red"; break;
		case GREEN: color = "green"; break;
		case BLUE: color = "blue"; break;
		default : color = "black";
	}
	put(edge_color, graph, e, color);
}

const Graph::GraphContainer& Graph::getGraph()
{
	return graph;
}

Graph::EdgeList Graph::getShortestPath(const Vertex& v1, const Vertex& v2)
{
	EdgeList result;
	/* Reset all vertexes to default properties */
	VertexIter v, v_end;
	tie(v,v_end) = vertices(graph);
	for(v; v!=v_end; v++)
	{
		put(vertex_distance, graph, *v, numeric_limits<double>::max());
		put(vertex_predecessor, graph, *v, -1);
	}
	/* Set lenght of shortest path from v1 to v1 to 0 */
	put(vertex_distance, graph, v1, 0);
	
	priority_queue<Vertex, deque<Vertex>, CompVertexDist> pq (CompVertexDist(graph));
	return result;
}

