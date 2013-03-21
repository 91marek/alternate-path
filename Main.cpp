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
	//TODO: check stream
	g.readGraph(in);
	
	//calculating
	cout<<"Szukam pomiedzy: "<<source_vertex<< " a "<<target_vertex<<endl;
	Graph::GraphContainer& container = g.getGraphContainer();
	
	std::pair<Graph::Vertex, Graph::Vertex> v_desc = g.getVerticesByName(source_vertex, target_vertex);
	cout<<"Deskryptory: "<<v_desc.first<<" i "<<v_desc.second<<endl;
	
	Graph::EdgeList shortest = g.getShortestPath(v_desc.first, v_desc.second);
	g.setEdgesColor(shortest, Graph::GREEN);// TODO niektore mialy byc na czerwono, bo nie ma zapasowej sciezki
	g.generateHTML("index.html"); //TODO dobra nazwa pliku?
	g.setEdgesColor(shortest, Graph::BLACK);
	
	BOOST_FOREACH(Graph::Edge e, shortest)
	{
		/* get old edge params */
		Graph::Vertex old_edge_end1, old_edge_end2;
		old_edge_end1 = source(e, container);
		old_edge_end2 = target(e, container);
		double old_edge_weight = get(edge_weight, container, e);
		string old_edge_name = get(edge_name, container, e);
	
		/* remove current edge */
		cout<<"Usuwam krawedz"<<e;
		remove_edge(old_edge_end1, old_edge_end2, container);
		g.writeGraph(std::cout);
		/* find new shortest path */
		Graph::EdgeList new_shortest = g.getShortestPath(v_desc.first, v_desc.second);
		g.setEdgesColor(new_shortest, Graph::GREEN);
		/* restore edge */
		Graph::Edge restored = add_edge(old_edge_end1, old_edge_end2, container).first;
		put(edge_weight, container, restored, old_edge_weight); //restore its weight
		put(edge_name, container, restored, old_edge_name); //restore its URL (TODO prawdopodobnie jest to niepotrzebne)
		g.setEdgeColor(restored, Graph::BLUE);
		
		g.generateHTML(old_edge_name); //TODO dobra nazwa pliku?
		
		/* clean all up */
		g.setEdgesColor(new_shortest, Graph::BLACK);
		g.setEdgeColor(restored, Graph::BLACK);
	
	}
	//writing
	//g.writeGraph(std::cout);
	
	return 0;
}
