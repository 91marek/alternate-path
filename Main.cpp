#include <boost/program_options.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>	// create_directory()
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
	string outdir;
	bool no_report = false;
	bool no_html = false;

	// Declare the supported options.
	options_description desc("Usage: " + string(argv[0]) + " [options]\nOptions:");
	desc.add_options()
		("help,h", "Produce help message")
		("file,f", value<string>(&filename), "Input graph filename (obligatory)")
		("source,s", value<string>(&source_vertex), "Source vertex name (obligatory)")
		("target,t", value<string>(&target_vertex), "Target vertex name (obligatory)")
		("outdir,o", value<string>(&outdir), "Output directory name (obligatory)")
		("no-report", value<bool>(&no_report)->zero_tokens(), "If specified, report won't be generated")
		("no-html", value<bool>(&no_html)->zero_tokens(), "If specified, HTML won't be generated")
	;
	positional_options_description p;
	p.add("file", -1);
	variables_map vm;
	store(command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
	notify(vm);

	if(vm.count("help") || !vm.count("file") ||
		!vm.count("source") || !vm.count("target") || !vm.count("outdir"))
	{
		cerr << desc << endl;
		return EXIT_FAILURE;
	}
	
	if(no_report)
		cout << "[INFO] Report won't be generated." << endl;
	
	if(no_html)
		cout << "[INFO] HTML won't be generated." << endl;
	
	if(source_vertex == target_vertex)
	{
		cerr<<"Source vertex must be different from target vertex."<<endl;
		return EXIT_FAILURE;
	}
	
	/* creating output directory */
	boost::filesystem::path dir(outdir);
	if(!boost::filesystem::exists(dir))
	{
		if(!boost::filesystem::create_directory(dir))
		{
			cerr<<"Cannot create directory \""+dir.string()+"\"."<<endl;
			return EXIT_FAILURE;
		}
	}
	else if (!boost::filesystem::is_directory(dir))
	{
		cerr<<"Cannot create directory \""+dir.string()+"\". File already exists."<<endl;
		return EXIT_FAILURE;
	}
	outdir = dir.string();
	
	/* reading input file */
	Graph g;
	ifstream in(filename.c_str());
	if(!in.is_open())
	{
		cerr<<"Unable to open file \""<<filename<<"\"."<<endl;
		return EXIT_FAILURE;
	}
	try
	{
		g.readGraph(in);
	}
	catch(std::exception& e)
	{
		cerr << "Error while reading graph file occurred:" << endl;
		cerr << e.what() << endl;
		return EXIT_FAILURE;
	}
	in.close();
	
	try
	{
		/* find source and target on graph */
		pair<Graph::Vertex, Graph::Vertex> v_desc;
		v_desc = g.getVerticesByName(source_vertex, target_vertex);
		
		/* find reference path */
		Graph::EdgeList shortest = g.findShortestPath(v_desc.first, v_desc.second);
		
		/* save length of shortest path */
		const string path_len_filename = "path_len.out";
		ofstream path_len(path_len_filename.c_str());
		if(!path_len.is_open())
		{
			cerr<<"Unable to open file \""<<path_len_filename<<"\"."<<endl;
			return EXIT_FAILURE;
		}
		path_len<<shortest.size()<<endl;
		path_len.close();
	
		if (shortest.empty())	// there is no path between given vertices
		{
			cerr<<"There is no path between \""<<source_vertex<<"\" and \""<<target_vertex<<"\"."<<endl;
			return EXIT_FAILURE;
		}
		
		Graph::EdgeList red_list = Graph::EdgeList();
		Graph::EdgeList green_list = Graph::EdgeList();
		if(!no_report)
			g.newReportFile(outdir, v_desc.first, v_desc.second);
		unsigned i=0;
		BOOST_FOREACH(Graph::Edge e, shortest)
		{
			/* get current edge params */
			Graph::Vertex curr_edge_end1 = g.getSourceVertex(e);
			Graph::Vertex curr_edge_end2 = g.getTargetVertex(e);
			double curr_edge_weight = g.getEdgeWeight(e);
	
			/* remove current edge */
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
				if(!no_report)
					g.appendBridgeLine(curr_edge_end2, curr_edge_end1);
			}
			else // current edge has alternatives
			{
				green_list.push_back(restored);
				g.setEdgeURL(restored, "index.html");
				g.setEdgeColor(restored, Graph::BLUE);	// blue broken edge
				g.setEdgesColor(new_shortest, Graph::GREEN);	// green alternate path
				if(!no_html)
					g.generateHTML("path" + lexical_cast<string>(++i), outdir);	// i starts from 1
				if(!no_report)
					g.appendEmergencyLine(curr_edge_end2, curr_edge_end1, v_desc.first, v_desc.second);
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
		if(!no_html)
			g.generateHTML("index", outdir);
	}
	catch(const string& err)
	{
		cerr<<err<<endl;
		return EXIT_FAILURE;
	}
	
	return EXIT_SUCCESS;
}
