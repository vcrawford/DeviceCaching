// Generate a random Watts-Strogatz graph

#include <math.h>
#include <string>
#include <stdexcept>
#include <vector>
#include <fstream>
#include <random>
#include <iostream>

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

   vector< vector<double> > graph (n, vector<double> (n, 0));

   for (int i = 0; i < n; i++) {

      for (int j = i - k/2; j < i; j++) {

         if (j < 0) {

            graph[i][n + j] = 1;
            graph[n + j][i] = 1;
         }
         else {

            graph[i][j] = 1;
            graph[j][i] = 1;
         }
      }
   }

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

   for (int i = 0; i < n; i++) {

      for (int j = 0; j < i; j++) {

         if (graph[i][j] != 0) {

            double edge_weight = dist_rewire(gen);
            graph[i][j] = edge_weight;
            graph[j][i] = edge_weight;
         }
      }
   }

   // write graph to file

   ofstream file_strm;
   file_strm.open(filename); 

   for (int i = 0; i < n; i++) {

      for (int j = 0; j < n; j++) {

         file_strm << graph[i][j] << " ";
      }

      file_strm << endl;
   }

   file_strm.close();

   cout << "=== GENERATING RANDOM WS GRAPH COMPLETE ===" << endl << endl;
}


