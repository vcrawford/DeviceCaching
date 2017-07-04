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

   // contact graph we will be using
   string filename = "contact_graph.txt";
   Graph graph = Graph(filename);
   int num_nodes = graph.getNodeCount();

   CacheGraph cg = CacheGraph(graph);

   // p used for all algorithms, epsilon for greedy
   float p = stof(argv[1]);
   float epsilon = 0.0000001;

   // Time each algorithm
   int start, end;

   // GREEDY

   cout << "Running the greedy algorithm for minimum file placement to get a cache hit rate "
        << "of " << p << " with epsilon " << epsilon << " ..." << endl;

   vector<int> greedy_nodes;

   start = time(NULL);
   float gamma = greedy(cg, greedy_nodes, p, epsilon);
   end = time(NULL);

   cout << greedy_nodes.size() << " nodes should cache for a cache hit rate of " << gamma << endl;
   cout << "Greedy time: " << end - start << " seconds" << endl; 

   ofstream write_greedy;
   write_greedy.open("cache_greedy.txt");

   for (int i = 0; i < greedy_nodes.size(); i++) {

      write_greedy << greedy_nodes[i] << ",";
   }
  
   write_greedy.close();

   // EXACT

   cout << "Running the exact algorithm for minimum file placement to get a cache hit rate "
        << "of " << p << " ..." << endl;

   vector<int> cache_exact;
   start = time(NULL);
   gamma = exact::exact(cg, cache_exact, p);
   end = time(NULL);

   cout << cache_exact.size() << " nodes should cache for a cache hit rate of " << gamma << endl;
   cout << "Exact time: " << end - start << " seconds" << endl; 

   ofstream write_exact;
   write_exact.open("cache_exact.txt");

   for (int i = 0; i < cache_exact.size(); i++) {

      write_exact << cache_exact[i] << ",";
   }

   write_exact.close();
   
   // EXACT FAST

   cout << "Running the fast exact algorithm for minimum file placement to get a cache hit rate "
        << "of " << p << " ..." << endl;

   start = time(NULL);
   gamma = fast_exact::exact(cg, cache_exact, p);
   end = time(NULL);

   cout << cache_exact.size() << " nodes should cache for a cache hit rate of " << gamma << endl;
   cout << "Fast exact time: " << end - start << " seconds" << endl; 

   write_exact.open("cache_fast_exact.txt");

   for (int i = 0; i < cache_exact.size(); i++) {

      write_exact << cache_exact[i] << ",";
   }

   write_exact.close();

  // EXACT GREEDY

   cout << "Running the greedy exact algorithm for minimum file placement to get a cache hit rate "
        << "of " << p << " ..." << endl;

   start = time(NULL);
   gamma = greedy_exact::exact(cg, cache_exact, p);
   end = time(NULL);

   cout << cache_exact.size() << " nodes should cache for a cache hit rate of " << gamma << endl;
   cout << "Greedy exact time: " << end - start << " seconds" << endl; 

   write_exact.open("cache_greedy_exact.txt");

   for (int i = 0; i < cache_exact.size(); i++) {

      write_exact << cache_exact[i] << ",";
   }

   write_exact.close();
 
}

