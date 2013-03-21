#include <boost/program_options.hpp>
#include <boost/foreach.hpp>
#include <iostream>
#include <fstream>
#include "Graph.hpp"

using namespace boost;
using namespace boost::program_options;
	
int main(int argc, const char* argv[])
{
	std::string filename;
	std::string source_vertex;
	std::string target_vertex;

	// Declare the supported options.
	options_description desc("Usage: " + std::string(argv[0]) + " [options]\nOptions:");
	desc.add_options()
		("help,h", "Produce help message")
		("file,f", value<std::string>(&filename), "Input graph filename")
		("source,s", value<std::string>(&source_vertex), "Source vertex name")
		("target,t", value<std::string>(&target_vertex), "Target vertex name")
	;
	positional_options_description p;
	p.add("file", -1);
	variables_map vm;
	store(command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
	notify(vm);

	if (vm.count("help") || !vm.count("file") ||
		!vm.count("source") || !vm.count("target"))
	{
		std::cout << desc << std::endl;
		return 1;
	}
	
	std::cout << "f: " << filename << "\ns: " << source_vertex << "\nt: " << target_vertex << std::endl;
	
	// Use ref_property_map to turn a graph property into a property map
	//boost::ref_property_map<graph_t*,std::string>
		//gname(get_property(graph,graph_name));
	//dp.property("name",gname);
	
	//reading
	Graph g;
	std::ifstream in(filename.c_str());
	if(!in.is_open())
	{
		cout<<"Unable to open file "<<filename<<endl;
		return -1;
	}
	g.readGraph(in);
	
	cout<<"Szukam pomiedzy: "<<source_vertex<< " a "<<target_vertex<<endl;
	
	std::pair<Graph::Vertex, Graph::Vertex> v_desc;
	try
	{
		v_desc = g.getVerticesByName(source_vertex, target_vertex);
	}
	catch(string err)
	{
		cout<<err<<endl;
		return -1;
	}
	cout<<"Deskryptory: "<<v_desc.first<<" i "<<v_desc.second<<endl;
	
	Graph::EdgeList shortest = g.findShortestPath(v_desc.first, v_desc.second);
	g.setEdgesColor(shortest, Graph::GREEN);// TODO niektore mialy byc na czerwono, bo nie ma zapasowej sciezki
	g.generateHTML("index.html"); //TODO dobra nazwa pliku? (raczej to trzeba zrobić na koniec)
	g.setEdgesColor(shortest, Graph::BLACK);
	
	BOOST_FOREACH(Graph::Edge e, shortest)
	{
		/* get old edge params */
		Graph::Vertex old_edge_end1 = g.getSourceVertex(e);
		Graph::Vertex old_edge_end2 = g.getTargetVertex(e);
		double old_edge_weight = g.getEdgeWeight(e);
		string old_edge_url = g.getEdgeURL(e);
	
		/* remove current edge */
		cout<<"Usuwam krawedz: "<<e<<endl;
		g.removeEdge(old_edge_end1, old_edge_end2);
		g.writeGraph(std::cout);
		/* find new shortest path */
		Graph::EdgeList new_shortest = g.findShortestPath(v_desc.first, v_desc.second);
		// jeśli new_shortest.empty() to trzeba ustawić kolor rozspójniającej krawędzi
		// (właśnie usuniętej) na czerwony, ale dla głównego obrazka (trzeba zapamiętać to na boku
		// a główny obrazek pokolorować na końcu i dopiero zapisać)
		g.setEdgesColor(new_shortest, Graph::GREEN);
		/* restore edge */
		Graph::Edge restored = g.addEdge(old_edge_end1, old_edge_end2);
		g.setEdgeWeight(restored, old_edge_weight);
		g.setEdgeURL(restored, old_edge_url);
		g.setEdgeColor(restored, Graph::BLUE);
		
		g.generateHTML(old_edge_url); //TODO dobra nazwa pliku?
		
		/* clean all up */
		g.setEdgesColor(new_shortest, Graph::BLACK);
		g.setEdgeColor(restored, Graph::BLACK);
	
	}
	//writing
	//g.writeGraph(std::cout);
	
	return 0;
}
