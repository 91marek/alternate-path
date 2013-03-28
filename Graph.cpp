#include "Graph.hpp"
#include <boost/graph/graphviz.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <algorithm>	// make_heap() etc.
#include <vector>
#include <cstdlib> // system()
#include <iostream>
#include <fstream>

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
	dp.property("label", get(edge_weight, graph));
	dp.property("color", get(edge_color, graph));
	dp.property("URL", get(edge_name, graph));
	write_graphviz_dp(out, graph, dp);
}

void Graph::generateHTML(const string& base_name, const string& outdir) throw(string)
{
	// write dot file
	ofstream out_dot;
	string filename_dot = filePath(outdir, base_name) + ".dot";
	out_dot.open(filename_dot.c_str());
	if(!out_dot.is_open())
	{
		throw string("Unable to open file \"" + filename_dot + "\".");
	}
	writeGraph(out_dot);
	out_dot.close();
	
	// generate map and gif file
	string filename_map = filePath(outdir, base_name) + ".map";
	string filename_gif = filePath(outdir, base_name) + ".gif";
	string cmd("neato -Tcmapx -o" + filename_map + " -Tgif -o" + filename_gif + " " + filename_dot);
	cout<<cmd<<endl;
	system(cmd.c_str());
	
	// write html file
	ofstream out_html;
	string filename_html = filePath(outdir, base_name) + ".html";
	out_html.open(filename_html.c_str());
	if(!out_html.is_open())
	{
		throw string("Unable to open file \"" + filename_html + "\".");
	}
	out_html<<"<FONT COLOR=\"green\"><b>-- Shortest path &nbsp</FONT>"<<endl;
	out_html<<"<FONT COLOR=\"red\">-- Critical edges (without emergency path) on shortest path &nbsp</FONT>"<<endl;
	out_html<<"<FONT COLOR=\"blue\">-- Edge marked as broken</b></FONT><br />"<<endl;
	out_html<<"<IMG SRC=" + base_name + ".gif USEMAP=\"#G\" />"<<endl;
	ifstream in_map;
	in_map.open(filename_map.c_str());
	if(!in_map.is_open())
	{
		throw string("Unable to open file \"" + filename_map + "\".");
	}
	istreambuf_iterator<char> src(in_map.rdbuf());
	istreambuf_iterator<char> end;
	ostream_iterator<char> dest(out_html);
	copy(src, end, dest);
	in_map.close();
	// here the map file can be removed, but is it necessary?
	out_html.close();
}

void Graph::newReportFile(const string& outdir, Vertex v_source, Vertex v_target)
{
	report.open(filePath(outdir, "report.txt").c_str(), fstream::out | fstream::trunc);
	report << "Shortest path: ";
	appendLine(v_source, v_target);
}

void Graph::appendBridgeLine(Vertex v1, Vertex v2)
{
	report << getVertexName(v1) << "--" << getVertexName(v2) << " | No emergency path" << endl;
}

void Graph::appendEmergencyLine(Vertex v1, Vertex v2, Vertex v_source, Vertex v_target)
{
	report << getVertexName(v1) << "--" << getVertexName(v2) << " | ";
	appendLine(v_source, v_target);
}

void Graph::appendLine(Vertex v_source, Vertex v_target)
{
	double cost;
	VertexList shortest;
	tie(shortest, cost) = readShortestVertexPath(v_source, v_target);
	string path="";
	BOOST_FOREACH(Vertex v, shortest)
	{
		path+=getVertexName(v) + "--";
	}
	report << path.substr(0, path.length()-2) << " Cost: " << cost << endl;
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

Graph::EdgeList Graph::findShortestPath(const Vertex v1, const Vertex v2) throw(string)
{
	if(!computeDistances(v1, v2))
		return EdgeList();	// return empty list when v2 is inaccessible from v1
	else
		return readShortestPath(v1, v2);
}

bool Graph::computeDistances(const Vertex v1, const Vertex v2) throw(string)
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
			//DEBUGprint2();
			return true;
		}
		
		OutEdgeIter e, e_end;
		tie(e,e_end) = out_edges(v_source, graph);
		for(; e!=e_end; ++e){
			Vertex v_target = target(*e, graph);
			double tmp_edge_weight = get(edge_weight, graph, *e);
			if(tmp_edge_weight < 0.0)
				throw string("Negative edge weight encountered.");
			double target_distance = get(vertex_distance, graph, v_target);
			if( target_distance > source_distance + tmp_edge_weight )
			{
				put(vertex_distance, graph, v_target, source_distance + tmp_edge_weight);
				put(vertex_predecessor, graph, v_target, v_source);
			}
		}
	}
	
	// graph does not contain vertex v2
	return false;
}

pair<Graph::VertexList, double> Graph::readShortestVertexPath(const Vertex v1, const Vertex v2)
{
	double cost = 0;
	VertexList result;
	Vertex v = v2;
	while(v!=v1)
	{
		Vertex new_v = get(vertex_predecessor, graph, v);
		result.push_front(v);
		cost += getEdgeWeight(edge(v, new_v, graph).first);
		v = new_v;		
	}
	result.push_front(v1);
	return make_pair(result, cost);
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
	//DEBUGprint(result);
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

void Graph::DEBUGprint2()
{
	cout << "Distances:" << endl;
	VertexIter v, v_end;
	tie(v,v_end) = vertices(graph);
	for(; v!=v_end; v++)
	{
		cout << get(vertex_name, graph, *v) << ": " << get(vertex_distance, graph, *v) << endl;
	}
}
