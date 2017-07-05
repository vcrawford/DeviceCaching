// Simulates a bunch of devices moving and requesting a single file
// This is meant to be used with a contact graph, from which the probability of
// two devices being in contact is gotten

#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>
#include <random>
#include <string>

using namespace std;

int main(int argc, char** argv) {

   cout << endl << "=== Running Cache Simulation ===" << endl;

   // The file that has the contact graph
   string graph_file = argv[1];
   ifstream graph_strm;
   graph_strm.open(graph_file);

   // the number of seconds to run the simulation
   int total_sec = stoi(argv[2]);

   // the probability at any time step that there will be a file request
   double request_prob = stof(argv[3]);

   // now fill in this graph vector
   vector< vector<double> > graph;
   string line;
   stringstream line_strm;
   double val;

   while(getline(graph_strm, line)) {

      line_strm.clear();
      line_strm.str(line);

      vector<double> node_line;

      while (line_strm >> val) {

         node_line.push_back(val);
      }

      graph.push_back(node_line);
   }

   // Now read in the cache nodes

   string cache_file = argv[4];
   ifstream cache_strm;
   cache_strm.open(cache_file);

   string cache_str;
   getline(cache_strm, cache_str);

   vector<int> cache;
   int search_start = 0;
   int pos;

   while ((pos = cache_str.find(",", search_start)) != string::npos) {

      cache.push_back(stoi(cache_str.substr(search_start, pos - search_start)));
      search_start = pos + 1;
   }

   cache_strm.close();

   // random number generators for file request

   mt19937 generator(random_device{}());

   // This is used to determine if there is a request at all, and also to determine if an edge exists
   uniform_real_distribution<double> unif_0_1 (0, 1);

   // This is used to determine which device requested the file
   uniform_int_distribution<int> unif_n (0, graph.size() - 1);

   // Now simulate

   int num_requests = 0;
   int num_hits = 0;

   for (int sec = 0; sec < total_sec; sec++) {

      // Is there a request?
      bool is_request = ((unif_0_1(generator)) < request_prob);
         
      if (is_request) {

         num_requests++;

         // which node?
         int requesting_node = unif_n(generator);

         // check if it's in contact with a cache node
         for (int j = 0; j < cache.size(); j++) {

            bool is_hit = (unif_0_1(generator) < graph[requesting_node][cache[j]]); 

            if (is_hit || (requesting_node == cache[j])) {

               num_hits++;
               break;
            }
         }
      }
   }


   cout << "Cache hit rate is " << (double(num_hits))/num_requests << endl;

   cout << "=== Ending Cache Simulation ===" << endl << endl;
}


