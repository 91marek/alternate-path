#include "Graph.hpp"
#include <boost/graph/graphviz.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/foreach.hpp>
#include <algorithm>
#include <vector>

#include <iostream>

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
	dp.property("predecessor", get(vertex_predecessor, graph));
	dp.property("dist", get(vertex_distance, graph));
	dp.property("weight", get(edge_weight, graph));
	dp.property("color", get(edge_color, graph));
	dp.property("URL", get(edge_name, graph));
	write_graphviz_dp(out, graph, dp);
}

void Graph::setEdgeColor(const Edge e, Color c)
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

Graph::EdgeList Graph::getShortestPath(const Vertex v1, const Vertex v2)
{
	calculateDistances(v1, v2);
	return readShortestPath(v1, v2);
}

void Graph::calculateDistances(const Vertex v1, const Vertex v2)
{
	// priority queue
	vector<Vertex> pq = vector<Vertex>(num_vertices(graph));
	/* Reset all vertices to default properties */
	VertexIter v, v_end;
	tie(v,v_end) = vertices(graph);
	for(unsigned i=0; v!=v_end; v++, ++i)
	{
		put(vertex_distance, graph, *v, numeric_limits<double>::max());
		put(vertex_predecessor, graph, *v, *v);
		pq[i] = *v;
	}
	
	/* Set length of shortest path from v1 to v1 to 0 */
	put(vertex_distance, graph, v1, 0);		
	
	/* Run algorithm */
	while(!pq.empty())
	{
		std::make_heap(pq.begin(), pq.end(), CompVertexDist(graph));
		std::pop_heap(pq.begin(), pq.end(), CompVertexDist(graph));
		Vertex v_source = pq.back();
		pq.pop_back();
		cout<<"Rozmiar kolejki: "<<pq.size()<<endl;
		if(v_source == v2)
			break;
		double source_distance = get(vertex_distance, graph, v_source);
		
		if( source_distance == numeric_limits<double>::max() )
		{
			//all remaining vertices are inaccessible from source
			break;
		}
		
		OutEdgeIter e, e_end;
		tie(e,e_end) = out_edges(v_source, graph);
		for(; e!=e_end; ++e){
			Vertex v_target = target(*e, graph);
			double tmp_edge_weight = get(edge_weight, graph, *e);
			double target_distance = get(vertex_distance, graph, v_target);
			cout<<target_distance<<" "<<source_distance<<" "<<tmp_edge_weight<<endl;
			if( target_distance > source_distance + tmp_edge_weight )
			{
				put(vertex_distance, graph, v_target, source_distance + tmp_edge_weight);
				put(vertex_predecessor, graph, v_target, v_source);
			}
		}
	}

	cout << "Distances:" << endl;
	tie(v,v_end) = vertices(graph);
	for(; v!=v_end; v++)
	{
		cout << get(vertex_name, graph, *v) << ": " << get(vertex_distance, graph, *v) << endl;
	}
}

Graph::EdgeList Graph::readShortestPath(const Vertex v1, const Vertex v2)
{
	EdgeList result;
	Vertex v = v2;
	while(v!=v1)
	{
		Vertex new_v = get(vertex_predecessor, graph, v);
		result.push_front( edge(v, new_v, graph).first );
		v = new_v;		
	}
	DEBUGprint(result);
	return result;
	
}

/* DEBUGS */
void Graph::DEBUGprint(EdgeList& el)
{
	cout<<"Shortest Path:"<<endl;
	BOOST_FOREACH(Edge &e, el)
	{
		cout<<get(vertex_name, graph, source(e, graph))<<"--"<<get(vertex_name, graph, target(e, graph))<<endl;
	}
}
