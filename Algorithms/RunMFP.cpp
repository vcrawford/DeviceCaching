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
   float epsilon = stof(argv[2]);
   string output_filename = argv[3]; // file we will append results to
   string contact_filename = argv[4]; // where the contact graph is stored
   int run_exact = stoi(argv[5]); // whether we run exact

   fstream out;
   out.open(output_filename, fstream::out | fstream::app);

   // get contact graph
   Graph graph = Graph(contact_filename);
   int n = graph.getNodeCount();
   CacheGraph cg = CacheGraph(graph);

   // Time each algorithm
   int start, end;

   // GREEDY

   cout << "Running greedy algorithm ..." << endl;
   vector<int> greedy_nodes;

   start = time(NULL);
   float gamma = greedy(cg, greedy_nodes, p, epsilon);
   end = time(NULL);

   out << "<greedy>\n <n>" << n << "</n>\n" << " <size>" << greedy_nodes.size() << "</size>\n"
       << " <time>" << end-start << "</time>\n";

   out << " <cache>";
   for (int i = 0; i < greedy_nodes.size(); i++) {

      out << greedy_nodes[i] << ",";
   }
   out << "</cache>\n";

   out << "</greedy>\n";

   out.flush();

   if (run_exact == 1) {   
      // EXACT FAST
      cout << "Running exact algorithm ..." << endl;
      vector<int> cache_exact;

      start = time(NULL);
      gamma = fast_exact::exact(cg, cache_exact, p);
      end = time(NULL);
      out << "<exact>\n <n>" << n << "</n>\n" << " <size>" << cache_exact.size() << "</size>\n"
          << " <time>" << end-start << "</time>\n</exact>\n";
   }

   out.close();

   cout << "=== COMPLETED ALGORITHMS ===" << endl << endl;
}

