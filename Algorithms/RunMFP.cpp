// Run main algorithm

#include <unordered_set>
#include <map>
#include <list>
#include <fstream>
#include <vector>
#include <iostream>
#include <string>
#include <cmath>
#include <iomanip>
#include <stdexcept>
#include <ctime>
#include "Graph.cpp"
#include "GammaUtil.cpp"
#include "CacheGraph.cpp"
#include "Greedy.cpp"
#include "Exact.cpp"
#include "ExactFast.cpp"
#include "ExactGreedy.cpp"

using namespace std;

int main(int argc, char** argv) {

   cout << endl << "==== RUNNING ALGORITHMS ====" << endl;

   // p used for all algorithms, epsilon for greedy
   float p = stof(argv[1]);
   string output_filename = argv[2]; // file we will append results to
   string contact_filename = argv[3]; // where the contact graph is stored
   int run_exact = stoi(argv[4]); // whether we run exact

   fstream out;
   out.open(output_filename, fstream::out | fstream::app);

   // get contact graph
   Graph graph = Graph(contact_filename);
   int n = graph.getNodeCount();

   vector<bool> can_cache;
   // assume all nodes can cache
   for (int i = 0; i < n; i++) {

      can_cache.push_back(true);
   }

   CacheGraph cg = CacheGraph(graph, can_cache);

   // Time each algorithm
   int start, end;

   // the cache hit rate it gets
   float gamma;

   // GREEDY

   // these are the three epsilons we will do
   double epsilons [1] = {0.001};

   for (int i = 0; i < 1; i++) {
      cout << "Running greedy algorithm with epsilon " << epsilons[i] << " ..." << endl;
      vector<int> greedy_nodes;

      start = time(NULL);
      gamma = greedy(cg, greedy_nodes, p, epsilons[i]);
      end = time(NULL);

      out << "<greedy" << i << ">\n <n>" << n << "</n>\n" << " <size>" << greedy_nodes.size() << "</size>\n"
          << " <time>" << end-start << "</time>\n" << " <gamma>" << gamma << "</gamma>\n"
          << " <epsilon>" << epsilons[i] << "</epsilon>\n";

      out << " <cache>";
      for (int i = 0; i < greedy_nodes.size(); i++) {

         out << greedy_nodes[i] << ",";
      }
      out << "</cache>\n";

      out << "</greedy" << i << ">\n";

      out.flush();
   }

   if (run_exact == 1) {   
      // EXACT FAST
      cout << "Running exact algorithm ..." << endl;
      vector<int> cache_exact;

      start = time(NULL);
      gamma = fast_exact::exact(cg, cache_exact, p);
      end = time(NULL);
      out << "<exact>\n <n>" << n << "</n>\n" << " <size>" << cache_exact.size() << "</size>\n"
          << " <time>" << end-start << "</time>\n <gamma>" << gamma << "</gamma>\n";

      out << " <cache>";
      for (int i = 0; i < cache_exact.size(); i++) {

         out << cache_exact[i] << ",";
      }
      out << "</cache>\n";
 
      out << "</exact>\n";
   }

   out.close();

   cout << "=== COMPLETED ALGORITHMS ===" << endl << endl;
}

