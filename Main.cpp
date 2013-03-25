#include <boost/program_options.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <fstream>
#include "Graph.hpp"

using namespace std;
using namespace boost;
using namespace boost::program_options;
	
int main(int argc, const char* argv[])
{
	string filename;
	string source_vertex;
	string target_vertex;

	// Declare the supported options.
	options_description desc("Usage: " + string(argv[0]) + " [options]\nOptions:");
	desc.add_options()
		("help,h", "Produce help message")
		("file,f", value<string>(&filename), "Input graph filename")
		("source,s", value<string>(&source_vertex), "Source vertex name")
		("target,t", value<string>(&target_vertex), "Target vertex name")
	;
	positional_options_description p;
	p.add("file", -1);
	variables_map vm;
	store(command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
	notify(vm);

	if(vm.count("help") || !vm.count("file") ||
		!vm.count("source") || !vm.count("target"))
	{
		cerr << desc << endl;
		return EXIT_FAILURE;
	}
	
	if(source_vertex == target_vertex)
	{
		cerr<<"Source vertex must be different from target vertex."<<endl;
		return EXIT_FAILURE;
	}
	
	//reading
	Graph g;
	ifstream in(filename.c_str());
	if(!in.is_open())
	{
		cerr<<"Unable to open file \""<<filename<<"\"."<<endl;
		return EXIT_FAILURE;
	}
	g.readGraph(in);
	in.close();
	
	pair<Graph::Vertex, Graph::Vertex> v_desc;
	try
	{
		v_desc = g.getVerticesByName(source_vertex, target_vertex);
	}
	catch(const string& err)
	{
		cerr<<err<<endl;
		return EXIT_FAILURE;
	}
	
	/* find reference path */
	Graph::EdgeList shortest = g.findShortestPath(v_desc.first, v_desc.second);
	Graph::EdgeList red_list = Graph::EdgeList();
	Graph::EdgeList green_list = Graph::EdgeList();
	
	/* there is no path between given vertexes */
	if (shortest.size() == 0)
	{
		cerr<<"There is no path between "<< source_vertex << " and " << target_vertex<<endl;
		return EXIT_FAILURE;
	}
	system ("make clear"); 	//TODO for windows probably it won't work
	try
	{
		unsigned i=0;
		BOOST_FOREACH(Graph::Edge e, shortest)
		{
			/* get current edge params */
			Graph::Vertex curr_edge_end1 = g.getSourceVertex(e);
			Graph::Vertex curr_edge_end2 = g.getTargetVertex(e);
			double curr_edge_weight = g.getEdgeWeight(e);
	
			/* remove current edge */
			//cout<<"I remove the edge: ("<<g.getVertexName(curr_edge_end1)<<", "
				//<<g.getVertexName(curr_edge_end2)<<")"<<endl;
			g.removeEdge(curr_edge_end1, curr_edge_end2);
		
			/* find new shortest path */
			Graph::EdgeList new_shortest = g.findShortestPath(v_desc.first, v_desc.second);
		
			/* restore current edge */
			Graph::Edge restored = g.addEdge(curr_edge_end1, curr_edge_end2);
			g.setEdgeWeight(restored, curr_edge_weight);
		
			if(new_shortest.empty()) // current edge is bridge (will be red only in main picture)
			{
				red_list.push_back(restored);
				g.setEdgeURL(restored, "");
				g.setEdgeColor(restored, Graph::BLACK);
			}
			else // current edge has alternatives
			{
				green_list.push_back(restored);
				g.setEdgeURL(restored, "index.html");
				g.setEdgeColor(restored, Graph::BLUE);	// blue broken edge
				g.setEdgesColor(new_shortest, Graph::GREEN);	// green alternate path
				g.generateHTML("path" + lexical_cast<string>(++i));	// i starts from 1
				/* clean all up */
				g.setEdgeURL(restored, "");
				g.setEdgeColor(restored, Graph::BLACK);
				g.setEdgesColor(new_shortest, Graph::BLACK);
			}
		}
	
		/* prepare and save main picture */
		g.setEdgesColor(red_list, Graph::RED);
		g.setEdgesColor(green_list, Graph::GREEN);
		g.setEdgesURL(green_list, "path", 1);
		g.generateHTML("index");
	}
	catch(const string& err)
	{
		cerr<<err<<endl;
		system ("make clear"); //TODO for windows probably it won't work
		return EXIT_FAILURE;
	}
	
	return EXIT_SUCCESS;
}
