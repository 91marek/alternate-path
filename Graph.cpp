#include "Graph.hpp"
#include <boost/graph/graphviz.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/foreach.hpp>


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
	VertexDistPriorityQueue pq = VertexDistPriorityQueue(CompVertexDist(graph));
	//cout<<pq.size();
	/* Reset all vertexes to default properties */
	VertexIter v, v_end;
	tie(v,v_end) = vertices(graph);
	for(v; v!=v_end; v++)
	{
		put(vertex_distance, graph, *v, numeric_limits<double>::max());
		put(vertex_predecessor, graph, *v, -1);
		
		/* Set lenght of shortest path from v1 to v1 to 0 */
		if(*v == v1)
			put(vertex_distance, graph, v1, 0);
		pq.push(*v);
	}
	/* Run algorithm */
	while(!pq.empty())
	{
		const Vertex& v_source = pq.top();
		double source_distance = get(vertex_distance, graph, v_source);
		pq.pop();
		OutEdgeIter e, e_end;
		tie(e,e_end) = out_edges(v_source, graph);
		for(e; e!=e_end; ++e){
			const Vertex& v_target = target(*e, graph);
			double tmp_edge_weight = get(edge_weight, graph, *e);
			double target_distance = get(vertex_distance, graph, v_target);
			if( target_distance > source_distance + tmp_edge_weight )
			{
				put(vertex_distance, graph, v_target, source_distance + tmp_edge_weight);
				put(vertex_predecessor, graph, v_target, v_source);
			}
		}
	}

	
	
	return result;
}

