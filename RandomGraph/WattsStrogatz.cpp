// Generate a random Watts-Strogatz graph

#include <math.h>
#include <string>
#include <stdexcept>
#include <vector>
#include <fstream>
#include <random>
#include <iostream>
#include <iomanip>
#include "RandomGraphUtil.cpp"

using namespace std;

int main(int argc, char** argv) {

   cout << endl << "=== GENERATING RANDOM WS GRAPH ===" << endl;

   // number of nodes
   int n = stoi(argv[1]);

   // the mean degree
   int k = stoi(argv[2]);

   // parameter beta: re-wiring probability
   double beta = stof(argv[3]);

   cout << n << " nodes with average degree " << k << " and beta parameter " << beta << endl;

   // where to save the graph
   string filename = argv[4];

   if ((k >= n) || (k%2 != 0) || (k < log(n)) || (beta > 1) || (beta < 0)) {

      throw invalid_argument("Invalid input to Watts-Strogatz graph");
   }

   // construct lattice

   vector< vector<double> > graph;
   random_graph_util::lattice(graph, n, k);

   // re-wire each edge with probability beta

   default_random_engine gen (time(NULL));

   // chooses whether we re-wire
   uniform_real_distribution<double> dist_rewire (0, 1);

   // chooses which other vertex to re-wire to
   uniform_int_distribution<int> dist_node (0, n-1);

   // original edges are 1
   // rewired edges are -1 (so we don't rewire twice)
   for (int i = 0; i < n; i++) {

      for (int j = 0; j < i; j++) {

         if (graph[i][j] == 1) {

            // here is an edge

            // should we re-wire?
            bool rewire = (dist_rewire(gen) < beta);

            if (rewire) {

               // pick another edge
               int k = dist_node(gen);

               while (graph[i][k] != 0) {

                  // must re-wire to where there is no edge
                  k = dist_node(gen);
               }

               // rewire
               graph[i][j] = 0;
               graph[j][i] = 0;
               graph[i][k] = -1;
               graph[k][i] = -1;
            }
         }
      }
   }

   // Now pick edge weights between 0 and 1 for every edge
   random_graph_util::edgeWeights(graph);

   // write graph to file
   random_graph_util::writeGraph(filename, graph);

   cout << "=== GENERATING RANDOM WS GRAPH COMPLETE ===" << endl << endl;
}


