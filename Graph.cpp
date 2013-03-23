#include "Graph.hpp"
#include <boost/graph/graphviz.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <algorithm>	// make_heap() etc.
#include <vector>
#include <cstdlib> // system()

#include <iostream>

void Graph::readGraph(istream& in)
{
	dynamic_properties dp;
	dp.property("node_id", get(vertex_name, graph));
	dp.property("weight", get(edge_weight, graph));
	read_graphviz(in, graph, dp);
}

void Graph::writeGraph(ostream& out)
{
	dynamic_properties dp;
	dp.property("node_id", get(vertex_name, graph));
	//dp.property("predecessor", get(vertex_predecessor, graph));
	//dp.property("dist", get(vertex_distance, graph));
	dp.property("label", get(edge_weight, graph));//dp.property("weight", get(edge_weight, graph));
	dp.property("color", get(edge_color, graph));
	dp.property("URL", get(edge_name, graph));
	write_graphviz_dp(out, graph, dp);
}

void Graph::setEdgeColor(const Edge& e, Color c)
{
	string color;
	switch(c)
	{
		case RED: color = "red"; break;
		case GREEN: color = "green"; break;
		case BLUE: color = "blue"; break;
		default : color = "black";
	}
	put(edge_color, graph, e, color);
}

void Graph::setEdgesColor(const EdgeList& el, Color c)
{
	BOOST_FOREACH(Edge e, el)
	{
		setEdgeColor(e, c);
	}
}

void Graph::setEdgesURL(const EdgeList& el, const string& base_name, unsigned starting_postfix)
{
	BOOST_FOREACH(Edge e, el)
	{
		setEdgeURL(e, base_name + lexical_cast<string>(starting_postfix++) + ".html");
	}
}

pair<Graph::Vertex, Graph::Vertex> Graph::getVerticesByName(const string& v1, const string& v2) const throw(string)
{
	VertexIter v, v_end;
	pair<Vertex, Vertex> result;
	tie(v, v_end) = vertices(graph);
	for(; v != v_end; ++v)
	{
		const string& v_name = get(vertex_name, graph, *v);
		if(v_name == v1)
		{
			result.first = *v;
			for(++v; v != v_end; ++v)
			{
				if(get(vertex_name, graph, *v) == v2)
				{
					result.second = *v;
					return result;
				}
			}
			throw string("Vertex \"" + v2 + "\" not found.");
			return result;
		}
		else if(v_name == v2)
		{
			result.second = *v;
			for(++v; v != v_end; ++v)
			{
				if(get(vertex_name, graph, *v) == v1)
				{
					result.first = *v;
					return result;
				}
			}
			throw string("Vertex \"" + v1 + "\" not found.");
			return result;
		}
	}
	throw string("Vertices \"" + v1 + "\" and \"" + v2 + "\" not found.");
}

Graph::EdgeList Graph::findShortestPath(const Vertex v1, const Vertex v2)
{
	if(!computeDistances(v1, v2))
		return EdgeList();	// return empty list when v2 is inaccessible from v1
	else
		return readShortestPath(v1, v2);
}

bool Graph::computeDistances(const Vertex v1, const Vertex v2)
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
		make_heap(pq.begin(), pq.end(), CompVertexDist(graph));
		pop_heap(pq.begin(), pq.end(), CompVertexDist(graph));
		Vertex v_source = pq.back();
		pq.pop_back();
		
		double source_distance = get(vertex_distance, graph, v_source);
		
		if(source_distance == numeric_limits<double>::max())
		{
			//all remaining vertices are inaccessible from v1
			return false;
		}
		
		if(v_source == v2)
		{
			//target vertex encountered
			break;
		}
		
		OutEdgeIter e, e_end;
		tie(e,e_end) = out_edges(v_source, graph);
		for(; e!=e_end; ++e){
			Vertex v_target = target(*e, graph);
			double tmp_edge_weight = get(edge_weight, graph, *e);
			double target_distance = get(vertex_distance, graph, v_target);
			//cout<<target_distance<<" "<<source_distance<<" "<<tmp_edge_weight<<endl;
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
	
	return true;
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

void Graph::generateHTML(const string& filename)
{
	// write dot file
	ofstream out_dot;
	out_dot.open((filename + ".dot").c_str());
	if(!out_dot.is_open())
	{
		;//exception?
	}
	writeGraph(out_dot);
	out_dot.close();
	
	// generate map and gif file
	string cmd("neato -Tcmapx -o" + filename + ".map -Tgif -o" + filename + ".gif " + filename + ".dot");
	cout<<"cmd: "<<cmd<<endl;
	system(cmd.c_str());
	
	// write html file
	ofstream out_html;
	out_html.open((filename + ".html").c_str());
	if(!out_html.is_open())
	{
		;//exception?
	}
	out_html<<"<IMG SRC=" + filename + ".gif USEMAP=\"#G\" />"<<endl;
	ifstream in_map;
	in_map.open((filename + ".map").c_str());
	if(!in_map.is_open())
	{
		;//exception?
	}
	istreambuf_iterator<char> src(in_map.rdbuf());
	istreambuf_iterator<char> end;
	ostream_iterator<char> dest(out_html);
	copy(src, end, dest);
	in_map.close();
	// TODO: here the map file can be removed, but shall we do it?
	out_html.close();
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
