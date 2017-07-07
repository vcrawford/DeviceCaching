// Generate a random Barabasi-Albert graph

#include <string>
#include <iostream>
#include <random>
#include <vector>
#include <fstream>
#include <stdexcept>
#include <iomanip>
#include "RandomGraphUtil.cpp"

using namespace std;

int main(int argc, char** argv) {

   cout << endl << "=== GENERATING BARABASI-ALBERT GRAPH ===" << endl;

   int n = stoi(argv[1]); // number of nodes

   int d = stoi(argv[2]); // desired average degree

   if ((d%2 != 0) || (d > n-1)) throw invalid_argument("For BA graph, d must be even and less than n-1");

   cout << "BA graph has " << n << " nodes" << endl;

   // we first generate a lattice of size d where each node has two connections
   vector< vector<double> > graph;

   random_graph_util::lattice(graph, d, 2);

   // Want to keep track of each node's degree, since that determines its probability for new edges

   vector<int> degrees; // degree of ith node
   int sum_degrees = 0; // the sum of all degrees (twice the number of edges)

   for (int i = 0; i < graph.size(); i++) {

      int degree = 0;

      for (int j = 0; j < graph[i].size(); j++) {

         if (graph[i][j] != 0) {

            degree++;
            sum_degrees++;
         }
      }

      degrees.push_back(degree);
   }

   // Then we iteratively add a node and connect it with d/2 other nodes

   default_random_engine gen (time(NULL));

   for (int j = d+1; j < n; j++) {
      // adding the j+1th node

      vector<double> new_row (graph.size() + 1, 0); // one bigger than rest because of self
      uniform_int_distribution<int> dist (0, sum_degrees);

      for (int i = 0; i < d/2; i++) {

         // get a random node according to its degree
         int rand_node = j;

         while((rand_node == j) || (new_row[rand_node] != 0)) { 

            int rand = dist(gen);

            // this is just for probability
            int deg_sum_so_far = 0;

            for (int k = 0; k < graph.size(); k++) {

               if (rand < deg_sum_so_far + degrees[k]) {

                  rand_node = k;
                  break;
               }

              deg_sum_so_far = deg_sum_so_far + degrees[k];
            }
         }

         new_row[rand_node] = 1;
      }

      // Add this node's column
      for (int i = 0; i < graph.size(); i++) {

         graph[i].push_back(new_row[i]); 

         if (new_row[i] != 0) {

            // adjust the degree
            degrees[i]++;
         }
      }

      // all nodes start out with the same degree
      degrees.push_back(d/2);
      sum_degrees += d;

      // add this row
      graph.push_back(new_row);

   }

   random_graph_util::writeGraph("a_test.txt", graph);

   cout << "The average degree is " << sum_degrees/double(n) << endl;

   cout << "=== COMPLETED GENERATING BA GRAPH===" <<endl << endl;
}


