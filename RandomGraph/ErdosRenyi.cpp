#include <fstream>
#include <string>
#include <vector>
#include <random>
#include <iostream>
#include <ctime>
#include <stdexcept>
#include <iomanip>
#include "RandomGraphUtil.cpp"

using namespace std;

// Generate a random ER graph

int main(int argc, char** argv) {

   cout << endl << "=== GENERATING RANDOM ER GRAPH ===" << endl;

   // the number of nodes
   int n = stoi(argv[1]);

   // the desired average node degree
   int deg = stoi(argv[2]);

   // where to write the graph matrix
   string output_filename = argv[3];

   cout << n << " nodes with expected average degree of " << deg << endl;

   if (deg > (n - 1)) {

      throw "Error in random erdos-renyi graph generation: The average degree must be at most n-1";
   }

   // get the probability from the average node degree
   double p = double(deg)/(n - 1);
 
   // the probability of edges
   //double p = stof(argv[2]);

   // used to determine if there exists an edge, and also what its weight is
   default_random_engine generator (time(NULL));
   uniform_real_distribution<double> dist (0.0, 1.0);

   vector< vector<double> > graph;

   random_graph_util::randomER(graph, n, p);

   random_graph_util::edgeWeights(graph);

   random_graph_util::writeGraph(output_filename, graph); 

   cout << "=== COMPLETED GENERATE RANDOM ER GRAPH ===" << endl;
}


