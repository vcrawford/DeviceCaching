#include <fstream>
#include <string>
#include <vector>
#include <random>
#include <iostream>
#include <ctime>

using namespace std;

// Generate a random ER graph

int main(int argc, char** argv) {

   cout << endl << "=== GENERATING RANDOM ER GRAPH ===" << endl;

   // the number of nodes
   int n = stoi(argv[1]);

   // the desired average node degree
   int deg = stoi(argv[2]);

   cout << n << " nodes with expected average degree of " << deg << endl;

   if (deg > (n - 1)) {

      throw "Error in random erdos-renyi graph generation: The average degree must be at most n-1";
   }

   // get the probability from the average node degree
   double p = double(deg)/(n - 1);
 
   // the probability of edges
   //double p = stof(argv[2]);


   // where to write the graph matrix
   string output_filename = argv[3];

   ofstream output;
   output.open(output_filename);

   // used to determine if there exists an edge, and also what its weight is
   default_random_engine generator (time(NULL));
   uniform_real_distribution<double> dist (0.0, 1.0);

   vector< vector<double> > graph (n, vector<double> (n, 0));

   // generate the graph
   for (int i = 0; i < n; i++) {

      for (int j = 0; j < i; j++) {

         if (dist(generator) < p) {

            // there will be an edge with random weight
            double edge_weight = dist(generator);

            graph[i][j] = edge_weight;
            graph[j][i] = edge_weight;
         }
      }
   }

   // write to file
   for (int i = 0; i < n; i++) {

      for (int j = 0; j < n; j++) {

         output << graph[i][j] << " ";
      }

      output << endl;
   }

   output.close();

   cout << "=== COMPLETED GENERATE RANDOM ER GRAPH ===" << endl;
}


